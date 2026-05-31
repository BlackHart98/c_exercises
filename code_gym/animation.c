#include "raylib.h"

#define WSA_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#define ARRAY_LIST_IMPLEMENTATION
#include "../data_structures/array_list.h"



Rectangle ryu_idle_frame[] = {
    (Rectangle){ .x = 110, .y = 120, .height = 102, .width = 70 },
    (Rectangle){ .x = 188, .y = 120, .height = 102, .width = 70 },
    (Rectangle){ .x = 270, .y = 120, .height = 102, .width = 70 },
    (Rectangle){ .x = 348, .y = 120, .height = 102, .width = 70 },
    (Rectangle){ .x = 425, .y = 120, .height = 102, .width = 70 },
    (Rectangle){ .x = 502, .y = 120, .height = 102, .width = 70 },
};

Rectangle ryu_start_frame[] = {
    (Rectangle){ .x = 12, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 87, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 165, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 240, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 315, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 393, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 462, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 532, .y = 5, .height = 102, .width = 70 },
    (Rectangle){ .x = 27, .y = 120, .height = 102, .width = 70 },
};

Rectangle ryu_move_frame[] = {
    (Rectangle){ .x = 58, .y = 238, .height = 102, .width = 70 },
    (Rectangle){ .x = 139, .y = 238, .height = 102, .width = 70 },
    (Rectangle){ .x = 218, .y = 238, .height = 102, .width = 70 },
    (Rectangle){ .x = 295, .y = 238, .height = 102, .width = 70 },
    (Rectangle){ .x = 369, .y = 238, .height = 102, .width = 70 },
    (Rectangle){ .x = 446, .y = 238, .height = 102, .width = 70 },
};

Rectangle ryu_backoff_frame[] = {
    (Rectangle){ .x = 60, .y = 345, .height = 102, .width = 70 },
    (Rectangle){ .x = 139, .y = 345, .height = 102, .width = 70 },
    (Rectangle){ .x = 218, .y = 345, .height = 102, .width = 70 },
    (Rectangle){ .x = 295, .y = 345, .height = 102, .width = 70 },
    (Rectangle){ .x = 369, .y = 345, .height = 102, .width = 70 },
    (Rectangle){ .x = 446, .y = 345, .height = 102, .width = 70 },
};

Rectangle ryu_punch_frame[] = {
    // Jab
    (Rectangle){ .x = 430, .y = 696, .height = 102, .width = 70 },
    (Rectangle){ .x = 522, .y = 696, .height = 102, .width = 110 },
    (Rectangle){ .x = 22, .y = 818, .height = 102, .width = 70 },
    
    // Left-Right hook
    (Rectangle){ .x = 103, .y = 818, .height = 102, .width = 70 },
    (Rectangle){ .x = 190, .y = 818, .height = 102, .width = 109 },
    (Rectangle){ .x = 308, .y = 818, .height = 102, .width = 70 },
    (Rectangle){ .x = 404, .y = 818, .height = 102, .width = 109 },
    (Rectangle){ .x = 518, .y = 818, .height = 102, .width = 70 },
    (Rectangle){ .x = 4, .y = 936, .height = 102, .width = 70 },
};

enum ANIMATION_GROUP {
    IDLE=0, 
    START, 
    MOVE, 
    BACKOFF, 
    LEFT_RIGHT_HOOK, 
    JAB,
};


typedef struct animation_t {
    float       frame_speed;
    float       elapsed_time;
    int         current_frame;

    // slice_t can hold this
    int         frame_count;
    Rectangle   *frame_rec; // []Rectangle
} animation_t;


typedef struct sprite_t{
    Texture2D texture;
    int current_anim_group;
    int prev_anim_group;

    animation_t idle_anim;
    animation_t start_anim;
    animation_t move_anim;
    animation_t backoff_anim;
    animation_t punch_anim;
} sprite_t;


void
update_fn(sprite_t *sprite, float delta);

void
draw_fn(sprite_t *sprite);


int
main(void)
{
    const int screen_width = 800;
    const int screen_height = 450;
    InitWindow(screen_width, screen_height, "ryu animation");
    {
        Texture2D ryu_texture = LoadTexture("code_gym/resources/Ryu.gif");
        sprite_t ryu_sprite = (sprite_t){
            .start_anim = (animation_t){
                .current_frame = 0,
                .frame_rec = ryu_start_frame,
                .frame_speed = 8.0f,
                .frame_count = 9,
                .elapsed_time = 0.0f,
            },
            .idle_anim = (animation_t){
                .current_frame = 0,
                .frame_rec = ryu_idle_frame,
                .frame_speed = 10.0f,
                .frame_count = 6,
                .elapsed_time = 0.0f,
            },
            .move_anim = (animation_t){
                .current_frame = 0,
                .frame_rec = ryu_move_frame,
                .frame_speed = 15.0f,
                .frame_count = 6,
                .elapsed_time = 0.0f,
            },
            .backoff_anim = (animation_t){
                .current_frame = 0,
                .frame_rec = ryu_backoff_frame,
                .frame_speed = 15.0f,
                .frame_count = 6,
                .elapsed_time = 0.0f,
            },
            .punch_anim = (animation_t){
                .current_frame = 0,
                .frame_rec = ryu_punch_frame,
                .frame_speed = 15.0f,
                .frame_count = 9,
                .elapsed_time = 0.0f,
            },
            
            .texture = ryu_texture,
            .current_anim_group = START,
            .prev_anim_group = START,
        };

        SetTargetFPS(60); 
        while (!WindowShouldClose()) {
            float delta = GetFrameTime();
            // Update
            update_fn(&ryu_sprite, delta);
            // Render
            draw_fn(&ryu_sprite);       
        }
        UnloadTexture(ryu_texture);

    }
    CloseWindow();
    return 0;
}


