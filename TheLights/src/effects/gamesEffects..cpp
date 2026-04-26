#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (gamesSettings):
    1) Скорости эффектов
*/

enum gamesSettings
{
  SNAKE_GAME_DELAY = 8000 / 60,
  TETRIS_GAME_DELAY = 350,
  ARKANOID_GAME_DELAY = 100
};

static void drawSnakeGame();
static void drawTetrisGame();
static void drawArkanoidGame();

static int8_t snakeX[5];
static int8_t snakeY[5];
static int8_t foodX, foodY;

static uint8_t numFallingFigures = 0;

struct FallingFigure
{
  int8_t figureIndex;
  int8_t x;
  int8_t y;
  CRGB color;
};

static FallingFigure fallingFigures[5];

const uint8_t figures[14][4][2] = {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{0, 0}, {1, 0}, {2, 0}, {1, 1}}, {{0, 0}, {1, 0}, {1, 1}, {2, 1}}, {{0, 0}, {0, 1}, {1, 1}, {2, 1}}, {{0, 0}, {1, 0}, {2, 0}, {2, 1}}, {{0, 0}, {1, 0}, {2, 0}, {0, 1}}, {{0, 0}, {0, 1}, {0, 2}, {0, 3}}, {{0, 0}, {0, 1}, {1, 0}, {1, 1}}, {{0, 0}, {0, 1}, {0, 2}, {1, 1}}, {{0, 0}, {0, 1}, {1, 1}, {1, 2}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{0, 0}, {0, 1}, {0, 2}, {1, 2}}, {{0, 0}, {0, 1}, {0, 2}, {1, 0}}};

void drawGamesEffects(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawSnakeGame();
    break;

  case 2:
    drawTetrisGame();
    break;

  case 3:
    drawArkanoidGame();
    break;

  default:
    break;
  }
}

