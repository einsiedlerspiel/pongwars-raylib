/* Copyright (c) 2024 Lou Woell */

#include <math.h>
#include <raylib.h>

// Color Scheme of my Website
#define DAY_COLOR               CLITERAL(Color) { 238, 114, 241, 255}
#define NIGHT_COLOR             CLITERAL(Color) { 33, 32, 44, 255}

// Alternative Neutral Colors
/* #define DAY_COLOR               SKYBLUE */
/* #define NIGHT_COLOR             DARKBLUE */

#define HEIGHT                  600
#define WIDTH                   600

#define SQUARE_SIZE             25

#define MAX_RECS_X              WIDTH/SQUARE_SIZE
#define MAX_RECS_Y              HEIGHT/SQUARE_SIZE

#define FONTSIZE                30
#define PAUSE_TEXT              "PAUSED"

typedef struct {
  Vector2 Position;
  Vector2 Speed;
  float Radius;
  Color Color;
} bBall;

typedef struct {
  Rectangle recs[MAX_RECS_X * MAX_RECS_Y];
  Color colors[MAX_RECS_X * MAX_RECS_Y];
} Board;

typedef struct {
  Board board;
  bBall DayBall;
  bBall NightBall;
} Game;

void MakeBoard(Board *board) {
  for (int y = 0; y < MAX_RECS_Y; y++)
    {
      for (int x = 0; x < MAX_RECS_X; x++)
        {
	  board->recs[y*MAX_RECS_X + x].x = SQUARE_SIZE/2.0f + SQUARE_SIZE*x;
	  board->recs[y*MAX_RECS_X + x].y = SQUARE_SIZE/2.0f + SQUARE_SIZE*y;
	  board->recs[y*MAX_RECS_X + x].width = SQUARE_SIZE;
	  board->recs[y*MAX_RECS_X + x].height = SQUARE_SIZE;
	  if (MAX_RECS_X/2 > x) {
              board->colors[y * MAX_RECS_X + x] = DAY_COLOR;
		}
	  else {
            board->colors[y * MAX_RECS_X + x] = NIGHT_COLOR;
	  }
        }
    }
}

void DrawBoard(Board *board) {
  for (int i = 0; i < MAX_RECS_X * MAX_RECS_Y; i++) {
        DrawRectanglePro(
            board->recs[i],
            (Vector2){board->recs[i].width / 2, board->recs[i].height / 2},
            0,
            board->colors[i]);
  }
}

int RandomOffset(int offset)
{
  return GetRandomValue(-(offset), (offset));
}                       

bool coloreq(Color first, Color second) {
    if (first.r == second.r &&
        first.g == second.g &&
        first.b == second.b &&
	first.a == second.a) return true;
  return false;
}

void flipColor(Color *tile) {
  if (coloreq(*tile, DAY_COLOR)) {
    *tile = NIGHT_COLOR;
  } else {
    *tile = DAY_COLOR;
  }
}

void MakeBouncingBall(bBall * ball, Color color, float startx, float starty) {

  float xspeed = 12.5;
  float yspeed = 12.5;

  if (startx > WIDTH / 2.0) {
    xspeed *= -1;
  } else {
    yspeed *= -1;
  }

  ball->Position = (Vector2){ startx, starty};
  ball->Speed = (Vector2){ xspeed, yspeed };
  ball->Radius = SQUARE_SIZE*0.5;
  ball->Color = color;
}

void BouncingBallPosition(bBall *ball, Color *color_array) {
  ball->Position.x += ball->Speed.x;
  ball->Position.y += ball->Speed.y;
  
  // Check walls collision
  if ((ball->Position.x >= (WIDTH - ball->Radius)) ||
      (ball->Position.x <= ball->Radius)) ball->Speed.x *= -1.0f;
  if ((ball->Position.y >= (HEIGHT - ball->Radius)) ||
      (ball->Position.y <= ball->Radius)) ball->Speed.y *= -1.0f;

  // Check Colour collision 
  for (double angle = 0; angle < (2 * PI); angle += (PI / 4)) {

    int i = floor((ball->Position.x + cos(angle) * ball->Radius) / SQUARE_SIZE);
    int j = floor((ball->Position.y + sin(angle) * ball->Radius) / SQUARE_SIZE);

    if (i >= 0 && i < MAX_RECS_X && j >= 0 && j < MAX_RECS_Y) {

      int k = j * MAX_RECS_X + i;

      if (coloreq(color_array[k], ball->Color)) {
	flipColor(&(color_array[k]));
	// Determine bounce direction based on the angle
	if (fabs(cos(angle)) > fabs(sin(angle))) {
	  ball->Speed.x *= -1;
	} else {
	  ball->Speed.y *= -1;
	}
      }
    }
  }
}

void DrawBouncingBall(bBall * ball) {
    /* Draw Bouncing Ball */
    DrawCircleV(ball->Position, ball->Radius, ball->Color);
}

void SetGame(Game *game) {
  MakeBoard(&game->board);
  MakeBouncingBall(&game->DayBall,
                   DAY_COLOR,
                   (WIDTH / 4.0f) * 3 + RandomOffset(WIDTH / 4),
                   HEIGHT/2.0f + RandomOffset(HEIGHT/2));
  MakeBouncingBall(&game->NightBall,
                   NIGHT_COLOR,
                   (WIDTH / 4.0f) + RandomOffset(WIDTH / 4),
                   HEIGHT/2.0f + RandomOffset(HEIGHT/2));
}

void DrawGame(Game *game) {
  DrawBoard(&game->board);
  DrawBouncingBall(&game->DayBall);
  DrawBouncingBall(&game->NightBall);
}

int main() {

  const char * title = "Pong Wars";

  InitWindow(WIDTH,HEIGHT, title);

  bool pause = 0;
  int framesCounter = 0;

  const int pause_text_width = MeasureText(PAUSE_TEXT, FONTSIZE);

  Game game = {0};
  SetGame(&game);

  SetTargetFPS(60); 
  while(!WindowShouldClose())
    {
      // Press P to Pause
      if (IsKeyPressed(KEY_P)) pause = !pause;

      // Press R to reset
      if (IsKeyPressed(KEY_R)) SetGame(&game);

      if (!pause) {
	BouncingBallPosition(&game.DayBall, game.board.colors);
	BouncingBallPosition(&game.NightBall, game.board.colors);
      }
      else framesCounter++;

      BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawGame(&game);

      // On pause, we draw a blinking message
      if (pause && ((framesCounter/30)%2)) DrawText(PAUSE_TEXT, (WIDTH - pause_text_width)/2, HEIGHT/3, FONTSIZE, WHITE);

      EndDrawing();
    }

  CloseWindow();
  return 0;
}
