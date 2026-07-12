#include <stdbool.h>

#define WSA_IMPLEMENTATION
#define STRING_LIB_IMPLEMENTATION
#define ARRAY_LIST_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/array_list.h"
#include "../data_structures/string_lib.h"


#define hash_t  int

typedef struct test_column_t {
    const char *name;
    const char *type;
    bool       nullable;
} test_column_t;

typedef struct test_row_t {
    const char **values;
    size_t     value_count;
} test_row_t;

typedef struct test_table_t {
    const char    *name;

    test_column_t *columns;
    size_t        column_count;

    test_row_t    *rows;
    size_t        row_count;
} test_table_t;

typedef struct test_database_t {
    const char   *dialect;

    test_table_t *tables;
    size_t       table_count;
} test_database_t;


// SchemaSnapshot is a complete, immutable picture of a database's structure
// at a point in time.
typedef struct schema_snapshot_t {
    hash_t       hash;
    string_t     dialect; // source DB: "sqlite", "postgres", "mysql"
    array_list_t table_list; // [dynamic]table_t, list of tables
} schema_snapshot_t;


// Column represents a single column in a table.
typedef struct column_t {
    hash_t   hash;
    string_t col_type;
    bool     nullable;
} column_t;


// Table represents a single database table.
typedef struct table_t {
    hash_t         hash;
    string_t       name;
    array_list_t   col_names; // [dynamic]string_t
    array_list_t   columns; // [dynamic]column_t
    array_list_t   primary_key; // primary key [dynamic]size_t, index to the columns
} table_t;


typedef struct row_t {
    hash_t       hash; // hash of the entire row
    array_list_t values; // [dynamic]string_t, actual row entry serialized to list of strings
} row_t;


typedef struct data_snapshot_t {
    hash_t       hash; // hash of all row hashes; fast change detection
    array_list_t table_data_list; // [dynamic]table_data_t 
} data_snapshot_t;


typedef struct table_data_t {
    array_list_t row_hash_list; // [dynamic]hash_t
} table_data_t;


typedef struct manifest_t {
    schema_snapshot_t schema_snapshot;
    data_snapshot_t   data_snapshot;
} manifest_t;


// Generate manifest from sqlite file
int
generate_state_from_sqlite(
    context_t *context, 
    const test_database_t *db, 
    manifest_t *manifest, 
    array_list_t *row_data);


hash_t 
hash_fn(string_t value);


int 
main(void)
{
    context_t context = context_init(MB(1), KB(512));
    if (!context_is_valid(&context)) goto cleanup;

    /* ============================================================
    * users table
    * ============================================================ */

    test_column_t users_columns[] = {
        {.name = "id", .type = "INTEGER", .nullable = false,},
        {.name = "name", .type = "TEXT", .nullable = false,},
    };

    const char *users_row1_values[] = {"1", "Alice",};
    const char *users_row2_values[] = {"2", "Bob",};

    test_row_t users_rows[] = {
        {.values = users_row1_values, .value_count = 2,},
        {.values = users_row2_values, .value_count = 2,},
    };

    /* ============================================================
    * products table
    * ============================================================ */

    test_column_t products_columns[] = {
        {.name = "id", .type = "INTEGER", .nullable = false,},
        {.name = "name", .type = "TEXT", .nullable = false,},
        {.name = "price", .type = "REAL", .nullable = false,},
    };

    const char *products_row1_values[] = {"1", "Keyboard", "100.50"};

    const char *products_row2_values[] = {"2", "Mouse", "35.75",};

    test_row_t products_rows[] = {
        {.values = products_row1_values, .value_count = 3,},
        {.values = products_row2_values, .value_count = 3,},
    };

    /* ============================================================
    * Database
    * ============================================================ */

    test_table_t tables[] = {
        {.name = "users", .columns = users_columns, .column_count = 2, .rows = users_rows, .row_count = 2,},
        {.name = "products", .columns = products_columns, .column_count = 3, .rows = products_rows, .row_count = 2,},
    };

    test_database_t database = {.dialect = "sqlite", .tables = tables, .table_count = 2,};

    manifest_t manifest   = {0};
    array_list_t row_data = array_list_init_capacity(&(context.allocator), row_t, 100); // [dynamic]row_t
    int ret               = generate_state_from_sqlite(&context, &database, &manifest, &row_data);
    if (!ret) goto cleanup;

    printf("schema snapshot hash: %d\n", manifest.schema_snapshot.hash);
    slice_t cstring = arena_allocator_alloc(&context.temp_allocator, char, manifest.schema_snapshot.dialect.len + 1);
    cstring         = string_lib_cstring_in_slice(&manifest.schema_snapshot.dialect, &cstring);
    printf("schema snapshot dialect: %s\n", (char *)cstring.ptr);
    
    row_t *row_list = (row_t *) row_data.ptr;
    printf("row count: %zu\n", row_data.len);
    for (size_t i = 0; i < row_data.len; i++) {
        string_t *value_list = (string_t *)row_list[i].values.ptr;
        printf("hash: %d: ", row_list[i].hash);
        for (size_t j = 0; j < row_list[i].values.len; j++){
            char temp[64] = {0};
            size_t cap = ((size_t)64 > value_list[j].len)? value_list[j].len : 63;
            memcpy(&temp, value_list[j].ptr, cap);
            printf(" %s | ", temp);
        }
        printf("\n");
    }
    cleanup:
        context_deinit(&context);
    return 0;
}


