#include "colorfulSpots.h"

static void fillnoise8();
static void mapNoiseToLEDsUsingPalette();
static void ChangePaletteAndSettingsPeriodically();

static CRGBPalette16 currentPalette(CloudColors_p);

static uint16_t speed = 10;
static uint16_t scale = 25;
static uint8_t colorLoop = 1;

static uint8_t noise[16][16];

void colorfulSpots(uint8_t sumMode) {
  ChangePaletteAndSettingsPeriodically();
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  LEDS.show();
}

void fillnoise8() {
  static uint16_t X = random16();
  static uint16_t Y = random16();
  static uint16_t Z = random16();

  uint8_t dataSmoothing = 0;
  if (speed < 50) {
    dataSmoothing = 400 - (speed * 4);
  }

  for (int i = 0; i < mHeight; i++) {
    int ioffset = scale * i;
    for (int j = 0; j < mHeight; j++) {
      int joffset = scale * j;

      uint8_t data = inoise8(X + ioffset, Y + joffset, Z);
      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata =
            scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
        data = newdata;
      }

      noise[i][j] = data;
    }
  }

  Z += speed;
  X += speed / 8;
  Y -= speed / 16;
}

void mapNoiseToLEDsUsingPalette() {
  static uint8_t ihue = 0;

  for (int i = 0; i < mWidth; i++) {
    for (int j = 0; j < mHeight; j++) {
      uint8_t index = noise[j][i];
      uint8_t bri = noise[i][j];

      if (colorLoop) {
        index += ihue;
      }

      if (bri > 127) {
        bri = 255;
      } else {
        bri = dim8_raw(bri * 2);
      }

      CRGB color = ColorFromPalette(currentPalette, index, bri);
      leds[XY(i, j)] = color;
    }
  }

  ihue += 1;
}

static void SetupRandomPalette() {
  currentPalette =
      CRGBPalette16(CHSV(random8(), 255, 32), CHSV(random8(), 255, 255),
                    CHSV(random8(), 128, 255), CHSV(random8(), 255, 255));
}

void ChangePaletteAndSettingsPeriodically() {
  uint8_t secondHand = ((millis() / 1000) / 10) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand) {
    lastSecond = secondHand;
    if (secondHand == 0) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 5) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 10) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 15) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 20) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 25) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 0;
    }
    if (secondHand == 30) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 35) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 40) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 45) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 50) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
  }
}