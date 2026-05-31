#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_MSVC_VER)
    #define u_alignof(x) __alignof(x)
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
    #define u_alignof(x) __alignof__(x)
#endif

typedef struct oops_t{
    uint16_t another_i;
} oops_t;

struct nested{
    uint8_t some_i; 
    oops_t x;
};

struct nested_inv{
    oops_t x;
    uint8_t some_i; 
};


struct SomeStruct{
    size_t x;
    size_t z;
    uint8_t y;
};


struct SomeStruct2{
    uint16_t x;
    size_t y;
    uint16_t z;
};

struct SomeStruct3{
    uint16_t x;
    size_t y;
    uint16_t z;
    uint16_t r;
};

struct SomeStruct4{
    uint16_t x;
    uint16_t z;
    uint16_t r;
    size_t y;
};


struct SomeStruct5{
    size_t y;
    uint16_t x;
    uint16_t z;
    uint16_t r;
};

typedef struct sg_buffer_usage {
    bool vertex_buffer;
    bool index_buffer;
    bool storage_buffer;
    bool immutable;
    bool dynamic_update;
    bool stream_update;
} sg_buffer_usage;


typedef enum{
    SG_USAGE_DEFAULT = 0x00,
    SG_VERTEX_BUFFER = 0x00,
    SG_INDEX_BUFFER = 0x01,
    SG_STORAGE_BUFFER,
    SG_IMMUTABLE,
    SG_DYNAMIC_UPDATE,
    SG_STREAM,
} sg_buffer_usage_variant;


typedef struct sg_buffer_usage_v2 {
    sg_buffer_usage_variant usage_type;
} sg_buffer_usage_v2;

int main(int argc, char *argv[]){
    printf("Here is the size of nested struct: %zu\n", sizeof(struct nested));
    printf("Here is the size of nested_inv struct: %zu\n", sizeof(struct nested_inv));
    printf("Here is the size of SomeStruct: %zu\n", sizeof(struct SomeStruct));
    printf("Here is the size of SomeStruct2: %zu\n", sizeof(struct SomeStruct2));
    printf("Here is the size of SomeStruct3: %zu\n", sizeof(struct SomeStruct3));
    printf("Here is the size of SomeStruct4: %zu\n", sizeof(struct SomeStruct4));
    printf("Here is the size of SomeStruct5: %zu\n", sizeof(struct SomeStruct5));
    printf("Here is the size of sg_buffer_usage: %zu\n", sizeof(sg_buffer_usage));
    printf("Here is the size of sg_buffer_usage_v2: %zu\n", sizeof(sg_buffer_usage_v2));
    printf("Here is the alignment of sg_buffer_usage: %zu\n", u_alignof(sg_buffer_usage));
    sg_buffer_usage_v2 sample;
    sample.usage_type = SG_INDEX_BUFFER;
    return EXIT_SUCCESS;
}