int
generate_state_from_sqlite(
    context_t *context, 
    const test_database_t *db, 
    manifest_t *manifest, 
    array_list_t *row_data)
{
    int result = 1;
    assert((NULL != db)&&"Database cannot be NULL");
    manifest->schema_snapshot.dialect = string_lib_init_with_strlit(&(context->allocator), db->dialect);
    array_list_t temp_schema_list     = array_list_init_capacity(&(context->allocator), table_t, 10); // [dynamic]table_t
    array_list_t temp_data_list       = array_list_init_capacity(&(context->allocator), hash_t, 10); // [dynamic]table_t
    if (NULL == temp_schema_list.ptr || NULL == manifest->schema_snapshot.dialect.ptr || NULL == temp_data_list.ptr) {result = 0; goto cleanup;}
    for (size_t i = 0; i < db->table_count; i++) {
        table_t temp         = {0};
        temp.name            = string_lib_init_with_strlit(&(context->allocator), db->tables[i].name);
        array_list_t names   = array_list_init_capacity(&(context->allocator), string_t, 3); // [dynamic]string_t
        array_list_t columns = array_list_init_capacity(&(context->allocator), column_t, 3); // [dynamic]column_t
        if (NULL == temp.name.ptr || NULL == names.ptr || NULL == columns.ptr) {result = 0; goto cleanup;}
        for (size_t j = 0; j < db->tables[i].column_count; j++) {
            string_t name = string_lib_init_with_strlit(&(context->allocator), db->tables[i].columns[j].name);
            string_t type = string_lib_init_with_strlit(&(context->allocator), db->tables[i].columns[j].type);
            column_t col = (column_t){
                .col_type = type,
                .nullable = db->tables[i].columns[j].nullable,
                .hash     = hash_fn((string_t){0}),
            };
            int ret = array_list_append_item_fn(&(context->allocator), &columns, (const char*)&col);
            if (0 != ret) {result = 0; goto cleanup;}
            ret = array_list_append_item_fn(&(context->allocator), &names, (const char*)&name);
            if (0 != ret) {result = 0; goto cleanup;}
        }
        assert((names.len == columns.len)&&"names and types should be of equal length");
        memcpy(&temp.columns, &columns, sizeof(array_list_t));
        memcpy(&temp.col_names, &names, sizeof(array_list_t));
        int ret = array_list_append_item_fn(&(context->allocator), &temp_schema_list, (const char *)&temp);
        if (0 != ret) {result = 0; goto cleanup;}
        array_list_t row_hashes = array_list_init_capacity(&(context->allocator), hash_t, 10); // [dynamic]hash_t
        if (NULL == row_hashes.ptr) {result = 0; goto cleanup;}
        for (size_t j = 0; j < db->tables[i].row_count; j++) {
            array_list_t row_entry = array_list_init_capacity(
                &(context->allocator), string_t, db->tables[i].rows[j].value_count); // [dynamic]string_t
            if (NULL == row_entry.ptr) {result = 0; goto cleanup;}
            for (size_t k = 0; k < db->tables[i].rows[j].value_count; k++) {
                string_t temp_val = string_lib_init_with_strlit(&(context->allocator), db->tables[i].rows[j].values[k]);
                if (NULL == temp_val.ptr) {result = 0; goto cleanup;}
                ret = array_list_append_item_fn(&(context->allocator), &row_entry, (const char*)&temp_val);
                if (0 != ret) {result = 0; goto cleanup;}
            }
            row_t row = (row_t){.hash = hash_fn((string_t){0}), .values = row_entry};
            ret       = array_list_append_item_fn(&(context->allocator), row_data, (const char*)&row);
            if (0 != ret) {result = 0; goto cleanup;}
            hash_t hash = hash_fn((string_t){0});
            ret         = array_list_append_item_fn(&(context->allocator), &row_hashes, (const char *)&hash);
            if (0 != ret) {result = 0; goto cleanup;}
        }

        // table data
        table_data_t item = (table_data_t){ .row_hash_list = row_hashes };
        ret               = array_list_append_item_fn(&(context->allocator), &temp_data_list, (const char *)&item);
        if (0 != ret) {result = 0; goto cleanup;}
    }
    memcpy(&manifest->schema_snapshot.table_list, &temp_schema_list, sizeof(array_list_t));
    memcpy(&manifest->data_snapshot.table_data_list, &temp_data_list, sizeof(array_list_t));
    manifest->schema_snapshot.hash = hash_fn((string_t){0});
    manifest->data_snapshot.hash   = hash_fn((string_t){0});

    cleanup:
        arena_allocator_reset(&(context->temp_allocator));
        return result;
}


hash_t 
hash_fn(string_t value)
{
    return 2; // arbitrary number, should be the hash of particular objects (as concatenated string)
}