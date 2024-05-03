/* Copyright (c) 2024 Lou Woell */

#include <math.h>
#include <raylib.h>
#include <sys/param.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Color Scheme of my Website
#define DAY_COLOR   CLITERAL(Color) { 238, 114, 241, 255}
#define NIGHT_COLOR CLITERAL(Color) { 33, 32, 44, 255}

// Alternative Neutral Colors
/* #define DAY_COLOR SKYBLUE */
/* #define NIGHT_COLOR DARKBLUE */

#define FONTSIZE 30
#define PAUSE_TEXT "PAUSED"

#define RECS  30
#define REC_SIZE 20

typedef enum {
     DAY,
     NIGHT,
} status;

typedef struct {
     Vector2 Position;

     Vector2 Direction;
     float   speed;

     status  team;
     float   radius;
} bBall;

typedef struct {
     // Pause related
     int   framesCounter;
     bool  pause;

     int BoardDim;
     RenderTexture2D  target;

     // Board
     status board[RECS][RECS];

     // Bouncing Balls
     bBall DayBall;
     bBall NightBall;
} Game;

Game game = {0};
int pause_text_width = 0;

void MakeBoard()
{
     for (int y = 0; y < RECS; y++)
     {
          for (int x = 0; x < RECS; x++)
          {
               if (RECS / 2 > x)
                    game.board[y][x] = DAY;
               else
                    game.board[y][x] = NIGHT;
          }
     }
}

void DrawBoard()
{
     ClearBackground(NIGHT_COLOR);
     for (int j = 0; j < RECS; j++)
     {
          for (int i = 0; i < RECS; i++)
          {
               if (game.board[i][j]){
                    DrawRectangle(REC_SIZE * j,
                                  REC_SIZE * i,
                                  REC_SIZE,
                                  REC_SIZE,
                                  DAY_COLOR);
               }
          }
     }
}

int RandomOffset(int offset)
{
     return GetRandomValue(-(offset), (offset));
}


void MakeBouncingBall(bBall *ball, status team, float startx, float starty)
{
     float xspeed;
     float yspeed;

     if (startx > game.BoardDim / 2.0f)
     {
          xspeed = -1;
          yspeed = 1;
     }
     else
     {
          xspeed =  1;
          yspeed = -1;
     }

     ball->Position = (Vector2){ startx, starty};
     ball->Direction = (Vector2){ xspeed, yspeed };
     ball->team = team;
     ball->radius = REC_SIZE* 0.5;
     ball->speed = REC_SIZE * 0.5 - 4;
}

void BouncingBallPosition(bBall *ball)
{
     ball->Position.x += ball->Direction.x * ball->speed;
     ball->Position.y += ball->Direction.y * ball->speed;

     // Check walls collision
     if ((ball->Position.x > (game.BoardDim - ball->radius)) ||
         (ball->Position.x < ball->radius))
     {
          ball->Direction.x *= -1;
          ball->Position.x += ball->Direction.x*ball->speed;
     }
     if ((ball->Position.y > (game.BoardDim - ball->radius)) ||
         (ball->Position.y < ball->radius))
     {
          ball->Direction.y *= -1;
          ball->Position.y += ball->Direction.y*ball->speed;
     }

     // Check Colour collision
     for (double angle = 0; angle < (2 * PI); angle += (PI / 4))
     {
          int i = floor((ball->Position.x + cos(angle) * ball->radius) /
                        REC_SIZE);
          int j = floor((ball->Position.y + sin(angle) * ball->radius) /
                        REC_SIZE);

          if (i >= 0 && i < RECS && j >= 0 && j < RECS)
          {
               if (game.board[j][i] == ball->team)
               {
                    game.board[j][i] = !game.board[j][i];
                    // Determine bounce direction based on the angle
                    if (fabs(cos(angle)) > fabs(sin(angle)))
                         ball->Direction.x *= -1;
                    else
                         ball->Direction.y *= -1;
               }
          }
     }
}

void DrawBouncingBall(bBall *ball)
{
     /* Draw Bouncing Ball */
     DrawCircleV(ball->Position, ball->radius, (ball->team ? DAY_COLOR : NIGHT_COLOR));
}

void SetGame()
{
     MakeBoard();
     MakeBouncingBall(
          &game.DayBall,
          DAY,
          (game.BoardDim / 4.0f) * 3 + RandomOffset(game.BoardDim / 4),
          game.BoardDim/2.0f + RandomOffset(game.BoardDim/2));
     MakeBouncingBall(
          &game.NightBall,
          NIGHT,
          (game.BoardDim / 4.0f) + RandomOffset(game.BoardDim/ 4),
          game.BoardDim/2.0f + RandomOffset(game.BoardDim/2));
}

void DrawGame()
{
     float scale = MIN((float)GetScreenWidth()/game.BoardDim,
                       (float)GetScreenHeight()/game.BoardDim);

     BeginTextureMode(game.target);

     DrawBoard();
     DrawBouncingBall(&game.DayBall);
     DrawBouncingBall(&game.NightBall);

     // On pause, we draw a blinking message
     if (game.pause && ((game.framesCounter/30)%2))
     {
          DrawText(PAUSE_TEXT,
                   (game.BoardDim - pause_text_width)/2,
                   game.BoardDim/3,
                   FONTSIZE,
                   WHITE);
     }
     EndTextureMode();

     BeginDrawing();
     ClearBackground(BLACK);
     DrawTexturePro(
          game.target.texture,
          (Rectangle){
               0.0f,
               0.0f,
               (float)game.target.texture.width,
               (float)-game.target.texture.height
          },
          (Rectangle){
               (GetScreenWidth() - ((float)game.BoardDim*scale))*0.5f,
               (GetScreenHeight() - ((float)game.BoardDim*scale))*0.5f,
               (float)game.BoardDim*scale,
               (float)game.BoardDim*scale
          },
          (Vector2){0, 0},
          0.0f,
          WHITE);
     EndDrawing();
}


void handleInput()
{
     // Press P to Pause
     if (IsKeyPressed(KEY_P)) game.pause = !game.pause;
     // Press R to reset
     if (IsKeyPressed(KEY_R)) SetGame();
}

void UpdateDrawFrame()
{
     handleInput();
     /* ScaleGame(); */
     if (!game.pause)
     {
          BouncingBallPosition(&game.DayBall);
          BouncingBallPosition(&game.NightBall);
     }
     else game.framesCounter++;

     DrawGame();
}

int main()
{
     game.BoardDim = RECS*REC_SIZE;

     SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
     InitWindow(game.BoardDim,game.BoardDim, "Pong Wars");

     game.target = LoadRenderTexture(game.BoardDim, game.BoardDim);
     SetTextureFilter(game.target.texture, TEXTURE_FILTER_POINT);

     pause_text_width = MeasureText(PAUSE_TEXT, FONTSIZE);

     SetGame();

#if defined(PLATFORM_WEB)
     emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
     SetTargetFPS(60);
     while (!WindowShouldClose())
     {
          UpdateDrawFrame();
     }
#endif

     CloseWindow();
     return 0;
}
