#include "raylib.h"




typedef enum {LOGO, TITLE, GAMEPLAY, ENDING } game_screen_t;



int
main(void)
{
    const int screen_height = 600;
    const int screen_width = 800;
    InitWindow(screen_width, screen_height, "PROJECT: BLOCKS GAME");

    SetTargetFPS(60);
 
    game_screen_t screen = LOGO;

    int frames_counter = 0;
    int game_result = -1;
    int game_paused = 0;

    // Main loop
    while (!WindowShouldClose()) {
        switch (screen) {
            case LOGO: {
                frames_counter++;
                if (frames_counter > 180) {
                    screen = TITLE;    // Change to TITLE screen after 3 seconds
                    frames_counter = 0;
                }
                break;
            }
            case TITLE: {
                frames_counter++;
                if (IsKeyPressed(KEY_ENTER)) screen = GAMEPLAY;
                break;
            }
            case GAMEPLAY: {
                if (!game_paused) {

                }
                if (IsKeyPressed(KEY_ENTER)) screen = ENDING;
                break;
            }
            case ENDING: {
                frames_counter++;
                if (IsKeyPressed(KEY_ENTER)) screen = TITLE;
                break;
            }
        }
        // Render
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            switch (screen) {
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