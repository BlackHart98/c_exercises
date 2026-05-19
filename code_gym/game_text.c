#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define PLAYER_LIFES             5
#define BRICKS_LINES             5
#define BRICKS_PER_LINE         20

#define BRICKS_POSITION_Y       50

#define HAS_TEXTURES

typedef enum {LOGO, TITLE, GAMEPLAY, ENDING } game_screen_t;


// Player structure
typedef struct player_t {
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    Rectangle bounds;
    int lifes;
#if defined(HAS_TEXTURES)
    Texture2D *texture;
#endif
} player_t;

// Ball structure
typedef struct ball_t {
    Vector2 position;
    Vector2 speed;
    float radius;
    int active;
#if defined(HAS_TEXTURES)
    Texture2D *texture;
#endif
} ball_t;

// Bricks structure
typedef struct brick_t {
    Vector2 position;
    Vector2 size;
    Rectangle bounds;
    int resistance;
    int active;
#if defined(HAS_TEXTURES)
    Texture2D *texture;
#endif
} brick_t;


typedef struct objects_t {
    player_t player;
    ball_t ball;
    brick_t *bricks;
} objects_t;

typedef struct game_state_t {
    game_screen_t screen;
    int frames_counter;
    int game_paused;
    int game_result;
    objects_t *objects;
} game_state_t;


void 
update_game_fn(game_state_t *state, const int screen_height,  const int screen_width);


void 
draw_game_fn(
    game_state_t *state
    , const int screen_height
    , const int screen_width
#if defined(HAS_TEXTURES)
    , Texture2D *tex_logo
    , Font *font
#endif
);

// Objects
objects_t
objects_init(
    const int screen_height 
    , const int screen_width 
    , char *buf
#if defined(HAS_TEXTURES)
    , Texture2D *tex_ball
    , Texture2D *tex_paddle
    , Texture2D *tex_brick
#endif
);


int
main(void)
{
    const int screen_height = 600;
    const int screen_width = 800;

    InitWindow(screen_width, screen_height, "PROJECT: BLOCKS GAME");
    {
        Texture2D tex_logo = LoadTexture("code_gym/resources/raylib_logo.png");
        Texture2D tex_ball = LoadTexture("code_gym/resources/ball.png");
        Texture2D tex_paddle = LoadTexture("code_gym/resources/paddle.png");
        Texture2D tex_brick = LoadTexture("code_gym/resources/brick.png");

        Font font = LoadFont("resources/setback.png");

        SetTargetFPS(60);
        brick_t buf[BRICKS_LINES][BRICKS_PER_LINE] = {0};
        objects_t objects = objects_init(
            screen_height
            , screen_width
            , (char *)buf
#if defined(HAS_TEXTURES)
            , &tex_ball
            , &tex_paddle
            , &tex_brick
#endif
        );
        game_state_t state = (game_state_t){
            .screen = LOGO, 
            .frames_counter = 0, 
            .game_paused = 0,
            .objects = &objects,};

        // Main loop
        while (!WindowShouldClose()) {
            update_game_fn(&state, screen_height, screen_width);
            draw_game_fn(
                &state
                , screen_height
                , screen_width
#if defined(HAS_TEXTURES)
                , &tex_logo
                , &font
#endif
            );
        }
        UnloadTexture(tex_logo);
        UnloadTexture(tex_ball);
        UnloadTexture(tex_paddle);
        UnloadTexture(tex_brick);
    }
    CloseWindow();  
    return 0;
}


