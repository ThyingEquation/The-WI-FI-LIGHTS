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
  LIGHT_NOISE_DELAY = 20,
  RAINBOW_WAVE_DELAY = 10,
  RAINBOW_SNAKE_DELAY = 30
};

static CRGBPalette16 currentPalette(CloudColors_p);

static uint16_t speed = 1;
static uint16_t scale = 25;
static uint8_t colorLoop = 1;

static uint8_t noise[16][16];

static void changePaletteAndSettingsPeriodically(bool resetVal);
static void drawColorfulSpots();
static void drawLightNoise();
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
    drawLightNoise();
    break;

  case 2:
    drawDiagonalWaves();
    break;

  case 3:
    drawRainbowWheel();
    break;

  case 4:
    drawRainbowRipples();
    break;

  case 5:
    drawRainbowWave();
    break;

  case 6:
    drawRainbowSnake();
    break;

  default:
    break;
  }
}

CRGB customWheel(byte WheelPos)
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

static void drawColorfulSpots()
{
  if (checkCommandReceived())
  {
    speed = 1;
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
  stripShow();
}

static void drawLightNoise()
{
  static uint16_t lightersPosX6[32];
  static uint16_t lightersPosY6[32];
  static uint16_t lightersSpeedX6[32];
  static uint16_t lightersSpeedY6[32];
  static byte lightersSpeedZ[32];
  static byte lcolor6[32];
  static byte mass6[32];
  static bool loadingFlag6 = true;
  static uint32_t lastTime = 0;

  if (millis() - lastTime < LIGHT_NOISE_DELAY) return;
  lastTime = millis();

  XYMap xyMap(MATRIX_WIDTH, MATRIX_HEIGHT);
  if (loadingFlag6)
  {
    loadingFlag6 = false;
    randomSeed(millis());
    for (byte i = 0; i < 32; i++)
    {
      lightersSpeedX6[i] = -10 + ESP8266TrueRandom.random(0, 21);
      lightersSpeedY6[i] = -10 + ESP8266TrueRandom.random(0, 21);
      mass6[i] = 5 + ESP8266TrueRandom.random(0, 6);
      lightersSpeedZ[i] = 3 + ESP8266TrueRandom.random(0, 23);
      lightersPosX6[i] = ESP8266TrueRandom.random(0, MATRIX_WIDTH * 10);
      lightersPosY6[i] = ESP8266TrueRandom.random(0, MATRIX_HEIGHT * 10);
      lcolor6[i] = ESP8266TrueRandom.random(0, 9) * 28;
    }
  }

  switch (2)
  {
  case 0:
    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    stripShow();
    break;
  case 1:
    fadeToBlackBy(leds, MATRIX_LEDS, 50);
    break;
  case 2:
    blur2d(leds, MATRIX_WIDTH, MATRIX_HEIGHT, 30, xyMap);
    fadeToBlackBy(leds, MATRIX_LEDS, 25);
    break;
  case 3:
    fadeToBlackBy(leds, MATRIX_LEDS, 200);
    break;
  }

  for (byte i = 0; i < 32; i++)
  {
    lcolor6[i]++;
    switch (3)
    {
    case 0:
      lightersPosX6[i] +=
          beatsin88(lightersSpeedX6[0] * 255, 0,
                    mass6[i] / 10 * ((MATRIX_HEIGHT + MATRIX_WIDTH) / 8)) -
          mass6[i] / 10 * ((MATRIX_HEIGHT + MATRIX_WIDTH) / 16);
      lightersPosY6[i] +=
          beatsin88(lightersSpeedY6[0] * 255, 0,
                    mass6[i] / 10 * ((MATRIX_HEIGHT + MATRIX_WIDTH) / 8)) -
          mass6[i] / 10 * ((MATRIX_HEIGHT + MATRIX_WIDTH) / 16);
      break;
    case 1:
      lightersPosX6[i] = beatsin16(
          lightersSpeedX6[i] / map(255, 1, 255, 10, 1), 0, (MATRIX_WIDTH - 1) * 10);
      lightersPosY6[i] =
          beatsin16(lightersSpeedY6[i] / map(255, 1, 255, 10, 1), 0,
                    (MATRIX_HEIGHT - 1) * 10);
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

    if (lightersPosY6[i] < 0)
    {
      lightersPosY6[i] = 1;
      lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
    }
    if (lightersPosX6[i] < 0)
    {
      lightersPosX6[i] = 1;
      lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
    }
    if (lightersPosY6[i] >= (MATRIX_HEIGHT - 1) * 10)
    {
      lightersPosY6[i] = ((MATRIX_HEIGHT - 1) * 10) - 1;
      lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
    }
    if (lightersPosX6[i] >= (MATRIX_WIDTH - 1) * 10)
    {
      lightersPosX6[i] = ((MATRIX_WIDTH - 1) * 10) - 1;
      lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
    }

    CRGB color =
        CHSV(lcolor6[i], 255,
             beatsin8(lightersSpeedZ[i] / map(255, 1, 255, 10, 1), 128, 255));
    drawPixelXYFB3((float)lightersPosX6[i] / 10, (float)lightersPosY6[i] / 10,
                   color);
  }

  EVERY_N_SECONDS(10)
  {
    randomSeed(millis());
    for (byte i = 0; i < 32; i++)
    {
      lightersSpeedX6[i] = -10 + ESP8266TrueRandom.random(0, 21);
      lightersSpeedY6[i] = ESP8266TrueRandom.random(0, 360);
      mass6[i] = 5 + ESP8266TrueRandom.random(0, 6);
      lightersSpeedZ[i] = 3 + ESP8266TrueRandom.random(0, 23);
    }
  }
  stripShow();
}

static void drawDiagonalWaves()
{
  static uint8_t hue = 0;
  static uint32_t lastTime = 0;

  if (millis() - lastTime < 25) return;
  lastTime = millis();

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
  stripShow();
}

void drawRainbowWheel()
{
  static uint16_t rotation = 0;
  static uint32_t lastMillis = 0;

  uint32_t ms = millis();
  uint32_t delta = ms - lastMillis;
  lastMillis = ms;
  rotation += delta * 10;

  uint8_t centerX = MATRIX_WIDTH / 2;
  uint8_t centerY = MATRIX_HEIGHT / 2;

  for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
  {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
    {
      float dx = (float)x - centerX + 0.5f;
      float dy = (float)y - centerY + 0.5f;
      uint8_t angle = (uint8_t)(atan2(dy, dx) * 128.0f / PI + 128);

      float dist = sqrt(dx * dx + dy * dy);
      uint8_t bri = (uint8_t)constrain(dist * 30, 30, 255);

      uint8_t hue = angle + (rotation >> 8);

      leds[XY(x, y)] = CHSV(hue, 255, bri);
    }
  }
  stripShow();
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
  stripShow();
}

void drawRainbowWave()
{
  static uint16_t waveRainbow = 0;
  static uint32_t lastTime = 0;

  if (millis() - lastTime < RAINBOW_WAVE_DELAY) return;
  lastTime = millis();

  if (waveRainbow < 256)
  {
    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    {
      leds[i] = customWheel((i + waveRainbow) & 255);
    }
    stripShow();
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
  static int16_t position = 0; 
  static int8_t direction = 1;
  static uint8_t hue = 0;
  static uint32_t lastTime = 0;

  if (checkCommandReceived())
  {
    position = 0;
    direction = 1;
    hue = 0;
    lastTime = 0;
  }

  if (millis() - lastTime < RAINBOW_SNAKE_DELAY) return;
  lastTime = millis();
  leds[position] = CHSV(hue++, 255, 255);

  fadeall();
  stripShow();

  position += direction;

  if (position >= MATRIX_LEDS)
  {
    position = MATRIX_LEDS - 2;
    direction = -1;
  }
  else if (position < 0)
  {
    position = 1; 
    direction = 1;
  }
}