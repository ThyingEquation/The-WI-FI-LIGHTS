#include <space.h>

const uint32_t colorStar[] = {
    strip.Color(149, 167, 232), strip.Color(224, 232, 255),
    strip.Color(255, 159, 19),  strip.Color(252, 127, 20),
    strip.Color(242, 95, 106),  strip.Color(199, 0, 17),
    strip.Color(56, 112, 255)};

unsigned long starnumber = 0;
unsigned long starcolor = 0;
int starcycle = 0;

void showStar(int starNumber, uint32_t color) {
  strip.setPixelColor(starNumber, color);
  strip.show();
}

void hideStar(int starNumber) {
  strip.setPixelColor(starNumber, strip.Color(0, 0, 0));
  strip.show();
}

const int centerX = 6;
const int centerY = 6;
const int maxRadius = max(centerX, centerY);

int rotationAngle = 0;

static int startColS[8] = {11, 7, 3, 0, 5, 10, 6, 4};
static int startRowS[8] = {0, 3, 5, 3, 1, 4, 0, 1};
static int currentRowS[8] = {11, 7, 3, 0, 5, 10, 6, 4};
static int lineLengthS = 7;

const int ledMatrix2[30][25] = {
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     143, 142, 117, 116, 91,  90,  65,  64,  39,  38,  13,  12},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     144, 141, 118, 115, 92,  89,  66,  63,  40,  37,  14,  11},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     145, 140, 119, 114, 93,  88,  67,  62,  41,  36,  15,  10},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     146, 139, 120, 113, 94,  87,  68,  61,  42,  35,  16,  9},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     147, 138, 121, 112, 95,  86,  69,  60,  43,  34,  17,  8},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     148, 137, 122, 111, 96,  85,  70,  59,  44,  33,  18,  7},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     149, 136, 123, 110, 97,  84,  71,  58,  45,  32,  19,  6},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     150, 135, 124, 109, 98,  83,  72,  57,  46,  31,  20,  5},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     151, 134, 125, 108, 99,  82,  73,  56,  47,  30,  21,  4},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     152, 133, 126, 107, 100, 81,  74,  55,  48,  29,  22,  3},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     153, 132, 127, 106, 101, 80,  75,  54,  49,  28,  23,  2},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     154, 131, 128, 105, 102, 79,  76,  53,  50,  27,  24,  1},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     155, 130, 129, 104, 103, 78,  77,  52,  51,  26,  25,  0},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     57,  157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157},
    {157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157,
     157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157}};

// uint8_t brightnessS;

// int rowIndexS = 0;
// int colIndexS = 0;
// int pixelIndexS = 0;
// uint32_t colorS = 0;

void space() {
  switch (choosenModeD2) {
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

    default:
      break;
  }
}

void starSky() {
  if (starcycle < 60) {
    int starnumber = random(256);
    int starcolor = random(7);

    showStar(starnumber, strip.Color(255, 240, 245));
    delay(500);

    starnumber = random(256);
    showStar(starnumber, colorStar[starcolor]);
    delay(1100);

    for (int i = 0; i < 6; i++) {
      starnumber = random(256);
      hideStar(starnumber);
      delay(10);
    }

    starcycle++;
  } else {
    int starnumber = random(256);
    showStar(starnumber, strip.Color(255, 0, 0));
    delay(1000);

    starnumber = random(256);
    showStar(starnumber, strip.Color(0, 0, 255));
    starcycle = 0;
  }
}

uint16_t XY7(uint8_t x, uint8_t y) { return (y * mWidth + x); }

byte dir7 = 3;
void drawPixel(byte x, byte y, CRGB color) {
  leds[XY7(x, y)] += color;
  if (mWidth > 24 || mHeight > 24) {
    leds[XY7(x + 1, y)] += color;
    leds[XY7(x - 1, y)] += color;
    leds[XY7(x, y + 1)] += color;
    leds[XY7(x, y - 1)] += color;
  }
}

void MoveX(int8_t delta) {
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

void MoveY(int8_t delta) {
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
  blur2d(leds, mWidth, mHeight, 32, xyMap);
  FastLED.show();
  EVERY_N_SECONDS(5) {
    if (dir7 == 7)
      dir7 = 0;
    else
      dir7++;
  }
}

void pulsar() {
  for (int radius = 0; radius <= maxRadius; radius++) {
    drawVortex(radius);
    delay(50);
  }

  for (int radius = maxRadius - 1; radius >= 0; radius--) {
    drawVortex(radius);
    delay(100);
  }
}

void drawVortex(int radius) {
  strip.clear();
  for (int angle = 0; angle < 360; angle += 10) {
    float rad = radians(angle + rotationAngle);
    int x = centerX + radius * cos(rad);
    int y = centerY + radius * sin(rad);

    if (x >= 0 && x < 12 && y >= 0 && y < 13) {
      int pixelIndex = xyToIndex(x, y);
      strip.setPixelColor(mainScheme[pixelIndex],
                          Wheel((angle + radius) & 255));
    }
  }
  strip.show();
  rotationAngle = (rotationAngle + 5) % 360;
}

int xyToIndex(int x, int y) { return y * 12 + x; }

uint32_t WheelS(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void starFall() {
  for (int i = 0; i < 8; i++) {
    if (currentRowS[i] < 22) {
      for (int j = 0; j < lineLengthS; j++) {
        int rowIndexS = currentRowS[i] + j;
        int colIndexS = startColS[i] + j;
        if (rowIndexS < 30 && colIndexS < 25) {
          int pixelIndexS = ledMatrix2[rowIndexS][colIndexS];
          int brightnessS;
          if (j == lineLengthS - 1) {
            brightnessS = 255;
          } else {
            brightnessS = 5 * j;
          }
          uint32_t colorS = HSVtoRGB(0, 0, brightnessS);
          if (pixelIndexS <= 155) {
            strip.setPixelColor(pixelIndexS, colorS);
          }
        }
      }
    }
  }

  strip.show();
  delay(110);

  for (int i = 0; i < 8; i++) {
    if (currentRowS[i] < 22) {
      for (int j = 0; j < lineLengthS; j++) {
        int rowIndexS = currentRowS[i] + j;
        int colIndexS = startColS[i] + j;
        if (rowIndexS < 30 && colIndexS < 25) {
          int pixelIndexS = ledMatrix2[rowIndexS][colIndexS];
          if (pixelIndexS <= 155) {
            strip.setPixelColor(pixelIndexS, strip.Color(0, 0, 0));
          }
        }
      }
    }
  }

  strip.show();

  for (int i = 0; i < 8; i++) {
    if (currentRowS[i] < 22) {
      currentRowS[i]++;
      startColS[i]++;
    } else {
      startColS[i] = random(12);
      startRowS[i] = random(6);
      currentRowS[i] = startRowS[i];
    }
  }
}