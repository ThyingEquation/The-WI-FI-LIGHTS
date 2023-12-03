#include <mode3.h>

#define MAX_DIMENSION ((kMatrixWidth > kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define HOLD_PALETTES_X_TIMES_AS_LONG 10

// режим цветных пятен с возможностью настройки

uint16_t speed = 10;  
uint16_t scale = 25;

uint8_t colorLoop = 1; 


void mode3() {

  ChangePaletteAndSettingsPeriodically();

   fillnoise8();
  
  mapNoiseToLEDsUsingPalette();

  LEDS.show();
}

////////////////////////////////////////////////// вызываемая функц.


void fillnoise8() {
  uint8_t dataSmoothing = 0;
  if (speed < 50) {
    dataSmoothing = 400 - (speed * 4);
  }

  for (int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for (int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;

      uint8_t data = inoise8(X + ioffset, Y + joffset, Z);
      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
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

  for (int i = 0; i < kMatrixWidth; i++) {
    for (int j = 0; j < kMatrixHeight; j++) {

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

void ChangePaletteAndSettingsPeriodically() {
  uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand) {
    lastSecond = secondHand;
    //if( secondHand ==  0)  { currentPalette = CloudColors_p;           speed =  1; scale = 30; colorLoop = 1; }
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
    //    if( secondHand == 10)  { currentPalette = ForestColors_p;          speed =  1; scale = 30; colorLoop = 0; }
    if (secondHand == 10) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    //    if( secondHand == 15)  { currentPalette = CloudColors_p;           speed =  1; scale = 30; colorLoop = 0; }
    if (secondHand == 15) {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    //    if( secondHand == 20)  { currentPalette = OceanColors_p;           speed =  1; scale = 30; colorLoop = 0; }
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
    //   if( secondHand == 50)  { currentPalette = RainbowStripeColors_p;   speed =  1; scale = 30; colorLoop = 1; }
  }
}

void SetupRandomPalette() {
  currentPalette = CRGBPalette16(
    CHSV(random8(), 255, 32),
    CHSV(random8(), 255, 255),
    CHSV(random8(), 128, 255),
    CHSV(random8(), 255, 255));
}

void SetupBlackAndWhiteStripedPalette() {
  fill_solid(currentPalette, 16, CRGB::Black);
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}
void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
  byte lineStartHue = startHue8;
  for (byte Y = 0; Y < kMatrixHeight; Y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for (byte X = 0; X < kMatrixWidth; X++) {
      pixelHue += xHueDelta8;
      leds[XY(X, Y)] = CHSV(pixelHue, 255, 255);
    }
  }
}