#include <stdio.h>

#define WSA_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"

typedef struct position_soa_t {
    size_t capacity;
    size_t len;
    float *x_pos;
    float *y_pos;
    float *z_pos;
} position_soa_t;


typedef struct position_t {
    float x_pos, y_pos, z_pos;
} position_t;



position_soa_t 
position_soa_init_capacity_fn(arena_allocator_t *allocator, size_t init_capacity)
{
    assert((0 < init_capacity)&&"init size should be > 0");
    slice_t x_pos = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    slice_t y_pos = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    slice_t z_pos = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    if(0 == x_pos.len_in_bytes || 0 == y_pos.len_in_bytes || 0 == z_pos.len_in_bytes) return (position_soa_t){0};
    return (position_soa_t){
        .capacity = init_capacity,
        .len = 0,
        .x_pos = (float *)x_pos.ptr,
        .y_pos = (float *)y_pos.ptr,
        .z_pos = (float *)z_pos.ptr,
    };
}


int 
position_soa_append_item_fn(arena_allocator_t *allocator, position_soa_t *dst, const position_t item)
{
    size_t expected_len = sizeof(float) * (1 + dst->len);
    slice_t x_slice = (slice_t){.len_in_bytes = sizeof(float) * dst->capacity, .ptr = dst->x_pos};
    slice_t y_slice = (slice_t){.len_in_bytes = sizeof(float) * dst->capacity, .ptr = dst->y_pos};
    slice_t z_slice = (slice_t){.len_in_bytes = sizeof(float) * dst->capacity, .ptr = dst->z_pos};
    if (0 == x_slice.len_in_bytes || 0 == y_slice.len_in_bytes || 0 == z_slice.len_in_bytes) return 1;
    if (dst->capacity * sizeof(float) < expected_len){
        x_slice = arena_allocator_resize_aligned(allocator, x_slice, expected_len << 1, sizeof(float), DEFAULT_ALIGNMENT);
        y_slice = arena_allocator_resize_aligned(allocator, y_slice, expected_len << 1, sizeof(float), DEFAULT_ALIGNMENT);
        z_slice = arena_allocator_resize_aligned(allocator, z_slice, expected_len << 1, sizeof(float), DEFAULT_ALIGNMENT);
        if (0 == x_slice.len_in_bytes || 0 == y_slice.len_in_bytes || 0 == z_slice.len_in_bytes) return 1;
        dst->capacity = (1 + dst->len) << 1;
    }
    memmove((void *)&(x_slice.ptr[dst->len * sizeof(float)]), (void *)&item.x_pos, sizeof(float));
    memmove((void *)&(y_slice.ptr[dst->len * sizeof(float)]), (void *)&item.y_pos, sizeof(float));
    memmove((void *)&(z_slice.ptr[dst->len * sizeof(float)]), (void *)&item.z_pos, sizeof(float));
    dst->len += 1;
    dst->x_pos = (float *)x_slice.ptr;
    dst->y_pos = (float *)y_slice.ptr;
    dst->z_pos = (float *)z_slice.ptr;
    return 0;
}


int 
position_soa_append_slice_fn(arena_allocator_t *allocator, position_soa_t *dst, const slice_t position_slice)
{
    position_t *items = (position_t *)position_slice.ptr;
    for (int i = 0; i < position_slice.len_in_bytes/sizeof(position_t); i++) {
        int ret = position_soa_append_item_fn(allocator, dst, items[i]);
        if (0 != ret) return 1;
    }
    return 0;
}


int 
main(int argc, char *argv[])
{
    arena_allocator_t gpa = arena_allocator_init_page_default(c_allocator, KB(1));
    position_soa_t pos_list = position_soa_init_capacity_fn(&gpa, 200);
    if (0 != pos_list.capacity){
        position_t items[] = {
            (position_t){.x_pos = 1.9, .y_pos = 2, .z_pos = 3},
            (position_t){.x_pos = 7, .y_pos = 2.9, .z_pos = 3},
            (position_t){.x_pos = 7, .y_pos = 2.39, .z_pos = 3},
        };
        slice_t position_slice = make_slice(items, sizeof(items));
        int ret = position_soa_append_slice_fn(&gpa, &pos_list, position_slice);
        // x positions
        for (int i = 0; i < pos_list.len; i++){
            printf("pos_list: x_pos[%d]: %f\n", i, pos_list.x_pos[i]);
        }
        // y positions
        for (int i = 0; i < pos_list.len; i++){
            printf("pos_list: y_pos[%d]: %f\n", i, pos_list.y_pos[i]);
        }
    }
    printf("capacity: %lu\n", pos_list.capacity);
    arena_allocator_deinit(&gpa);
    return 0;
}

