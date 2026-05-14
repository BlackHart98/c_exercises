#include <stdio.h>

#define WSA_IMPLEMENTATION
#define ARRAY_LIST_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/array_list.h"


#define DEFAULT_SPEED 2


typedef struct position_soa_t {
    size_t capacity;
    size_t len;
    float *x_pos;
    float *y_pos;
    float *z_pos;
    float *speed;
} position_soa_t;


typedef struct position_t {
    float x_pos, y_pos, z_pos, speed;
} position_t;



position_soa_t 
position_soa_init_capacity_fn(arena_allocator_t *allocator, size_t init_capacity)
{
    assert((0 < init_capacity)&&"init size should be > 0");
    slice_t x_pos = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    slice_t y_pos = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    slice_t z_pos = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    slice_t speed = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    if(0 == x_pos.len_in_bytes || 0 == y_pos.len_in_bytes || 0 == z_pos.len_in_bytes || 0 == speed.len_in_bytes) return (position_soa_t){0};
    return (position_soa_t){
        .capacity = init_capacity,
        .len = 0,
        .x_pos = (float *)x_pos.ptr,
        .y_pos = (float *)y_pos.ptr,
        .z_pos = (float *)z_pos.ptr,
        .speed = (float *)speed.ptr,
    };
}


int 
position_soa_append_item_fn(arena_allocator_t *allocator, position_soa_t *dst, const position_t item)
{
    size_t expected_len = sizeof(float) * (1 + dst->len);
    slice_t x_slice = (slice_t){.len_in_bytes = sizeof(float) * dst->capacity, .ptr = dst->x_pos};
    slice_t y_slice = (slice_t){.len_in_bytes = sizeof(float) * dst->capacity, .ptr = dst->y_pos};
    slice_t z_slice = (slice_t){.len_in_bytes = sizeof(float) * dst->capacity, .ptr = dst->z_pos};
    slice_t speed_slice = (slice_t){.len_in_bytes = sizeof(float) * dst->capacity, .ptr = dst->speed};
    // slice_t speed = arena_allocator_alloc_aligned(allocator, (init_capacity * sizeof(float)), sizeof(float), DEFAULT_ALIGNMENT);
    if (0 == x_slice.len_in_bytes || 0 == y_slice.len_in_bytes || 0 == z_slice.len_in_bytes || 0 == speed_slice.len_in_bytes) return 1;
    if (dst->capacity * sizeof(float) < expected_len){
        x_slice = arena_allocator_resize_aligned(allocator, x_slice, expected_len << 1, sizeof(float), DEFAULT_ALIGNMENT);
        y_slice = arena_allocator_resize_aligned(allocator, y_slice, expected_len << 1, sizeof(float), DEFAULT_ALIGNMENT);
        z_slice = arena_allocator_resize_aligned(allocator, z_slice, expected_len << 1, sizeof(float), DEFAULT_ALIGNMENT);
        speed_slice = arena_allocator_resize_aligned(allocator, speed_slice, expected_len << 1, sizeof(float), DEFAULT_ALIGNMENT);
        if (0 == x_slice.len_in_bytes || 0 == y_slice.len_in_bytes || 0 == z_slice.len_in_bytes || 0 == speed_slice.len_in_bytes) return 1;
        dst->capacity = (1 + dst->len) << 1;
    }
    memmove((void *)&(x_slice.ptr[dst->len * sizeof(float)]), (void *)&item.x_pos, sizeof(float));
    memmove((void *)&(y_slice.ptr[dst->len * sizeof(float)]), (void *)&item.y_pos, sizeof(float));
    memmove((void *)&(z_slice.ptr[dst->len * sizeof(float)]), (void *)&item.z_pos, sizeof(float));
    memmove((void *)&(speed_slice.ptr[dst->len * sizeof(float)]), (void *)&item.speed, sizeof(float));
    dst->len += 1;
    dst->x_pos = (float *)x_slice.ptr;
    dst->y_pos = (float *)y_slice.ptr;
    dst->z_pos = (float *)z_slice.ptr;
    dst->speed = (float *)speed_slice.ptr;
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

position_t
position_soa_get_position_fn(position_soa_t *dst, size_t index)
{
    return (position_t) {
        .x_pos = dst->x_pos[index],
        .y_pos = dst->y_pos[index],
        .z_pos = dst->z_pos[index],
        .speed = dst->speed[index],
    };
}


typedef struct player_t {
    size_t pos_idx;
    size_t vel_idx;
} player_t;


player_t 
spawn_player(arena_allocator_t *allocator, position_soa_t *pos_list, position_t pos)
{
    size_t item = pos_list->len;
    int ret = position_soa_append_item_fn(allocator, pos_list, pos);
    return (player_t){ .pos_idx = item };
}

int
main(void)
{
    arena_allocator_t gpa = arena_allocator_init_page_default(c_allocator, KB(1));
    array_list_t player_list = array_list_init_capacity(&gpa, player_t, 10); // float_list: [dynamic]player_t
    position_soa_t pos_list = position_soa_init_capacity_fn(&gpa, 200);

    player_t new_player = spawn_player(
        &gpa, &pos_list, (position_t){.x_pos = 1, .y_pos = 2, .z_pos = 0, .speed = DEFAULT_SPEED});
    player_t another_player = spawn_player(
        &gpa, &pos_list, (position_t){.x_pos = 2, .y_pos = 2, .z_pos = 0, .speed = DEFAULT_SPEED});
    array_list_append_item_fn(&gpa, &player_list, (char *)&new_player);
    array_list_append_item_fn(&gpa, &player_list, (char *)&another_player);
    
    player_t *player_itr = (player_t *)player_list.ptr;
    for (int i = 0; i < player_list.len; i++){
        printf("player_list[%d]: %lu\n", i, player_itr[i].pos_idx);
    }

    arena_allocator_deinit(&gpa);
    return 0;
}