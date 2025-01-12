#include "rainbows.h"

static void rainbowWheel();
static void rainbowWave1();
static void rainbowWave2();
static void rainbowSnake();

void rainbows(uint8_t subMode) {
  static void (*rainbowFunctions[])() = {nullptr, rainbowWheel, rainbowWave1,
                                         rainbowWave2, rainbowSnake};

  if (subMode >= 1 && subMode <= 4) {
    rainbowFunctions[subMode]();
  }
}

static void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
  byte lineStartHue = startHue8;
  for (byte Y = 0; Y < mHeight; Y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for (byte X = 0; X < mWidth; X++) {
      pixelHue += xHueDelta8;
      leds[XY(X, Y)] = CHSV(pixelHue, 255, 255);
    }
  }
}

void rainbowWheel() {
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / mWidth));
  int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / mHeight));
  DrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  if (ms < 5000) {
    FastLED.setBrightness(scale8(50, (ms * 256) / 5000));
  } else {
    FastLED.setBrightness(50);
  }
  FastLED.show();
}

void rainbowWave1() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * 128) / 65536;
    bri8 += (255 - 128);

    CRGB newcolor = CHSV(hue8, 255, bri8);

    uint16_t pixelnumber = i;

    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
  FastLED.show();
}

void rainbowWave2() {
  static uint16_t waveRainbow = 0;
  if (waveRainbow < 256) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + waveRainbow) & 255));
    }
    strip.show();
    delay(10);
    ++waveRainbow;
  } else {
    waveRainbow = 0;
  }
}

static void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}

void rainbowSnake() {
  static uint16_t waveSnake1 = 0;
  static uint16_t waveSnake2 = 0;
  static uint8_t hue = 0;

  if (waveSnake1 < NUM_LEDS) {
    leds[waveSnake1] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    delay(15);
    ++waveSnake1;
    waveSnake2 = NUM_LEDS - 1;
  }

  else if (waveSnake2 > 0) {
    leds[waveSnake2] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    delay(15);
    --waveSnake2;
  } else {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
    waveSnake1 = 0;
    waveSnake2 = (NUM_LEDS)-1;
  }
}