objects_t
objects_init(
    const int screen_height
    , const int screen_width
    , char *buf
#if defined(HAS_TEXTURES)
    , Texture2D *tex_ball
    , Texture2D *tex_paddle
    , Texture2D *tex_brick
#endif
)
{
    brick_t bricks[BRICKS_LINES][BRICKS_PER_LINE] = {0};
    for (int j = 0; j < BRICKS_LINES; j++)
    {
        for (int i = 0; i < BRICKS_PER_LINE; i++)
        {
            bricks[j][i].size = (Vector2){ screen_width/BRICKS_PER_LINE, 20 };
            bricks[j][i].position = (Vector2){ i*bricks[j][i].size.x, j*bricks[j][i].size.y + BRICKS_POSITION_Y };
            bricks[j][i].bounds = (Rectangle){ bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y };
            bricks[j][i].active = 1;
#if defined(HAS_TEXTURES)
            bricks[j][i].texture = tex_brick;
#endif
        }
    }
    player_t player = (player_t){ 
        .position = (Vector2){ screen_width/2, screen_height*7/8 },
        .speed = (Vector2){ 8.0f, 0.0f },
        .size = (Vector2){ 100, 24 },
        .lifes = PLAYER_LIFES,
#if defined(HAS_TEXTURES)
        .texture = tex_paddle
#endif
    };

    ball_t ball = (ball_t){0};
    ball.radius = 10.0f;
    ball.active = false;
    ball.position = (Vector2){ player.position.x + player.size.x/2, player.position.y - ball.radius*2 };
    ball.speed = (Vector2){ 4.0f, 4.0f };
#if defined(HAS_TEXTURES)
    ball.texture = tex_ball;
#endif
    memcpy(buf, bricks, sizeof(brick_t) * BRICKS_LINES * BRICKS_PER_LINE);
    return (objects_t){
        .player = player,
        .ball = ball,
        .bricks = (struct brick_t *)buf,
    };
}


