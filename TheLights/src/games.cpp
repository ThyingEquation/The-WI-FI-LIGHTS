#include "games.h"

static void snakeGame();
static void tetrisGame();
static void arkanoidGame();

extern uint8_t mainScheme[];

void games(uint8_t subMode) {
  switch (subMode) {
    case 1:
      snakeGame();
      break;

    case 2:
      tetrisGame();
      break;

    case 3:
      arkanoidGame();
      break;

    default:
      break;
  }
}

static int snakeX[5];
static int snakeY[5];
static int foodX, foodY;

static void updateSnake() {
  static int snakeDirection = 0;

  for (int i = 5 - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  switch (snakeDirection) {
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

  if (snakeX[0] < 0) snakeX[0] = mWidth - 1;
  if (snakeX[0] >= mWidth) snakeX[0] = 0;
  if (snakeY[0] < 0) snakeY[0] = mHeight - 1;
  if (snakeY[0] >= mHeight) snakeY[0] = 0;

  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    foodX = rand() % mWidth;
    foodY = rand() % mHeight;
  }

  if (rand() % 10 == 0) {
    snakeDirection = rand() % 4;
  }
}

void snakeGame() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  updateSnake();

  for (int i = 0; i < 5; i++) {
    int ledIndex = XY(snakeX[i], snakeY[i]);
    if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
      leds[ledIndex] = CRGB::Green;
    }
  }

  int ledIndex = XY(foodX, foodY);
  if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
    leds[ledIndex] = CRGB::White;
  }

  FastLED.show();
  FastLED.delay(8000 / 60);
}

const int figures[14][4][2] = {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{0, 0}, {1, 0}, {2, 0}, {1, 1}}, {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
    {{0, 0}, {0, 1}, {1, 1}, {2, 1}}, {{0, 0}, {1, 0}, {2, 0}, {2, 1}},
    {{0, 0}, {1, 0}, {2, 0}, {0, 1}}, {{0, 0}, {0, 1}, {0, 2}, {0, 3}},
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}}, {{0, 0}, {0, 1}, {0, 2}, {1, 1}},
    {{0, 0}, {0, 1}, {1, 1}, {1, 2}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{0, 0}, {0, 1}, {0, 2}, {1, 2}}, {{0, 0}, {0, 1}, {0, 2}, {1, 0}}};

struct FallingFigure {
  int figureIndex;
  int x;
  int y;
  CRGB color;
};

FallingFigure fallingFigures[5];
int numFallingFigures = 0;

static bool checkCollision(int figureIndex, int x, int y) {
  for (int i = 0; i < 4; i++) {
    int px = x + figures[figureIndex][i][0];
    int py = y + figures[figureIndex][i][1];
    if (px < 0 || px >= mWidth || py < 0 || py >= mHeight) {
      return true;
    }
    for (int j = 0; j < numFallingFigures; j++) {
      FallingFigure &fig = fallingFigures[j];
      for (int k = 0; k < 4; k++) {
        int fx = fig.x + figures[fig.figureIndex][k][0];
        int fy = fig.y + figures[fig.figureIndex][k][1];
        if (abs(px - fx) <= 2 && abs(py - fy) <= 2) {
          return true;
        }
      }
    }
  }
  return false;
}

static void addNewFigure() {
  if (numFallingFigures < 5) {
    int figureIndex = rand() % 14;
    int x, y;
    int attempts = 0;
    do {
      x = rand() % 12;
      y = 0;  // rand() % 1;
      attempts++;
      if (attempts > 100) {
        return;
      }
    } while (checkCollision(figureIndex, x, y));
    CRGB color = CHSV(random8(), 255, 255);
    fallingFigures[numFallingFigures] = {figureIndex, x, y, color};
    numFallingFigures++;
  }
}

void tetrisGame() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  for (int i = 0; i < numFallingFigures; i++) {
    FallingFigure &fig = fallingFigures[i];
    fig.y++;
    if (fig.y >= mHeight + 3) {
      for (int j = i; j < numFallingFigures - 1; j++) {
        fallingFigures[j] = fallingFigures[j + 1];
      }
      numFallingFigures--;
    } else {
      for (int i = 0; i < 4; i++) {
        int px = fig.x + figures[fig.figureIndex][i][0];
        int py = fig.y + figures[fig.figureIndex][i][1];
        if (px >= 0 && px < mWidth && py >= 0 && py < mHeight) {
          leds[XY(px, py)] = fig.color;
        }
      }
    }
  }
  if (rand() % 10 < 4) {
    addNewFigure();
  }
  delay(25);
  FastLED.show();
  delay(350);
}

void arkanoidGame() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  static CRGB tileColors[] = {CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Red};
  const int tileWidth = 3, tilesPerRow = 4, totalTiles = 20;
  const int platformWidth = 4;
  const int ballMinY = 0, ballMaxY = 7;

  for (int i = 0; i < totalTiles; i++) {
    int row = i / tilesPerRow;
    int col = i % tilesPerRow;
    int startIndex = row * tileWidth * tilesPerRow + col * tileWidth;

    for (int j = 0; j < tileWidth; j++) {
      int ledIndex = mainScheme[startIndex + j] - 1;
      if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
        leds[ledIndex] = tileColors[col];
      }
    }
  }

  static int platformPosition = 4;
  static int platformDirection = 1;
  int platformRowOffset = (NUM_LEDS / 12) - 1;

  platformPosition += platformDirection;
  if (platformPosition >= 12 - platformWidth || platformPosition < 0) {
    platformDirection = -platformDirection;
    platformPosition += platformDirection;
  }

  for (int i = 0; i < platformWidth; i++) {
    int ledIndex =
        mainScheme[(platformRowOffset * 12) + platformPosition + i] - 1;

    if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
      leds[ledIndex] =
          (i == 0 || i == platformWidth - 1) ? CRGB::Red : CRGB::White;
    }
  }

  static int ballX = 6, ballY = 5, ballDX = 1, ballDY = -1;

  leds[XY(ballX, ballY)] = CRGB::Black;

  ballX += ballDX;
  ballY += ballDY;

  if (ballX <= 0 || ballX >= 11) {
    ballDX = -ballDX;
  }
  if (ballY >= ballMaxY || ballY <= ballMinY) {
    ballDY = -ballDY;
  }

  leds[XY(ballX, ballY)] = CRGB::White;

  FastLED.show();
  delay(100);
}