static void updateSnake()
{
  static uint8_t snakeDirection = 0;

  for (uint8_t i = 5 - 1; i > 0; i--)
  {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  switch (snakeDirection)
  {
  case 0:
    snakeY[0]--;
    break;
  case 1:
    snakeX[0]++;
    break;
  case 2:
    snakeY[0]++;
    break;
  case 3:
    snakeX[0]--;
    break;
  }

  if (snakeX[0] < 0)
    snakeX[0] = MATRIX_WIDTH - 1;
  if (snakeX[0] >= MATRIX_WIDTH)
    snakeX[0] = 0;
  if (snakeY[0] < 0)
    snakeY[0] = MATRIX_HEIGHT - 1;
  if (snakeY[0] >= MATRIX_HEIGHT)
    snakeY[0] = 0;

  if (snakeX[0] == foodX && snakeY[0] == foodY)
  {
    foodX = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
    foodY = ESP8266TrueRandom.random(0, MATRIX_HEIGHT);
  }

  if (ESP8266TrueRandom.random(0, 10) == 0)
  {
    snakeDirection = ESP8266TrueRandom.random(0, 4);
  }
}

static void drawSnakeGame()
{
  fill_solid(leds, MATRIX_LEDS, CRGB::Black);
  updateSnake();

  for (uint8_t i = 0; i < 5; i++)
  {
    int16_t ledIndex = XY(snakeX[i], snakeY[i]);
    if (ledIndex >= 0 && ledIndex < MATRIX_LEDS)
    {
      leds[ledIndex] = CRGB::Green;
    }
  }

  int16_t ledIndex = XY(foodX, foodY);
  if (ledIndex >= 0 && ledIndex < MATRIX_LEDS)
  {
    leds[ledIndex] = CRGB::White;
  }

  FastLED.show();
  FastLED.delay(SNAKE_GAME_DELAY);
}

static bool checkCollision(uint8_t figureIndex, int8_t x, int8_t y)
{
  for (uint8_t i = 0; i < 4; i++)
  {
    int8_t px = x + figures[figureIndex][i][0];
    int8_t py = y + figures[figureIndex][i][1];

    if (px < 0 || px >= MATRIX_WIDTH || py < -4)
    {
      return true;
    }

    for (uint8_t j = 0; j < numFallingFigures; j++)
    {
      FallingFigure &fig = fallingFigures[j];
      for (uint8_t k = 0; k < 4; k++)
      {
        int8_t fx = fig.x + figures[fig.figureIndex][k][0];
        int8_t fy = fig.y + figures[fig.figureIndex][k][1];

        if (px == fx && py == fy)
        {
          return true;
        }
      }
    }
  }
  return false;
}

static void addNewFigure()
{
  if (numFallingFigures < 6)
  {
    int8_t figureIndex = ESP8266TrueRandom.random(0, 14);
    int8_t x, y;
    uint8_t attempts = 0;
    do
    {
      x = ESP8266TrueRandom.random(0, MATRIX_WIDTH - 2);
      y = MATRIX_HEIGHT - 1;
      attempts++;
      if (attempts > 20)
        return;
    } while (checkCollision(figureIndex, x, y));

    CRGB color = CHSV(random8(), 255, 255);
    fallingFigures[numFallingFigures] = {figureIndex, x, y, color};
    numFallingFigures++;
  }
}

static void drawTetrisGame()
{
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    leds[i] = CRGB::Black;

  for (int8_t i = 0; i < numFallingFigures; i++)
  {
    FallingFigure &fig = fallingFigures[i];
    fig.y--;

    if (fig.y < -4)
    {
      for (uint8_t j = i; j < numFallingFigures - 1; j++)
      {
        fallingFigures[j] = fallingFigures[j + 1];
      }
      numFallingFigures--;
      i--;
    }
    else
    {
      for (uint8_t f = 0; f < 4; f++)
      {
        int16_t px = (int16_t)fig.x + figures[fig.figureIndex][f][0];
        int16_t py = (int16_t)fig.y + figures[fig.figureIndex][f][1];

        if (px >= 0 && px < MATRIX_WIDTH && py >= 0 && py < MATRIX_HEIGHT)
        {
          leds[XY(px, py)] = fig.color;
        }
      }
    }
  }

  if (random8(100) < 25)
  {
    addNewFigure();
  }

  FastLED.show();
  delay(TETRIS_GAME_DELAY);
}

static void drawArkanoidGame()
{
  fill_solid(leds, MATRIX_LEDS, CRGB::Black);

  static CRGB tileColors[] = {CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Red};
  const uint8_t tileWidth = 3, tilesPerRow = 4, totalTiles = 20;
  const uint8_t platformWidth = 4;
  const uint8_t ballMinY = 0, ballMaxY = 7;

  for (uint8_t i = 0; i < totalTiles; i++)
  {
    uint8_t row = i / tilesPerRow;
    uint8_t col = i % tilesPerRow;
    uint8_t startIndex = row * tileWidth * tilesPerRow + col * tileWidth;

    for (uint8_t j = 0; j < tileWidth; j++)
    {
      uint8_t ledIndex = mainMatrixScheme[startIndex + j] - 1;
      if (ledIndex >= 0 && ledIndex < MATRIX_LEDS)
      {
        leds[ledIndex] = tileColors[col];
      }
    }
  }

  static int16_t platformPosition = 4;
  static int16_t platformDirection = 1;
  uint8_t platformRowOffset = (MATRIX_LEDS / 12) - 1;

  platformPosition += platformDirection;
  if (platformPosition >= 12 - platformWidth || platformPosition < 0)
  {
    platformDirection = -platformDirection;
    platformPosition += platformDirection;
  }

  for (uint8_t i = 0; i < platformWidth; i++)
  {
    uint8_t ledIndex =
        mainMatrixScheme[(platformRowOffset * 12) + platformPosition + i] - 1;

    if (ledIndex >= 0 && ledIndex < MATRIX_LEDS)
    {
      leds[ledIndex] =
          (i == 0 || i == platformWidth - 1) ? CRGB::Red : CRGB::White;
    }
  }

  static uint8_t ballX = 6, ballY = 5, ballDX = 1, ballDY = -1;

  leds[XY(ballX, ballY)] = CRGB::Black;

  ballX += ballDX;
  ballY += ballDY;

  if (ballX <= 0 || ballX >= 11)
  {
    ballDX = -ballDX;
  }
  if (ballY >= ballMaxY || ballY <= ballMinY)
  {
    ballDY = -ballDY;
  }

  leds[XY(ballX, ballY)] = CRGB::White;

  FastLED.show();
  delay(ARKANOID_GAME_DELAY);
}