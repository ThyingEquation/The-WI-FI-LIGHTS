#include "space.h"

static void starSky();
static void spaceship();
static void starFall();
static void pulsar();

static void colorfulSpiral();

void space(uint8_t subMode) {
  switch (subMode) {
    case 1:
      starSky();
      break;

    case 2:
      spaceship();
      break;

    case 3:
      pulsar();
      break;

    case 4:
      starFall();
      break;

    case 5:
      colorfulSpiral();
      break;

    default:
      break;
  }
}

void showStar(int starNumber, uint32_t color) {
  strip.setPixelColor(starNumber, color);
  strip.show();
}

void hideStar(int starNumber) {
  strip.setPixelColor(starNumber, strip.Color(0, 0, 0));
  strip.show();
}

void starSky() {
  const uint32_t colorStar[] = {
      strip.Color(149, 167, 232), strip.Color(224, 232, 255),
      strip.Color(255, 159, 19),  strip.Color(252, 127, 20),
      strip.Color(242, 95, 106),  strip.Color(199, 0, 17),
      strip.Color(56, 112, 255)};

  static int starcycle = 0;

  if (starcycle < 60) {
    int starnumber = rand() % 256;
    int starcolor = rand() % 7;

    showStar(starnumber, strip.Color(255, 240, 245));
    delay(500);

    starnumber = rand() % 256;
    showStar(starnumber, colorStar[starcolor]);
    delay(1100);

    for (int i = 0; i < 6; i++) {
      starnumber = rand() % 256;
      hideStar(starnumber);
      delay(10);
    }

    starcycle++;
  } else {
    int starnumber = rand() % 256;
    showStar(starnumber, strip.Color(255, 0, 0));
    delay(1000);

    starnumber = rand() % 256;
    showStar(starnumber, strip.Color(0, 0, 255));
    starcycle = 0;
  }
}

uint16_t XY7(uint8_t x, uint8_t y) { return (y * mWidth + x); }

byte dir7 = 3;

static void MoveX(int8_t delta) {
  if (delta) {
    if (delta > 0) {
      for (uint8_t y = 0; y < mHeight; y++) {
        for (uint8_t x = 0; x < mWidth; x++) {
          leds[XY7(x, y)] = leds[XY7(x + delta, y)];
        }
      }
    } else {
      for (uint8_t y = 0; y < mHeight; y++) {
        for (uint8_t x = mWidth - 1; x > 0; x--) {
          leds[XY7(x, y)] = leds[XY7(x + delta, y)];
        }
      }
    }
  }
}

static void MoveY(int8_t delta) {
  if (delta) {
    if (delta > 0) {
      for (uint8_t x = 0; x < mWidth; x++) {
        for (uint8_t y = 0; y < mHeight; y++) {
          leds[XY7(x, y)] = leds[XY7(x, y + delta)];
        }
      }
    } else {
      for (uint8_t x = 0; x < mWidth; x++) {
        for (uint8_t y = mHeight - 1; y > 0; y--) {
          leds[XY7(x, y)] = leds[XY7(x, y + delta)];
        }
      }
    }
  }
}

static void drawPixel(byte x, byte y, CRGB color) {
  leds[XY7(x, y)] += color;
  if (mWidth > 24 || mHeight > 24) {
    leds[XY7(x + 1, y)] += color;
    leds[XY7(x - 1, y)] += color;
    leds[XY7(x, y + 1)] += color;
    leds[XY7(x, y - 1)] += color;
  }
}

void spaceship() {
  fadeToBlackBy(leds, NUM_LEDS, 16);
  switch (dir7) {
    case 0:
      MoveX(1);
      break;
    case 1:
      MoveX(1);
      MoveY(-1);
      break;
    case 2:
      MoveY(-1);
      break;
    case 3:
      MoveX(-1);
      MoveY(-1);
      break;
    case 4:
      MoveX(-1);
      break;
    case 5:
      MoveX(-1);
      MoveY(1);
      break;
    case 6:
      MoveY(1);
      break;
    case 7:
      MoveX(1);
      MoveY(1);
      break;
  }
  for (byte i = 0; i < 8; i++) {
    byte x = beatsin8(12 + i, 2, mWidth - 3);
    byte y = beatsin8(15 + i, 2, mHeight - 3);
    drawPixel(x, y,
              ColorFromPalette(RainbowColors_p, beatsin8(12 + i, 0, 255), 255));
  }
  XYMap xyMap(mWidth, mHeight);
  blur2d(leds, mWidth, mHeight, 32, xyMap);
  FastLED.show();
  EVERY_N_SECONDS(5) {
    if (dir7 == 7)
      dir7 = 0;
    else
      dir7++;
  }
}

