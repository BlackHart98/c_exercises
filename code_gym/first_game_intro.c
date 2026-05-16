#include "raylib.h"




typedef enum {LOGO, TITLE, GAMEPLAY, ENDING } game_screen_t;
typedef struct game_state_t {
    game_screen_t screen;
    int frames_counter;
    int game_paused;
} game_state_t;


void update_game_fn(game_state_t *state)
{
    switch (state->screen) {
        case LOGO: {
            state->frames_counter++;
            if (state->frames_counter > 180) {
                state->screen = TITLE;    // Change to TITLE screen after 3 seconds
                state->frames_counter = 0;
            }
            break;
        }
        case TITLE: {
            state->frames_counter++;
            if (IsKeyPressed(KEY_ENTER)) state->screen = GAMEPLAY;
            break;
        }
        case GAMEPLAY: {
            if (!state->game_paused) {

            }
            if (IsKeyPressed(KEY_ENTER)) state->screen = ENDING;
            break;
        }
        case ENDING: {
            state->frames_counter++;
            if (IsKeyPressed(KEY_ENTER)) state->screen = TITLE;
            break;
        }
    }
} 

int
main(void)
{
    const int screen_height = 600;
    const int screen_width = 800;
    InitWindow(screen_width, screen_height, "PROJECT: BLOCKS GAME");

    SetTargetFPS(60);
    int game_result = -1;

    game_state_t state = (game_state_t){
        .screen = LOGO, 
        .frames_counter = 0, 
        .game_paused = 0};

    // Main loop
    while (!WindowShouldClose()) {
        update_game_fn(&state);
        // Render
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            switch (state.screen) {
                case LOGO: {
                    DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
                    DrawText("WAIT for 3 SECONDS...", 290, 220, 20, GRAY);
                    break;
                }
                case TITLE: {
                    DrawRectangle(0, 0, screen_width, screen_height, GREEN);
                    DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
                    DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
                    break;
                }
                case GAMEPLAY: {
                    DrawRectangle(0, 0, screen_width, screen_height, PURPLE);
                    DrawText("GAMEPLAY SCREEN", 20, 20, 40, MAROON);
                    DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);
                    break;
                }
                case ENDING: {
                    DrawRectangle(0, 0, screen_width, screen_height, BLUE);
                    DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
                    break;
                }
            }
        }
        EndDrawing();
    }
    CloseWindow();  
    return 0;
}