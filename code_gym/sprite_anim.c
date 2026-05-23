#include "raylib.h"
#include <stdlib.h>

#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1


typedef struct player_t {
    Vector2 position;
    Rectangle frame_rec;
    Texture2D *texture;
    int current_frame;
    int frames_counter;
    int frames_speed;
} player_t;


typedef struct objects_t {
    player_t player;
} objects_t;


typedef struct game_state_t {
    objects_t *objects;
} game_state_t;


// Objects
objects_t
objects_init(
    const int screen_height 
    , const int screen_width 
    , Texture2D *tex_player
);


void 
update_game_fn(game_state_t *state, const int screen_height,  const int screen_width);

void 
draw_game_fn(game_state_t *state, const int screen_height, const int screen_width);

int
main(void)
{
    const int screen_width = 800;
    const int screen_height = 450;

    InitWindow(screen_width, screen_height, "raylib [textures] example - sprite animation");
    {
        Texture2D scarfy = LoadTexture("code_gym/resources/scarfy.png");
        objects_t game_objects = objects_init(screen_height, screen_width, &scarfy);

        game_state_t state = (game_state_t){ .objects = &game_objects };

        SetTargetFPS(60); 
        while (!WindowShouldClose()) {
            update_game_fn(&state, screen_height, screen_width);
            draw_game_fn(&state, screen_height, screen_width);
        }
        UnloadTexture(scarfy);
    }
    CloseWindow();
    return 0;
}


objects_t
objects_init(
    const int screen_height 
    , const int screen_width 
    , Texture2D *tex_player
)
{
    player_t player = (player_t){ 
        .position = (Vector2){ 350.0f, 280.0f },
        .frame_rec = { 0.0f, 0.0f, (float)tex_player->width/6, (float)tex_player->height },
        .texture = tex_player,
        .frames_counter = 0,
        .frames_speed = 8,
        .current_frame = 0,
    };
    return (objects_t){
        .player = player,
    };
}


void 
update_game_fn(game_state_t *state, const int screen_height,  const int screen_width)
{
    state->objects->player.frames_counter++;
    if (state->objects->player.frames_counter >= (60/state->objects->player.frames_speed)){
        state->objects->player.frames_counter = 0;
        state->objects->player.current_frame++;

        if (state->objects->player.current_frame > 5) state->objects->player.current_frame = 0;

        state->objects->player.frame_rec.x = 
            (float)state->objects->player.current_frame * (float)state->objects->player.texture->width/6;
    }
    // Control frames speed
    if (IsKeyPressed(KEY_RIGHT)) state->objects->player.frames_speed++;
    else if (IsKeyPressed(KEY_LEFT)) state->objects->player.frames_speed--;

    if (state->objects->player.frames_speed > MAX_FRAME_SPEED) state->objects->player.frames_speed = MAX_FRAME_SPEED;
    else if (state->objects->player.frames_speed < MIN_FRAME_SPEED) state->objects->player.frames_speed = MIN_FRAME_SPEED;
}

void 
draw_game_fn(game_state_t *state, const int screen_height, const int screen_width)
{
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawTexture(*(state->objects->player.texture), 15, 40, WHITE);
        DrawRectangleLines(15, 40, state->objects->player.texture->width, state->objects->player.texture->height, LIME);
        DrawRectangleLines(
            15 + (int)state->objects->player.frame_rec.x
            , 40 + (int)state->objects->player.frame_rec.y
            , (int)state->objects->player.frame_rec.width
            , (int)state->objects->player.frame_rec.height, RED);
        DrawText("FRAME SPEED: ", 165, 210, 10, DARKGRAY);
        DrawText(TextFormat("%02i FPS", state->objects->player.frames_speed), 575, 210, 10, DARKGRAY);
        DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, DARKGRAY);
        for (int i = 0; i < MAX_FRAME_SPEED; i++){
            if (i < state->objects->player.frames_speed) DrawRectangle(250 + 21*i, 205, 20, 20, RED);
            DrawRectangleLines(250 + 21*i, 205, 20, 20, MAROON);
        }
        DrawTextureRec(*(state->objects->player.texture), state->objects->player.frame_rec, state->objects->player.position, WHITE);

        DrawText("(c) Scarfy sprite by Eiden Marsal", screen_width - 200, screen_height - 20, 10, GRAY);
    }
    EndDrawing();
}