static void drawVortex(int radius) {
  const int centerX = 6;
  const int centerY = 6;

  static int rotationAngle = 0;

  strip.clear();
  for (int angle = 0; angle < 360; angle += 10) {
    float rad = radians(angle + rotationAngle);
    int x = centerX + radius * cos(rad);
    int y = centerY + radius * sin(rad);

    if (x >= 0 && x < 12 && y >= 0 && y < 13) {
      strip.setPixelColor(XY(x, y), Wheel((angle + radius) & 255));
    }
  }
  strip.show();
  rotationAngle = (rotationAngle + 5) % 360;
}

void pulsar() {
  const int centerX = 6;
  const int centerY = 6;
  const int maxRadius = max(centerX, centerY);
  for (int radius = 0; radius <= maxRadius; radius++) {
    drawVortex(radius);
    delay(50);
  }

  for (int radius = maxRadius - 1; radius >= 0; radius--) {
    drawVortex(radius);
    delay(100);
  }
}

void starFall() {
  static int currentCol[8] = {11, 7, 3, 0, 5, 10, 6, 4};
  static int currentRow[8] = {11, 7, 3, 0, 5, 10, 6, 4};
  const int lineLength = 7;

  auto drawLine = [](int col, int row, int length, bool clear) {
    for (int j = 0; j < length; ++j) {
      int rowIndex = row - j;
      int colIndex = col - j;
      if (rowIndex >= 0 && rowIndex < mHeight && colIndex >= 0 &&
          colIndex < mWidth) {
        int pixelIndex = XY(colIndex, rowIndex);
        if (clear) {
          leds[pixelIndex] = CRGB::Black;
        } else {
          int brightness = (j == length - 1) ? 255 : 5 * j;
          leds[pixelIndex] = hsv2rgb_spectrum(CHSV(0, 0, brightness));
        }
      }
    }
  };

  for (int i = 0; i < 8; ++i) {
    drawLine(currentCol[i], currentRow[i], lineLength, false);
  }
  FastLED.show();
  delay(100);

  for (int i = 0; i < 8; ++i) {
    drawLine(currentCol[i], currentRow[i], lineLength, true);
  }
  FastLED.show();

  for (int i = 0; i < 8; ++i) {
    if (currentRow[i] > 0) {
      --currentRow[i];
      --currentCol[i];
    } else {
      currentCol[i] = mWidth + 4 + std::rand() % 8;
      currentRow[i] = mWidth + 6 + std::rand() % 8;
    }
  }
}

void colorfulSpiral() {
  static bool forward = true;
  static int spiralIndex = 0;

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  int x = 0, y = 0;
  int dx = 0, dy = -1;
  int maxI = max(mWidth, mHeight) * max(mWidth, mHeight);

  for (int i = 0; i < maxI; i++) {
    if (x >= -mWidth / 2 && x < mWidth / 2 && y >= -mHeight / 2 &&
        y < mHeight / 2) {
      int ledIndex = XY(x + mWidth / 2, y + mHeight / 2);
      if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
        leds[ledIndex] = CHSV((i + spiralIndex) % 256, 255, 255);
      }
    }

    if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1 - y)) {
      int temp = dx;
      dx = -dy;
      dy = temp;
    }

    x += dx;
    y += dy;
  }
  FastLED.show();

  spiralIndex = (spiralIndex + (forward ? 1 : -1)) % 256;

  if (spiralIndex == 0 || spiralIndex == 255) {
    forward = !forward;
  }

  FastLED.delay(130);
}