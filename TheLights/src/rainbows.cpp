#include <rainbows.h>

// extern uint16_t waveRainbow2j;
uint16_t waveRainbow2j = 0;

void rainbows() {
  switch (choosenModeD2) {
    case 1:
      rainbow1();
      break;

    case 2:
      rainbow2();
      break;

    case 3:
      rainbow();
      break;

    case 4:
      rainbow3();
      break;

    default:
      break;
  }
}

void rainbow() {
  if (waveRainbow2j < 256) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + waveRainbow2j) & 255));
    }
    strip.show();
    delay(10);
    ++waveRainbow2j;
  } else {
    waveRainbow2j = 0;
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}

void rainbow1() {
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
  int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
  DrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  if (ms < 5000) {
    FastLED.setBrightness(scale8(BRIGHTNESS, (ms * 256) / 5000));
  } else {
    FastLED.setBrightness(BRIGHTNESS);
  }
  FastLED.show();
}

void rainbow2() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t brightdepth = 128;
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
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, 255, bri8);

    uint16_t pixelnumber = i;

    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
  FastLED.show();
}

void rainbow3() {
  static uint8_t hue = 0;

  if (waveSnakeJ < NUM_LEDS) {
    leds[waveSnakeJ] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    delay(15);
    ++waveSnakeJ;
    waveSnakeK = NUM_LEDS - 1;
  }

  else if (waveSnakeK > 0) {
    leds[waveSnakeK] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    delay(15);
    --waveSnakeK;
  } else {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
    waveSnakeJ = 0;
    waveSnakeK = (NUM_LEDS)-1;
  }
}
