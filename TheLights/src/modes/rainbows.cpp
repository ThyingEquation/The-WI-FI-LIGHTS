#include "modes.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (rainbowsSettings):
    1) Скорости эффектов
*/

enum rainbowsSettings
{
  RAINBOW_WAVE_2_DELAY = 10,
  RAINBOW_SNAKE_DELAY = 15
};

static void drawRainbowWheel();
static void drawRainbowWave1();
static void drawRainbowWave2();
static void drawRainbowSnake();

void drawRainbows(uint8_t subMode)
{
  static void (*rainbowFunctions[])() = {nullptr, drawRainbowWheel, drawRainbowWave1,
                                         drawRainbowWave2, drawRainbowSnake};

  if (subMode >= 1 && subMode <= 4)
  {
    rainbowFunctions[subMode]();
  }
}

static void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for (byte Y = 0; Y < MATRIX_HEIGHT; Y++)
  {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for (byte X = 0; X < MATRIX_WIDTH; X++)
    {
      pixelHue += xHueDelta8;
      leds[XY(X, Y)] = CHSV(pixelHue, 255, 255);
    }
  }
}

void drawRainbowWheel()
{
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / MATRIX_WIDTH));
  int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / MATRIX_HEIGHT));
  DrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  if (ms < 5000)
  {
    FastLED.setBrightness(scale8(50, (ms * 256) / 5000));
  }
  else
  {
    FastLED.setBrightness(50);
  }
  FastLED.show();
}

void drawRainbowWave1()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint16_t brightnessThetaInc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msMultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltaMs = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltaMs * msMultiplier;
  sHue16 += deltaMs * beatsin88(400, 5, 9);
  uint16_t brightnessTheta16 = sPseudotime;

  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnessTheta16 += brightnessThetaInc16;
    uint16_t b16 = sin16(brightnessTheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * 128) / 65536;
    bri8 += (255 - 128);

    CRGB newColor = CHSV(hue8, 255, bri8);

    uint16_t pixelnumber = i;

    pixelnumber = (MATRIX_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newColor, 64);
  }
  FastLED.show();
}

void drawRainbowWave2()
{
  static uint16_t waveRainbow = 0;
  if (waveRainbow < 256)
  {
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel((i + waveRainbow) & 255));
    }
    strip.show();
    delay(RAINBOW_WAVE_2_DELAY);
    ++waveRainbow;
  }
  else
  {
    waveRainbow = 0;
  }
}

static void fadeall()
{
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    leds[i].nscale8(250);
  }
}

void drawRainbowSnake()
{
  static uint16_t waveSnake1 = 0;
  static uint16_t waveSnake2 = 0;
  static uint8_t hue = 0;

  if (waveSnake1 < MATRIX_LEDS)
  {
    leds[waveSnake1] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    delay(RAINBOW_SNAKE_DELAY );
    ++waveSnake1;
    waveSnake2 = MATRIX_LEDS - 1;
  }

  else if (waveSnake2 > 0)
  {
    leds[waveSnake2] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    delay(RAINBOW_SNAKE_DELAY );
    --waveSnake2;
  }
  else
  {
    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
    waveSnake1 = 0;
    waveSnake2 = (MATRIX_LEDS)-1;
  }
}

uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}