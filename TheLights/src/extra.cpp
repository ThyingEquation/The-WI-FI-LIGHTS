#include <extra.h>

int squareX = 0;
int squareY = 0;
int directionX = 1;
int directionY = 1;
CRGB squareColor = CRGB::Red;

struct Point {
  int x;
  int y;
  CRGB color;
  int directionX;
  int directionY;
};

Point points[6];

void updatePointPosition(Point &point);
void updatePointColor(Point &point);

void initPoints() {
  for (int i = 0; i < 6; i++) {
    points[i].x = random(kMatrixWidth);
    points[i].y = random(kMatrixHeight);
    points[i].color = CHSV(random8(), 255, 255);
    points[i].directionX = random(2) == 0 ? 1 : -1;
    points[i].directionY = random(2) == 0 ? 1 : -1;
  }
}

int spiralIndex = 0;
int snakeX[5];
int snakeY[5];
int snakeDirection = 0;

void initSnake() {
  for (int i = 0; i < 5; i++) {
    snakeX[i] = kMatrixWidth / 2;
    snakeY[i] = kMatrixHeight / 2 + i;
  }
  spawnFood();
}

int foodX, foodY;

const int ledMatrix[13][12] = {
    {143, 142, 117, 116, 91, 90, 65, 64, 39, 38, 13, 12},
    {144, 141, 118, 115, 92, 89, 66, 63, 40, 37, 14, 11},
    {145, 140, 119, 114, 93, 88, 67, 62, 41, 36, 15, 10},
    {146, 139, 120, 113, 94, 87, 68, 61, 42, 35, 16, 9},
    {147, 138, 121, 112, 95, 86, 69, 60, 43, 34, 17, 8},
    {148, 137, 122, 111, 96, 85, 70, 59, 44, 33, 18, 7},
    {149, 136, 123, 110, 97, 84, 71, 58, 45, 32, 19, 6},
    {150, 135, 124, 109, 98, 83, 72, 57, 46, 31, 20, 5},
    {151, 134, 125, 108, 99, 82, 73, 56, 47, 30, 21, 4},
    {152, 133, 126, 107, 100, 81, 74, 55, 48, 29, 22, 3},
    {153, 132, 127, 106, 101, 80, 75, 54, 49, 28, 23, 2},
    {154, 131, 128, 105, 102, 79, 76, 53, 50, 27, 24, 1},
    {155, 130, 129, 104, 103, 78, 77, 52, 51, 26, 25, 0}};

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

int platformPosition = 0;
int platformDirection = 1;
int ballX = 6;
int ballY = 11;
int ballDX = 1;
int ballDY = -1;

static int startColE[11] = {10, 0, 5, 1, 3, 11, 6, 7, 3, 8, 2};
static int startRowE[11] = {2, 2, 0, 3, 2, 3, 3, 1, 3, 2, 1};
static int currentRowE[11] = {10, 0, 5, 1, 3, 11, 6, 7, 3, 8, 2};
static int lineLengthE[11] = {5, 8, 8, 6, 8, 7, 8, 9, 8, 9, 8};

const int ledMatrix1[30][12] = {
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {143, 142, 117, 116, 91, 90, 65, 64, 39, 38, 13, 12},
    {144, 141, 118, 115, 92, 89, 66, 63, 40, 37, 14, 11},
    {145, 140, 119, 114, 93, 88, 67, 62, 41, 36, 15, 10},
    {146, 139, 120, 113, 94, 87, 68, 61, 42, 35, 16, 9},
    {147, 138, 121, 112, 95, 86, 69, 60, 43, 34, 17, 8},
    {148, 137, 122, 111, 96, 85, 70, 59, 44, 33, 18, 7},
    {149, 136, 123, 110, 97, 84, 71, 58, 45, 32, 19, 6},
    {150, 135, 124, 109, 98, 83, 72, 57, 46, 31, 20, 5},
    {151, 134, 125, 108, 99, 82, 73, 56, 47, 30, 21, 4},
    {152, 133, 126, 107, 100, 81, 74, 55, 48, 29, 22, 3},
    {153, 132, 127, 106, 101, 80, 75, 54, 49, 28, 23, 2},
    {154, 131, 128, 105, 102, 79, 76, 53, 50, 27, 24, 1},
    {155, 130, 129, 104, 103, 78, 77, 52, 51, 26, 25, 0},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157}};

