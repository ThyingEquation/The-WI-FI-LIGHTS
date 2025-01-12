#include "flashLights.h"

static void flashLights1();
static void flashLights2();
static void flashLights3();

void flashLights(uint8_t subMode) {
  switch (subMode) {
    case 1:
      flashLights1();
      break;

    case 2:
      flashLights2();
      break;

    case 3:
      flashLights3();
      break;

    default:
      break;
  }
}

void flashLights2() {
  static bool loadingFlag = true;

  static byte FF[mWidth][mHeight];
  static byte SF[mWidth][mHeight];

  if (loadingFlag) {
    memset8(SF, 0, NUM_LEDS);
    memset8(FF, 0, NUM_LEDS);
    loadingFlag = 0;
  }
  for (byte i = 0; i < map(128, 1, 255, 2, 16); i++) {
    uint8_t x = rand() % mWidth;
    uint8_t y = rand() % mHeight;
    if (!SF[x][y]) {
      SF[x][y] = 255;
      FF[x][y] = rand() % 255;
    }
  }
  for (byte x = 0; x < mWidth; x++) {
    for (byte y = 0; y < mHeight; y++) {
      if (SF[x][y] <= 30)
        SF[x][y] = 0;
      else
        SF[x][y] = ((SF[x][y] - map(128, 1, 255, 1, 16)) <= 0)
                       ? 0
                       : (SF[x][y] - map(128, 1, 255, 1, 16));
      leds[XY(x, y)] = CHSV(FF[x][y], 255, SF[x][y]);
    }
  }
  delay(20);
  FastLED.show();
}

static uint16_t getIndex(uint16_t x, uint16_t y) {
  uint16_t index;
  if (y == 0) {
    index = x;
  } else if (y % 2 == 0) {
    index = y * mWidth + x;
  } else {
    index = ((y * mWidth) + (mWidth - 1)) - x;
  }
  return index;
}

void flashLights3() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(HUE_PURPLE, 255, 255);
  FastLED.show();
  FastLED.delay(1000 / 60);
}

void flashLights1() {
  EVERY_N_MILLIS(75) {
    for (int8_t row = mHeight - 1; row >= 0; row--) {
      for (int8_t col = 0; col < mWidth; col++) {
        if (leds[getIndex(col, row)] == CRGB(175, 255, 175)) {
          leds[getIndex(col, row)] = CRGB(27, 130, 39);
          if (row < mHeight - 1)
            leds[getIndex(col, row + 1)] = CRGB(175, 255, 175);
        }
      }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      if (leds[i].g != 255) leds[i].nscale8(192);
    }
    bool emptyScreen = true;
    for (int i = 0; i < NUM_LEDS; i++) {
      if (leds[i]) {
        emptyScreen = false;
        break;
      }
    }

    if (random8(3) == 0 || emptyScreen) {
      int8_t spawnX = random8(mWidth);
      leds[getIndex(spawnX, 0)] = CRGB(175, 255, 175);
    }

    FastLED.show();
  }
}