#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (flickeringLightsSettings):
    1) Скорости эффектов
*/

enum flickeringLightsSettings
{
  FLYING_LIGHTS_DELAY = 75,
  CONFETTI_DELAY = 20,
  FLASH_LIGHT_DELAY = 17
};

static void drawFlyingLights();
static void drawConfetti();
static void drawLocalFlickeringLights();

void drawFlickeringLights(uint8_t subMode)
{
  switch (subMode)
  {
  case 0:
    drawFlyingLights();
    break;

  case 1:
    drawConfetti();
    break;

  case 2:
    drawLocalFlickeringLights();
    break;

  default:
    break;
  }
}

static void drawFlyingLights()
{
  static const CRGB HEAD_COLOR = CRGB(175, 255, 175);
  static const CRGB TAIL_COLOR = CRGB(27, 130, 39);

  bool emptyScreen = true;

  EVERY_N_MILLIS(FLYING_LIGHTS_DELAY)
  {
    for (int8_t row = MATRIX_HEIGHT - 1; row >= 0; row--)
    {
      for (int8_t col = 0; col < MATRIX_WIDTH; col++)
      {
        if (leds[XY(col, row)] == HEAD_COLOR)
        {
          leds[XY(col, row)] = TAIL_COLOR;
          if (row < MATRIX_HEIGHT - 1)
          {
            int8_t drift = random8(3) - 1;
            int8_t nextCol = col + drift;

            if (nextCol < 0)
              nextCol = 0;
            if (nextCol >= MATRIX_WIDTH)
              nextCol = MATRIX_WIDTH - 1;

            leds[XY(nextCol, row + 1)] = HEAD_COLOR;
          }
        }
      }
    }

    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    {
      if (leds[i].g != 255)
        leds[i].nscale8(180);
      if (leds[i])
        emptyScreen = false;
    }

    if (random8(5) == 0 || emptyScreen)
    {
      int8_t spawnX = random8(MATRIX_WIDTH);
      leds[XY(spawnX, 0)] = CRGB(175, 255, 175);
    }
    stripShow();
  }
}

static void drawConfetti()
{
  static bool loadingFlag = true;

  static byte FF[MATRIX_WIDTH][MATRIX_HEIGHT];
  static byte SF[MATRIX_WIDTH][MATRIX_HEIGHT];

  static uint32_t lastTime = 0;

  if (millis() - lastTime < CONFETTI_DELAY)
    return;
  lastTime = millis();

  if (loadingFlag)
  {
    memset8(SF, 0, sizeof(SF));
    memset8(FF, 0, sizeof(FF));
    loadingFlag = false;
  }

  for (byte i = 0; i < 8; i++)
  {
    uint8_t x = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
    uint8_t y = ESP8266TrueRandom.random(0, MATRIX_HEIGHT);
    if (!SF[x][y])
    {
      SF[x][y] = 255;
      FF[x][y] = ESP8266TrueRandom.random(0, 255);
    }
  }

  for (byte x = 0; x < MATRIX_WIDTH; x++)
  {
    for (byte y = 0; y < MATRIX_HEIGHT; y++)
    {
      if (SF[x][y] <= 30)
      {
        SF[x][y] = 0;
      }
      else
      {
        SF[x][y] = ((SF[x][y] - 8) <= 0)
                       ? 0
                       : (SF[x][y] - 8);
        leds[XY(x, y)] = CHSV(FF[x][y], 255, SF[x][y]);
      }
    }
  }
  stripShow();
}

static void drawLocalFlickeringLights()
{
  static uint32_t lastTime = 0;

  if (millis() - lastTime < FLASH_LIGHT_DELAY)
    return;
  lastTime = millis();

  fadeToBlackBy(leds, MATRIX_LEDS, 20);
  uint16_t pos = random16(MATRIX_LEDS);
  leds[pos] += CHSV(HUE_PURPLE, 255, 255);
  stripShow();
}