void 
update_game_fn(game_state_t *state, const int screen_height,  const int screen_width)
{
    brick_t bricks[BRICKS_LINES][BRICKS_PER_LINE] = {0};
    memcpy(bricks, state->objects->bricks, sizeof(brick_t) * BRICKS_LINES * BRICKS_PER_LINE);
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
            if (IsKeyPressed('P')) state->game_paused = !state->game_paused;
            if (!state->game_paused) {
                if (IsKeyDown(KEY_LEFT)) state->objects->player.position.x -= state->objects->player.speed.x;
                if (IsKeyDown(KEY_RIGHT)) state->objects->player.position.x += state->objects->player.speed.x;

                // Lock to window frame
                if ((state->objects->player.position.x) <= 0) state->objects->player.position.x = 0;
                if ((state->objects->player.position.x + state->objects->player.size.x) >= screen_width) 
                    state->objects->player.position.x = screen_width - state->objects->player.size.x;

                state->objects->player.bounds = (Rectangle){ 
                    state->objects->player.position.x, state->objects->player.position.y, 
                    state->objects->player.size.x, state->objects->player.size.y };
                
                if (state->objects->ball.active){
                    state->objects->ball.position.x += state->objects->ball.speed.x;
                    state->objects->ball.position.y += state->objects->ball.speed.y;

                    if (((state->objects->ball.position.x + state->objects->ball.radius) >= screen_width) 
                        || ((state->objects->ball.position.x - state->objects->ball.radius) <= 0)) state->objects->ball.speed.x *= -1;
                    
                    if ((state->objects->ball.position.y - state->objects->ball.radius) <= 0) state->objects->ball.speed.y *= -1;

                    if (CheckCollisionCircleRec(state->objects->ball.position, state->objects->ball.radius, state->objects->player.bounds)){
                        state->objects->ball.speed.y *= -1;
                        state->objects->ball.speed.x = 
                            (state->objects->ball.position.x - (state->objects->player.position.x + state->objects->player.size.x/2))/state->objects->player.size.x*5.0f;
                    }

                    for (int i = 0; i < BRICKS_LINES; i++) {
                        for (int j = 0; j < BRICKS_PER_LINE; j++) {
                            if (bricks[i][j].active 
                                && CheckCollisionCircleRec(state->objects->ball.position, state->objects->ball.radius, bricks[i][j].bounds)){
                                bricks[i][j].active = false;
                                state->objects->ball.speed.y *= -1;
                                break;
                            }
                        }
                    }

                    if ((state->objects->ball.position.y + state->objects->ball.radius) >= screen_height){
                        state->objects->ball.position.x = state->objects->player.position.x + state->objects->player.size.x/2;
                        state->objects->ball.position.y = state->objects->player.position.y - state->objects->ball.radius - 1.0f;
                        state->objects->ball.speed = (Vector2){ 0, 0 };
                        state->objects->ball.active = false;

                        state->objects->player.lifes--;
                    }
                        
                    if (state->objects->player.lifes < 0){
                        state->screen = ENDING;
                        state->objects->player.lifes = 5;
                        state->frames_counter = 0;
                    }
                } else {
                    state->objects->ball.position.x = state->objects->player.position.x + state->objects->player.size.x/2;
                    if (IsKeyPressed(KEY_SPACE)){
                        // Activate ball logic
                        state->objects->ball.active = true;
                        state->objects->ball.speed = (Vector2){ 0, -5.0f };
                    }
                }
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
    memcpy(state->objects->bricks, bricks, sizeof(brick_t) * BRICKS_LINES * BRICKS_PER_LINE);
}


void 
draw_game_fn(
    game_state_t *state
    , const int screen_height
    , const int screen_width
#if defined(HAS_TEXTURES)
    , Texture2D *tex_logo
    , Font *font
#endif
)
{
    brick_t bricks[BRICKS_LINES][BRICKS_PER_LINE] = {0};
    memcpy(bricks, state->objects->bricks, sizeof(brick_t) * BRICKS_LINES * BRICKS_PER_LINE);
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        switch (state->screen) {
            case LOGO: {
                DrawTexture(*tex_logo, screen_width/2 - tex_logo->width/2, screen_height/2 - tex_logo->height/2, WHITE);
                break;
            }
            case TITLE: {
                DrawTextEx(*font, "BLOCKS", (Vector2){ 100, 80 }, 160, 10, MAROON);
                if ((state->frames_counter/30)%2 == 0) 
                    DrawText("PRESS [ENTER] to START", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] to START", 20)/2, GetScreenHeight()/2 + 60, 20, DARKGRAY);
                break;
            }
            case GAMEPLAY: {
#if defined(SHAPES)
                DrawRectangle(
                    state->objects->player.position.x, 
                    state->objects->player.position.y, 
                    state->objects->player.size.x, 
                    state->objects->player.size.y, BLACK); 
                DrawCircleV(state->objects->ball.position, state->objects->ball.radius, MAROON);
                for (int j = 0; j < BRICKS_LINES; j++){
                    for (int i = 0; i < BRICKS_PER_LINE; i++){
                        if (bricks[j][i].active){
                            if ((i + j)%2 == 0) DrawRectangle(bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y, GRAY);
                            else DrawRectangle(bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y, DARKGRAY);
                        }
                    }
                }
#elif defined(HAS_TEXTURES)
                DrawTextureEx(*(state->objects->player.texture), state->objects->player.position, 0.0f, 1.0f, WHITE);
                DrawTexture(
                    *(state->objects->ball.texture), 
                    state->objects->ball.position.x - state->objects->ball.radius/2, 
                    state->objects->ball.position.y - state->objects->ball.radius/2, MAROON);
                for (int j = 0; j < BRICKS_LINES; j++){
                    for (int i = 0; i < BRICKS_PER_LINE; i++){
                        if (bricks[j][i].active){
                            if ((i + j)%2 == 0) DrawTextureEx(*(bricks[j][i].texture), bricks[j][i].position, 0.0f, 1.0f, GRAY);
                            else DrawTextureEx(*(bricks[j][i].texture), bricks[j][i].position, 0.0f, 1.0f, DARKGRAY);
                        }
                    }
                }
#endif
                for (int i = 0; i < state->objects->player.lifes; i++) DrawRectangle(20 + 40*i, screen_height - 30, 35, 10, LIGHTGRAY);
                if (state->game_paused) DrawText("GAME PAUSED", screen_width/2 - MeasureText("GAME PAUSED", 40)/2, screen_height/2 + 60, 40, GRAY);
                break;
            }
            case ENDING: {
                DrawTextEx(*font, "GAME FINISHED", (Vector2){ 80, 100 }, 80, 6, MAROON);
                if ((state->frames_counter/30)%2 == 0) 
                    DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 + 60, 20, DARKGRAY);
                break;
            }
        }
    }
    EndDrawing();

}