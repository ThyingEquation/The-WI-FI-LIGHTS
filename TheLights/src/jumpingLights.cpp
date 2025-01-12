#include "jumpingLights.h"

static void jumpingLights1();
static void jumpingLights2();
static void jumpingLights3();
static void jumpingLights4();
static void jumpingSquare();
static void jumpingPoints();
static void initPoints();

static int8_t pos[2][8];
static byte dir[8];
static byte hue6;

void jumpingLights(uint8_t subMode) {
  static uint8_t firstStartPoints = 0;
  switch (subMode) {
    case 1:
      jumpingLights1();
      break;

    case 2:
      jumpingLights2();
      break;

    case 3:
      jumpingLights3();
      break;

    case 4:
      jumpingLights4();
      break;

    case 5:
      jumpingSquare();
      break;

    case 6:
      if (firstStartPoints == 0) {
        initPoints();
        firstStartPoints = 1;
      }
      jumpingPoints();
      break;

    default:
      break;
  }
}

static void move(byte id) {
  switch (dir[id]) {
    case 0:
      pos[1][id]++;
      break;
    case 1:
      pos[0][id]++;
      break;
    case 2:
      pos[1][id]--;
      break;
    case 3:
      pos[0][id]--;
      break;
  }
}

static void check1(byte id) {
  if (leds[pos[1][id] * mWidth + pos[0][id]] == CRGB(0, 0, 0))
    leds[XY(pos[0][id], pos[1][id])] = CHSV(hue6, 255, 255);
  else
    leds[XY(pos[0][id], pos[1][id])] = 0;
}

static void check2(byte id) {
  if (leds[pos[1][id] * mWidth + pos[0][id]] == CRGB(0, 0, 0))
    dir[id]++;
  else
    dir[id]--;
  if (dir[id] > 3)
    dir[id] = 0;
  else if (dir[id] < 0)
    dir[id] = 3;
}

static void check3(byte id) {
  if (pos[0][id] > mHeight - 1) pos[0][id] = 0;
  if (pos[1][id] > mWidth - 1) pos[1][id] = 0;
  if (pos[0][id] < 0) pos[0][id] = mHeight - 1;
  if (pos[1][id] < 0) pos[1][id] = mWidth - 1;
}

void jumpingLights1() {
  static bool setUp = true;
  static bool fade = false;

  if (setUp) {
    setUp = false;
    FastLED.clear();
    for (byte i = 0; i < 8; i++) {
      pos[0][i] = rand() % mHeight;
      pos[1][i] = rand() % mWidth;
      dir[i] = rand() % 3;
    }
  }

  for (byte i = 0; i < 8; i++) {
    check1(i);
    move(i);
    check3(i);
    check2(i);
  }
  hue6++;
  if (fade) fadeToBlackBy(leds, NUM_LEDS, 6);
  EVERY_N_SECONDS(1) { setUp = true; }
  FastLED.show();
}

static void drawPixelXYF(float x, float y, CRGB color) {
  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx,
          iy = 255 - yy;

#define WU_WEIGHT(a, b) ((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy), WU_WEIGHT(ix, yy),
                   WU_WEIGHT(xx, yy)};

  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XY(xn, yn)];
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    leds[XY(xn, yn)] = clr;
  }
}