void
update_fn(sprite_t *sprite, float delta)
{
    if (IsKeyDown(KEY_RIGHT)) {
        sprite->prev_anim_group = sprite->current_anim_group;
        sprite->current_anim_group = MOVE;
    }
    if (IsKeyDown(KEY_LEFT)) {
        sprite->prev_anim_group = sprite->current_anim_group;
        sprite->current_anim_group = BACKOFF;
    }
    if (IsKeyPressed(KEY_P)) {
        if (JAB == sprite->prev_anim_group){
            sprite->prev_anim_group = sprite->current_anim_group;
            sprite->current_anim_group = LEFT_RIGHT_HOOK;
        } else {
            sprite->prev_anim_group = sprite->current_anim_group;
            sprite->current_anim_group = JAB;
        }
    }
    switch (sprite->current_anim_group){
        case IDLE: {
            sprite->idle_anim.elapsed_time += delta;
            float fps = 1 / sprite->idle_anim.frame_speed;
            if (sprite->idle_anim.elapsed_time >= fps) {
                sprite->idle_anim.elapsed_time = 0.0f;
                sprite->idle_anim.current_frame++;
                if (sprite->idle_anim.current_frame >= sprite->idle_anim.frame_count) sprite->idle_anim.current_frame = 0;
            }
            break;
        }
        case START: {
            sprite->start_anim.elapsed_time += delta;
            float fps = 1 / sprite->start_anim.frame_speed;
            if (sprite->start_anim.elapsed_time >= fps) {
                sprite->start_anim.elapsed_time = 0.0f;
                sprite->start_anim.current_frame++;
                if (sprite->start_anim.current_frame >= sprite->start_anim.frame_count) {
                    sprite->current_anim_group = IDLE;
                }
            }
            break;
        }
        case MOVE: {
            sprite->move_anim.elapsed_time += delta;
            float fps = 1 / sprite->move_anim.frame_speed;
            if (sprite->move_anim.elapsed_time >= fps) {
                sprite->move_anim.elapsed_time = 0.0f;
                sprite->move_anim.current_frame++;
                if (sprite->move_anim.current_frame >= sprite->move_anim.frame_count) {
                    sprite->move_anim.current_frame = 0;
                    sprite->current_anim_group = IDLE;
                }
            }
            break;
        }
        case BACKOFF:{
            sprite->backoff_anim.elapsed_time += delta;
            float fps = 1 / sprite->backoff_anim.frame_speed;
            if (sprite->backoff_anim.elapsed_time >= fps) {
                sprite->backoff_anim.elapsed_time = 0.0f;
                sprite->backoff_anim.current_frame++;
                if (sprite->backoff_anim.current_frame >= sprite->backoff_anim.frame_count) {
                    sprite->backoff_anim.current_frame = 0;
                    sprite->prev_anim_group = sprite->current_anim_group;
                    sprite->current_anim_group = IDLE;
                }
            }
            break;
        }
        case JAB:{
            sprite->punch_anim.elapsed_time += delta;
            float fps = 1 / sprite->punch_anim.frame_speed;
            if (sprite->punch_anim.elapsed_time >= fps) {
                sprite->punch_anim.elapsed_time = 0.0f;
                sprite->punch_anim.current_frame++;
                if (sprite->punch_anim.current_frame >= 3) {
                    sprite->punch_anim.current_frame = 3;
                    sprite->prev_anim_group = sprite->current_anim_group;
                    sprite->current_anim_group = IDLE;
                }
            }
            break;
        }
        case LEFT_RIGHT_HOOK:{
            sprite->punch_anim.elapsed_time += delta;
            float fps = 1 / sprite->punch_anim.frame_speed;
            if (sprite->punch_anim.elapsed_time >= fps) {
                sprite->punch_anim.elapsed_time = 0.0f;
                sprite->punch_anim.current_frame++;
                if (sprite->punch_anim.current_frame >= sprite->punch_anim.frame_count) {
                    sprite->punch_anim.current_frame = 0;
                    sprite->prev_anim_group = sprite->current_anim_group;
                    sprite->current_anim_group = IDLE;
                }
            }
            break;
        }
    }
}


void
draw_fn(sprite_t *sprite)
{
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        switch (sprite->current_anim_group){
            case IDLE: {
                DrawTextureRec(
                    sprite->texture, 
                    sprite->idle_anim.frame_rec[sprite->idle_anim.current_frame], 
                    (Vector2){50, 50}, WHITE);
                break;
            }
            case START: {
                DrawTextureRec(
                    sprite->texture, 
                    sprite->start_anim.frame_rec[sprite->start_anim.current_frame], 
                    (Vector2){50, 50}, WHITE);
                break;
            }
            case MOVE: {
                DrawTextureRec(
                    sprite->texture, 
                    sprite->move_anim.frame_rec[sprite->move_anim.current_frame], 
                    (Vector2){50, 50}, WHITE);
                break;
            }
            case BACKOFF: {
                DrawTextureRec(
                    sprite->texture, 
                    sprite->backoff_anim.frame_rec[sprite->backoff_anim.current_frame], 
                    (Vector2){50, 50}, WHITE);
                break;
            }
            case JAB: {
                DrawTextureRec(
                    sprite->texture, 
                    sprite->punch_anim.frame_rec[sprite->punch_anim.current_frame], 
                    (Vector2){50, 50}, WHITE);
                break;
            }
            case LEFT_RIGHT_HOOK: {
                DrawTextureRec(
                    sprite->texture, 
                    sprite->punch_anim.frame_rec[sprite->punch_anim.current_frame], 
                    (Vector2){50, 50}, WHITE);
                break;
            }
        }
    }
    EndDrawing(); 
}