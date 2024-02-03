/* Copyright (c) 2024 Lou Woell */

#include <math.h>
#include <raylib.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
#endif

#include <sys/param.h>
// Color Scheme of my Website
#define DAY_COLOR   CLITERAL(Color) { 238, 114, 241, 255}
#define NIGHT_COLOR CLITERAL(Color) { 33, 32, 44, 255}

// Alternative Neutral Colors
/* #define DAY_COLOR SKYBLUE */
/* #define NIGHT_COLOR DARKBLUE */

#define FONTSIZE 30
#define PAUSE_TEXT "PAUSED"

#define MAX_RECS  24

int WIDTH = 600;
int HEIGHT = 600;

typedef struct {
  Vector2 Position;
  Vector2 Speed;
  float   Radius;
  Color   Color;
} bBall;

typedef struct {
  // Pause related
  int   framesCounter;
  bool  pause;

  // Board
  int   squareSize;
  Color colors[MAX_RECS][MAX_RECS];

  // Bouncing Balls
  bBall DayBall;
  bBall NightBall;
} Game;

Game game = {0};
int pause_text_width;

void MakeBoard()
{
  if(!game.squareSize) game.squareSize = WIDTH/MAX_RECS;

  for (int y = 0; y < MAX_RECS; y++)
    {
      for (int x = 0; x < MAX_RECS; x++)
        {
          if (MAX_RECS / 2 > x)
	    game.colors[y][x] = DAY_COLOR;
          else
	    game.colors[y][x] = NIGHT_COLOR;
        }
    }
}

int boardWidth()
{
  return game.squareSize * MAX_RECS;
}

void DrawBoard()
{
     for (int j = 0; j < MAX_RECS; j++)
     {
	  for (int i = 0; i < MAX_RECS; i++)
	  {
          DrawRectangle(game.squareSize * j,
                        game.squareSize * i,
                        game.squareSize,
                        game.squareSize,
                        game.colors[i][j]);
	  }
     }
}

int RandomOffset(int offset)
{
  return GetRandomValue(-(offset), (offset));
}

bool coloreq(Color first, Color second)
{
  if (first.r == second.r &&
      first.g == second.g &&
      first.b == second.b &&
      first.a == second.a) return true;
  return false;
}

void flipColor(Color *tile)
{
  if (coloreq(*tile, DAY_COLOR))
    *tile = NIGHT_COLOR;
  else
    *tile = DAY_COLOR;
}

void MakeBouncingBall(bBall *ball, Color color, float startx, float starty)
{
  float xspeed = game.squareSize * 0.5 - 1;
  float yspeed = game.squareSize * 0.5 - 1;

  if (startx > WIDTH / 2.0f)
    xspeed *= -1;
  else
    yspeed *= -1;

  ball->Position = (Vector2){ startx, starty};
  ball->Speed = (Vector2){ xspeed, yspeed };
  ball->Radius = game.squareSize*0.5;
  ball->Color = color;
}

void BouncingBallPosition(bBall *ball)
{
  ball->Position.x += ball->Speed.x;
  ball->Position.y += ball->Speed.y;

  // Check walls collision
  if ((ball->Position.x >= (boardWidth() - ball->Radius)) ||
      (ball->Position.x <= ball->Radius))
  {
    ball->Speed.x *= -1.0f;
    ball->Position.x += ball->Speed.x/ball->Radius;
  }
  if ((ball->Position.y >= (boardWidth() - ball->Radius)) ||
      (ball->Position.y <= ball->Radius))
  {
    ball->Speed.y *= -1.0f;
    ball->Position.y += ball->Speed.y/ball->Radius;
  }

  // Check Colour collision
  for (double angle = 0; angle < (2 * PI); angle += (PI / 4))
    {

      int i = floor((ball->Position.x + cos(angle) * ball->Radius) /
                    game.squareSize);
      int j = floor((ball->Position.y + sin(angle) * ball->Radius) /
                    game.squareSize);

      if (i >= 0 && i < MAX_RECS && j >= 0 && j < MAX_RECS)
        {
          if (coloreq(game.colors[j][i], ball->Color))
            {
              flipColor(&(game.colors[j][i]));
              // Determine bounce direction based on the angle
              if (fabs(cos(angle)) > fabs(sin(angle)))
                ball->Speed.x *= -1;
              else
                ball->Speed.y *= -1;
            }
        }
    }
}

void DrawBouncingBall(bBall *ball)
{
  /* Draw Bouncing Ball */
  DrawCircleV(ball->Position, ball->Radius, ball->Color);
}

void SetGame()
{
  MakeBoard();
  MakeBouncingBall(&game.DayBall,
                   DAY_COLOR,
                   (WIDTH / 4.0f) * 3 +
                   RandomOffset(WIDTH / 4),
                   boardWidth()/2.0f + RandomOffset(boardWidth()/2));
  MakeBouncingBall(&game.NightBall,
                   NIGHT_COLOR,
                   (boardWidth() / 4.0f) +
                   RandomOffset(WIDTH / 4),
                   boardWidth()/2.0f + RandomOffset(boardWidth()/2));
}

void DrawGame()
{
  BeginDrawing();

  ClearBackground(RAYWHITE);

  DrawBoard();
  DrawBouncingBall(&game.DayBall);
  DrawBouncingBall(&game.NightBall);

  // On pause, we draw a blinking message
  if (game.pause && ((game.framesCounter/30)%2))
    {
      DrawText(PAUSE_TEXT,
               (boardWidth() - pause_text_width)/2,
               boardWidth()/3,
               FONTSIZE,
               WHITE);
    }

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

#if defined(PLATFORM_WEB)
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
#else
  SetConfigFlags(FLAG_VSYNC_HINT);
#endif

  InitWindow(WIDTH,HEIGHT, "Pong Wars");

#if defined(PLATFORM_WEB)
  emscripten_get_canvas_element_size("canvas", &WIDTH, &HEIGHT);

  // avoid Gaps due to rounding
  WIDTH = (MIN(WIDTH, HEIGHT) / MAX_RECS) * MAX_RECS;
  HEIGHT = WIDTH;

  emscripten_set_canvas_element_size("canvas", WIDTH, HEIGHT);
  SetWindowSize( WIDTH,  HEIGHT);
  SetWindowMaxSize(WIDTH,  HEIGHT);
#endif

  // needs initialized Window
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