void jumpingLights2() {
  static int lightersPosX6[32];
  static int lightersPosY6[32];
  static uint16_t lightersSpeedX6[32];
  static uint16_t lightersSpeedY6[32];
  static byte lightersSpeedZ[32];
  static byte lcolor6[32];
  static byte mass6[32];
  static bool loadingFlag6 = true;

  XYMap xyMap(mWidth, mHeight);
  if (loadingFlag6) {
    loadingFlag6 = false;
    randomSeed(millis());
    for (byte i = 0; i < 32; i++) {
      lightersSpeedX6[i] = -10 + rand() % (10 - (-10) + 1);
      lightersSpeedY6[i] = -10 + rand() % (10 - (-10) + 1);
      mass6[i] = 5 + rand() % (10 - 5 + 1);
      lightersSpeedZ[i] = 3 + rand() % (25 - 3 + 1);
      lightersPosX6[i] = rand() % mWidth * 10;
      lightersPosY6[i] = rand() % mHeight * 10;
      lcolor6[i] = (rand() % 9) * 28;
    }
  }

  switch (2) {
    case 0:
      FastLED.clear();
      break;
    case 1:
      fadeToBlackBy(leds, NUM_LEDS, 50);
      break;
    case 2:
      blur2d(leds, mWidth, mHeight, 30, xyMap);
      fadeToBlackBy(leds, NUM_LEDS, 5);
      break;
    case 3:
      fadeToBlackBy(leds, NUM_LEDS, 200);
      break;
  }

  for (byte i = 0; i < 32; i++) {
    lcolor6[i]++;
    switch (3) {
      case 0:
        lightersPosX6[i] +=
            beatsin88(lightersSpeedX6[0] * 255, 0,
                      mass6[i] / 10 * ((mHeight + mWidth) / 8)) -
            mass6[i] / 10 * ((mHeight + mWidth) / 16);
        lightersPosY6[i] +=
            beatsin88(lightersSpeedY6[0] * 255, 0,
                      mass6[i] / 10 * ((mHeight + mWidth) / 8)) -
            mass6[i] / 10 * ((mHeight + mWidth) / 16);
        break;
      case 1:
        lightersPosX6[i] = beatsin16(
            lightersSpeedX6[i] / map(255, 1, 255, 10, 1), 0, (mWidth - 1) * 10);
        lightersPosY6[i] =
            beatsin16(lightersSpeedY6[i] / map(255, 1, 255, 10, 1), 0,
                      (mHeight - 1) * 10);
        break;
      case 2:
        lightersPosX6[i] += lightersSpeedX6[i] / map(255, 1, 255, 10, 1);
        lightersPosY6[i] += lightersSpeedY6[i] / map(255, 1, 255, 10, 1);
        break;
      case 3:
        lightersPosX6[i] += mass6[i] * cos(radians(lightersSpeedY6[i])) /
                            map(255, 1, 255, 10, 1);
        lightersPosY6[i] += mass6[i] * sin(radians(lightersSpeedY6[i])) /
                            map(255, 1, 255, 10, 1);
        lightersSpeedY6[i] += lightersSpeedX6[i] / map(255, 1, 255, 20, 2);
        break;
    }

    if (lightersPosY6[i] < 0) {
      lightersPosY6[i] = 1;
      lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
    }
    if (lightersPosX6[i] < 0) {
      lightersPosX6[i] = 1;
      lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
    }
    if (lightersPosY6[i] >= (mHeight - 1) * 10) {
      lightersPosY6[i] = ((mHeight - 1) * 10) - 1;
      lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
    }
    if (lightersPosX6[i] >= (mWidth - 1) * 10) {
      lightersPosX6[i] = ((mWidth - 1) * 10) - 1;
      lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
    }

    CRGB color =
        CHSV(lcolor6[i], 255,
             beatsin8(lightersSpeedZ[i] / map(255, 1, 255, 10, 1), 128, 255));
    drawPixelXYF((float)lightersPosX6[i] / 10, (float)lightersPosY6[i] / 10,
                 color);
  }

  EVERY_N_SECONDS(10) {
    randomSeed(millis());
    for (byte i = 0; i < 32; i++) {
      lightersSpeedX6[i] = -10 + rand() % (10 - (-10) + 1);
      lightersSpeedY6[i] = rand() % 360;
      mass6[i] = 5 + rand() % (10 - 5 + 1);
      lightersSpeedZ[i] = 3 + rand() % (25 - 3 + 1);
    }
  }

  FastLED.delay(20);
  FastLED.show();
}

void jumpingLights3() {
  for (byte i = 8; i--;) {
    leds[XY(beatsin8(12 + i, 0, mWidth - 1),
            beatsin8(15 - i, 0, mHeight - 1))] =
        CHSV(beatsin8(12, 0, 255), 255, 255);
    XYMap xyMap(mWidth, mHeight);
    blur2d(leds, mWidth, mHeight, 16, xyMap);
  }
  FastLED.show();
}

uint16_t XYB3(uint8_t x, uint8_t y) { return (y * mWidth + x); }

