#include "effects.h"

/*
  Эта группа эффектов только для матриц 12х12 и 16х16

  Настраиваемые параметры цветных пятен:
    1) Скорость, переменная speed
    2) Масштаб, переменная scale

    Настраиваемые параметры радуги (rainbowsSettings):
    1) Скорости эффектов
*/

enum rainbowsSettings
{
  RAINBOW_WAVE_2_DELAY = 10,
  RAINBOW_SNAKE_DELAY = 15
};

static CRGBPalette16 currentPalette(CloudColors_p);

static uint16_t speed = 10;
static uint16_t scale = 25;
static uint8_t colorLoop = 1;

static uint8_t noise[16][16];

static void fillNoise8(bool resetVal);
static void mapNoiseToLedsUsingPalette(bool resetVal);
static void changePaletteAndSettingsPeriodically(bool resetVal);

static void drawColorfulSpots();
static void drawDiagonalWaves();
static void drawRainbowWheel();
static void drawRainbowRipples();
static void drawRainbowWave();
static void drawRainbowSnake();

void drawColorfulEffects(uint8_t subMode)
{
  switch (subMode)
  {
  case 0:
    drawColorfulSpots();
    break;

  case 1:
    drawDiagonalWaves();
    break;

  case 2:
    drawRainbowWheel();
    break;

  case 3:
    drawRainbowRipples();
    break;

  case 4:
    drawRainbowWave();
    break;

  case 5:
    drawRainbowSnake();
    break;

  default:
    break;
  }
}

static void drawColorfulSpots()
{
  if (checkCommandReceived())
  {
    speed = 10;
    scale = 25;
    colorLoop = 1;

    changePaletteAndSettingsPeriodically(true);
    fillNoise8(true);
    mapNoiseToLedsUsingPalette(true);
  }
  else
  {
    changePaletteAndSettingsPeriodically(false);
    fillNoise8(false);
    mapNoiseToLedsUsingPalette(false);
  }

  FastLED.show();
}

static void fillNoise8(bool resetVal)
{
  static uint16_t X = random16();
  static uint16_t Y = random16();
  static uint16_t Z = random16();

  if (resetVal)
  {
    X = random16();
    Y = random16();
    Z = random16();
  }

  uint8_t dataSmoothing = 0;
  if (speed < 50)
  {
    dataSmoothing = 400 - (speed * 4);
  }

  for (uint8_t i = 0; i < MATRIX_HEIGHT; i++)
  {
    uint16_t ioffset = scale * i;
    for (uint8_t j = 0; j < MATRIX_HEIGHT; j++)
    {
      uint16_t joffset = scale * j;

      uint8_t data = inoise8(X + ioffset, Y + joffset, Z);
      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing)
      {
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

static void mapNoiseToLedsUsingPalette(bool resetVal)
{
  static uint8_t ihue = 0;

  if (resetVal)
  {
    ihue = 0;
  }

  for (uint8_t i = 0; i < MATRIX_WIDTH; i++)
  {
    for (uint8_t j = 0; j < MATRIX_HEIGHT; j++)
    {
      uint8_t index = noise[j][i];
      uint8_t bri = noise[i][j];

      if (colorLoop)
      {
        index += ihue;
      }

      if (bri > 127)
      {
        bri = 255;
      }
      else
      {
        bri = dim8_raw(bri * 2);
      }

      CRGB color = ColorFromPalette(currentPalette, index, bri);
      leds[XY(i, j)] = color;
    }
  }

  ihue += 1;
}

static void SetupRandomPalette()
{
  currentPalette =
      CRGBPalette16(CHSV(random8(), 255, 32), CHSV(random8(), 255, 255),
                    CHSV(random8(), 128, 255), CHSV(random8(), 255, 255));
}

static void changePaletteAndSettingsPeriodically(bool resetVal)
{
  uint8_t secondHand = ((millis() / 1000) / 10) % 60;
  static uint8_t lastSecond = 99;

  if (resetVal)
  {
    lastSecond = 99;
  }

  if (lastSecond != secondHand)
  {
    lastSecond = secondHand;
    if (secondHand == 0)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 5)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 10)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 15)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 20)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 25)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 0;
    }
    if (secondHand == 30)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 35)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 40)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 45)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 50)
    {
      SetupRandomPalette();
      speed = 1;
      scale = 30;
      colorLoop = 1;
    }
  }
}

static void drawDiagonalWaves()
{
  static uint8_t hue = 0;

  for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
  {
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
    {
      uint16_t index = XY(x, y);
      uint8_t brightness = sin8(x * 8 + y * 8 + hue);
      leds[index] = CHSV(hue, 255, brightness);
    }
  }
  hue++;
  FastLED.show();
  FastLED.delay(10);
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

void drawRainbowRipples()
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

void drawRainbowWave()
{
  static uint16_t waveRainbow = 0;
  static uint32_t lastTime = 0;

  if (millis() - lastTime < RAINBOW_WAVE_2_DELAY)
  {
    return;
  }
  lastTime = millis();

  if (waveRainbow < 256)
  {
    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    {
      leds[i] = Wheel((i + waveRainbow) & 255);
    }
    FastLED.show();
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
  static uint32_t lastTime = 0;

  if (checkCommandReceived())
  {
    waveSnake1 = 0;
    waveSnake2 = 0;
    hue = 0;
    lastTime = 0;
  }

  if (millis() - lastTime < RAINBOW_SNAKE_DELAY)
  {
    return;
  }
  lastTime = millis();

  if (waveSnake1 < MATRIX_LEDS)
  {
    leds[waveSnake1] = CHSV(hue++, 255, 255);

    fadeall();

    FastLED.show();
    ++waveSnake1;
    waveSnake2 = MATRIX_LEDS - 1;
  }
  else if (waveSnake2 > 0)
  {
    leds[waveSnake2] = CHSV(hue++, 255, 255);
    fadeall();
    FastLED.show();
    --waveSnake2;
  }
  else
  {
    FastLED.clear();
    FastLED.show();

    waveSnake1 = 0;
    waveSnake2 = MATRIX_LEDS - 1;
  }
}

CRGB Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
}