void extra() {
  switch (choosenModeD2) {
    case 1:
      extra1();
      break;

    case 2:
      extra2();
      break;

    case 3:
      extra3();
      break;

    case 4:
      extra4();
      break;

    case 5:
      extra5();
      break;

    case 6:
      extra6();
      break;

    case 7:
      extra7();
      break;

    default:
      break;
  }
}

void drawSquare(int x, int y, CRGB color) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int ledIndex = XY(x + i, y + j);
      if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
        leds[ledIndex] = color;
      }
    }
  }
}

void updateSquarePosition() {
  squareX += directionX;
  if (squareX + 3 >= kMatrixWidth || squareX < 0) {
    directionX = -directionX;
  }

  squareY += directionY;
  if (squareY + 3 >= kMatrixHeight || squareY < 0) {
    directionY = -directionY;
  }
}

void updateSquareColor() {
  static int frameCount = 0;
  frameCount++;
  if (frameCount >= 50) {
    squareColor = CHSV(random8(), 255, 255);
    frameCount = 0;
  }
}

int XY(int x, int y) {
  if (x % 2 == 0) {
    return x * kMatrixHeight + y;
  } else {
    return x * kMatrixHeight + (kMatrixHeight - 1 - y);
  }
}

void drawPoint(int x, int y, CRGB color) {
  int ledIndex = XY(x, y);
  if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
    leds[ledIndex] = color;
  }
}

void updatePointPosition(Point &point) {
  point.x += point.directionX;
  if (point.x >= kMatrixWidth || point.x < 0) {
    point.directionX = -point.directionX;
  }

  point.y += point.directionY;
  if (point.y >= kMatrixHeight || point.y < 0) {
    point.directionY = -point.directionY;
  }
}

void updatePointColor(Point &point) {
  static int frameCount = 0;
  frameCount++;
  if (frameCount >= 50) {
    point.color = CHSV(random8(), 255, 255);
    frameCount = 0;
  }
}

void drawSpiral() {
  int x = 0, y = 0;
  int dx = 0, dy = -1;
  int t = kMatrixWidth;
  int maxI = t * t;

  for (int i = 0; i < maxI; i++) {
    if ((-kMatrixWidth / 2 <= x) && (x < kMatrixWidth / 2) &&
        (-kMatrixHeight / 2 <= y) && (y < kMatrixHeight / 2)) {
      int ledIndex = XY(x + kMatrixWidth / 2, y + kMatrixHeight / 2);
      if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
        leds[ledIndex] = CHSV((i + spiralIndex) % 256, 255, 255);
      }
    }
    if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))) {
      t = dx;
      dx = -dy;
      dy = t;
    }
    x += dx;
    y += dy;
  }
}

void updateSnake() {
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

  if (snakeX[0] < 0) snakeX[0] = kMatrixWidth - 1;
  if (snakeX[0] >= kMatrixWidth) snakeX[0] = 0;
  if (snakeY[0] < 0) snakeY[0] = kMatrixHeight - 1;
  if (snakeY[0] >= kMatrixHeight) snakeY[0] = 0;

  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    spawnFood();
  }

  if (random(10) == 0) {
    snakeDirection = random(4);
  }
}

void drawSnake() {
  for (int i = 0; i < 5; i++) {
    int ledIndex = XY(snakeX[i], snakeY[i]);
    if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
      leds[ledIndex] = CRGB::Green;
    }
  }
}

void drawFood() {
  int ledIndex = XY(foodX, foodY);
  if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
    leds[ledIndex] = CRGB::White;
  }
}