void drawPixelXYFB3(float x, float y, const CRGB& color) {
  if (x < 0 || y < 0 || x > ((float)mWidth - 1) || y > ((float)mHeight - 1))
    return;

  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx,
          iy = 255 - yy;

#define WU_WEIGHT(a, b) ((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy), WU_WEIGHT(ix, yy),
                   WU_WEIGHT(xx, yy)};

  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XYB3(xn, yn)];
    if (xn < (int)mWidth - 1 && yn < (int)mHeight - 1 && yn > 0 && xn > 0) {
      clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
      clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
      clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    } else {
      clr.r = qadd8(clr.r, (color.r * 85) >> 8);
      clr.g = qadd8(clr.g, (color.g * 85) >> 8);
      clr.b = qadd8(clr.b, (color.b * 85) >> 8);
    }
    leds[XYB3(xn, yn)] = clr;
  }
#undef WU_WEIGHT
}

void drawCurve(float x, float y, float x2, float y2, float x3, float y3,
               CRGB coll) {
  float xu = 0.0, yu = 0.0, u = 0.0;
  for (u = 0.0; u <= 1.0; u += 0.01) {
    xu = pow(1 - u, 3) * x + 3 * u * pow(1 - u, 2) * x2 +
         3 * pow(u, 2) * (1 - u) * x3 + pow(u, 3) * x3;
    yu = pow(1 - u, 3) * y + 3 * u * pow(1 - u, 2) * y2 +
         3 * pow(u, 2) * (1 - u) * y3 + pow(u, 3) * y3;
    drawPixelXYFB3(xu, yu, coll);
  }
}
byte hue;

void jumpingLights4() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  byte x1 = beatsin8(18 + 100, 1, (mWidth - 2));
  byte x2 = beatsin8(23 + 100, 1, (mWidth - 2));
  byte x3 = beatsin8(27 + 100, 1, (mWidth - 2));

  byte y1 = beatsin8(20 + 100, 1, (mHeight - 2));
  byte y2 = beatsin8(26 + 100, 1, (mHeight - 2));
  byte y3 = beatsin8(15 + 100, 1, (mHeight - 2));

  drawCurve(x1, y1, x2, y2, x3, y3, CHSV(hue, 255, 255));
  hue++;
  FastLED.show();
}

void jumpingSquare() {
  static int squareX = 0;
  static int squareY = 0;
  static int directionX = 1;
  static int directionY = 1;
  static CRGB squareColor = CRGB::Red;

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int ledIndex = XY(squareX + i, squareY + j);
      if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
        leds[ledIndex] = squareColor;
      }
    }
  }
  FastLED.show();

  squareX += directionX;
  squareY += directionY;

  if (squareX + 3 >= mWidth || squareX < 0) {
    directionX = -directionX;
    squareX = max(0, min(squareX, mWidth - 3));
  }
  if (squareY + 3 >= mHeight || squareY < 0) {
    directionY = -directionY;
    squareY = max(0, min(squareY, mHeight - 3));
  }

  static int frameCount = 0;
  if (++frameCount >= 50) {
    squareColor = CHSV(random8(), 255, 255);
    frameCount = 0;
  }
  FastLED.delay(85);
}

struct Point {
  int x;
  int y;
  CRGB color;
  int directionX;
  int directionY;
};

static Point points[6];

void initPoints() {
  for (int i = 0; i < 6; i++) {
    points[i].x = rand() % mWidth;
    points[i].y = rand() % mHeight;
    points[i].color = CHSV(random8(), 255, 255);
    points[i].directionX = rand() % 2 == 0 ? 1 : -1;
    points[i].directionY = rand() % 2 == 0 ? 1 : -1;
  }
}

void jumpingPoints() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for (int i = 0; i < 6; i++) {
    int ledIndex = XY(points[i].x, points[i].y);
    if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
      leds[ledIndex] = points[i].color;
    }
  }
  FastLED.show();

  for (int i = 0; i < 6; i++) {
    points[i].x += points[i].directionX;
    points[i].y += points[i].directionY;

    if (points[i].x >= mWidth || points[i].x < 0) {
      points[i].directionX = -points[i].directionX;
      points[i].x = max(0, min(points[i].x, mWidth - 1));
    }
    if (points[i].y >= mHeight || points[i].y < 0) {
      points[i].directionY = -points[i].directionY;
      points[i].y = max(0, min(points[i].y, mHeight - 1));
    }
  }

  for (int i = 0; i < 6; i++) {
    points[i].color = CHSV(random8(), 255, 255);
  }
  FastLED.delay(45);
}