void spawnFood() {
  foodX = random(kMatrixWidth);
  foodY = random(kMatrixHeight);
}

void drawFigure(int figureIndex, int x, int y, CRGB color) {
  for (int i = 0; i < 4; i++) {
    int px = x + figures[figureIndex][i][0];
    int py = y + figures[figureIndex][i][1];
    if (px >= 0 && px < 12 && py >= 0 && py < 13) {
      int ledIndex = ledMatrix[py][px];
      leds[ledIndex] = color;
    }
  }
}

bool checkCollision(int figureIndex, int x, int y) {
  for (int i = 0; i < 4; i++) {
    int px = x + figures[figureIndex][i][0];
    int py = y + figures[figureIndex][i][1];
    if (px < 0 || px >= 12 || py < 0 || py >= 13) {
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

void addNewFigure() {
  if (numFallingFigures < 5) {
    int figureIndex = random(0, 14);
    int x, y;
    int attempts = 0;
    do {
      x = random(0, 12);
      y = random(0, 1);
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

void drawTiles() {
  CRGB colors[] = {CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Red};
  int tileWidth = 3;
  int tilesPerRow = 4;
  int totalTiles = 20;

  for (int i = 0; i < totalTiles; i++) {
    int row = i / tilesPerRow;
    int col = i % tilesPerRow;
    int startIndex = row * tileWidth * tilesPerRow + col * tileWidth;

    for (int j = 0; j < tileWidth; j++) {
      int ledIndex = mainScheme[startIndex + j] - 1;
      leds[ledIndex] = colors[col];
    }
  }
}

void movePlatform() {
  int platformWidth = 4;
  int platformHeight = 1;
  int rowOffset = (NUM_LEDS / 12) - platformHeight;

  for (int i = 0; i < platformWidth; i++) {
    int ledIndex = mainScheme[(rowOffset * 12) + platformPosition + i] - 1;
    leds[ledIndex] = CRGB::Black;
  }

  platformPosition += platformDirection;
  if (platformPosition >= 12 - platformWidth || platformPosition < 0) {
    platformDirection = -platformDirection;
    platformPosition += platformDirection;
  }

  for (int i = 0; i < platformWidth; i++) {
    int ledIndex = mainScheme[(rowOffset * 12) + platformPosition + i] - 1;
    if (i == 0 || i == platformWidth - 1) {
      leds[ledIndex] = CRGB::Red;
    } else {
      leds[ledIndex] = CRGB::White;
    }
  }
}

void moveBall() {
  int ballIndex = ballY * 12 + ballX;
  leds[mainScheme[ballIndex] - 1] = CRGB::Black;

  ballX += ballDX;
  ballY += ballDY;

  if (ballX <= 0 || ballX >= 11) {
    ballDX = -ballDX;
  }

  if (ballY >= 12 || ballY <= 5) {
    ballDY = -ballDY;
  }

  if (ballY == 11 && ballX >= platformPosition &&
      ballX < platformPosition + 4) {
    ballDY = -ballDY;
  }

  ballIndex = ballY * 12 + ballX;
  leds[mainScheme[ballIndex] - 1] = CRGB::White;
}

void clearAll() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}

uint8_t rE, gE, bE;
uint8_t regionE, remainderE, pE, qE, tE;

uint32_t HSVtoRGB(uint8_t hE, uint8_t sE, uint8_t vE) {
  if (sE == 0) {
    rE = vE;
    gE = vE;
    bE = vE;
    return strip.Color(rE, gE, bE);
  }

  regionE = hE / 43;
  remainderE = (hE - (regionE * 43)) * 6;

  pE = (vE * (255 - sE)) >> 8;
  qE = (vE * (255 - ((sE * remainderE) >> 8))) >> 8;
  tE = (vE * (255 - ((sE * (255 - remainderE)) >> 8))) >> 8;

  switch (regionE) {
    case 0:
      rE = vE;
      gE = tE;
      bE = pE;
      break;
    case 1:
      rE = qE;
      gE = vE;
      bE = pE;
      break;
    case 2:
      rE = pE;
      gE = vE;
      bE = tE;
      break;
    case 3:
      rE = pE;
      gE = qE;
      bE = vE;
      break;
    case 4:
      rE = tE;
      gE = pE;
      bE = vE;
      break;
    default:
      rE = vE;
      gE = pE;
      bE = qE;
      break;
  }

  return strip.Color(rE, gE, bE);
}

void extra1() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  drawSquare(squareX, squareY, squareColor);
  FastLED.show();
  updateSquarePosition();
  updateSquareColor();
  FastLED.delay(5000 / 60);
}
void extra2() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i = 0; i < 6; i++) {
    drawPoint(points[i].x, points[i].y, points[i].color);
  }
  FastLED.show();
  for (int i = 0; i < 6; i++) {
    updatePointPosition(points[i]);
  }
  for (int i = 0; i < 6; i++) {
    updatePointColor(points[i]);
  }
  FastLED.delay(2200 / 60);
}

bool forward = true;

void extra3() {
  delay(100);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  drawSpiral();
  FastLED.show();
  spiralIndex = (spiralIndex + (forward ? 1 : -1)) % NUM_LEDS;
  FastLED.delay(6000 / 60);

  if (spiralIndex == 0 || spiralIndex == NUM_LEDS - 1) {
    forward = !forward;
  }
}

void extra4() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  updateSnake();
  drawSnake();
  drawFood();
  FastLED.show();
  FastLED.delay(8000 / 60);
}
void extra5() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  for (int i = 0; i < numFallingFigures; i++) {
    FallingFigure &fig = fallingFigures[i];
    fig.y++;
    if (fig.y >= 13) {
      for (int j = i; j < numFallingFigures - 1; j++) {
        fallingFigures[j] = fallingFigures[j + 1];
      }
      numFallingFigures--;
    } else {
      drawFigure(fig.figureIndex, fig.x, fig.y, fig.color);
    }
  }
  if (random(10) < 3) {
    addNewFigure();
  }
  FastLED.show();
  delay(500);
}
void extra6() {
  clearAll();
  drawTiles();
  movePlatform();
  moveBall();
  FastLED.show();
  delay(100);
}
void extra7() {
  for (int i = 0; i < 11; i++) {
    if (currentRowE[i] < 20) {
      for (int j = 0; j < lineLengthE[i]; j++) {
        int rowIndexE = currentRowE[i] + j;
        int colIndexE = startColE[i];
        if (rowIndexE < 30 && colIndexE < 12) {
          int pixelIndexE = ledMatrix1[rowIndexE][colIndexE];
          int brightnessE;
          if (j == lineLengthE[i] - 1) {
            brightnessE = 255;
          } else {
            brightnessE = 5 + (5 * j);
          }
          uint32_t colorE = HSVtoRGB(90, 255, brightnessE);
          if (pixelIndexE <= 155) {
            strip.setPixelColor(pixelIndexE, colorE);
          }
        }
      }
    }
  }

  strip.show();
  delay(120);

  for (int i = 0; i < 11; i++) {
    if (currentRowE[i] < 20) {
      for (int j = 0; j < lineLengthE[i]; j++) {
        int rowIndexE = currentRowE[i] + j;
        int colIndexE = startColE[i];
        if (rowIndexE < 30 && colIndexE < 12) {
          int pixelIndexE = ledMatrix1[rowIndexE][colIndexE];
          if (pixelIndexE <= 155) {
            strip.setPixelColor(pixelIndexE, strip.Color(0, 0, 0));
          }
        }
      }
    }
  }

  strip.show();

  for (int i = 0; i < 11; i++) {
    if (currentRowE[i] < 20) {
      currentRowE[i]++;
    } else {
      startColE[i] = random(12);
      startRowE[i] = random(4);
      currentRowE[i] = startRowE[i];
      lineLengthE[i] = random(5, 9);
    }
  }
}