#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (flickeringLightsSettings):
    1) Скорости эффектов
*/

enum flickeringLightsSettings
{
  FLASH_LIGHT_1_DELAY = 75,
  FLASH_LIGHT_2_DELAY = 20,
  FLASH_LIGHT_3_DELAY = 1000 / 60
};

static void drawFlickeringLights1();
static void drawFlickeringLights2();
static void drawFlickeringLights3();

void drawFlickeringLights(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawFlickeringLights1();
    break;

  case 2:
    drawFlickeringLights2();
    break;

  case 3:
    drawFlickeringLights3();
    break;

  default:
    break;
  }
}

static void drawFlickeringLights1()
{
  EVERY_N_MILLIS(FLASH_LIGHT_1_DELAY)
  {
    for (int8_t row = MATRIX_HEIGHT - 1; row >= 0; row--)
    {
      for (int8_t col = 0; col < MATRIX_WIDTH; col++)
      {
        if (leds[getIndex(col, row)] == CRGB(175, 255, 175))
        {
          leds[getIndex(col, row)] = CRGB(27, 130, 39);
          if (row < MATRIX_HEIGHT - 1)
          {
            int8_t drift = random8(3) - 1;
            int8_t nextCol = col + drift;

            if (nextCol < 0)
              nextCol = 0;
            if (nextCol >= MATRIX_WIDTH)
              nextCol = MATRIX_WIDTH - 1;

            leds[getIndex(nextCol, row + 1)] = CRGB(175, 255, 175);
          }
        }
      }
    }

    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    {
      if (leds[i].g != 255)
        leds[i].nscale8(180);
    }

    bool emptyScreen = true;
    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    {
      if (leds[i])
      {
        emptyScreen = false;
        break;
      }
    }

    if (random8(5) == 0 || emptyScreen)
    {
      int8_t spawnX = random8(MATRIX_WIDTH);
      leds[getIndex(spawnX, 0)] = CRGB(175, 255, 175);
    }

    FastLED.show();
  }
}

static void drawFlickeringLights2()
{
  static bool loadingFlag = true;

  static byte FF[MATRIX_WIDTH][MATRIX_HEIGHT];
  static byte SF[MATRIX_WIDTH][MATRIX_HEIGHT];

  if (loadingFlag)
  {
    memset8(SF, 0, MATRIX_LEDS);
    memset8(FF, 0, MATRIX_LEDS);
    loadingFlag = 0;
  }
  for (byte i = 0; i < map(128, 1, 255, 2, 16); i++)
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
        SF[x][y] = 0;
      else
        SF[x][y] = ((SF[x][y] - map(128, 1, 255, 1, 16)) <= 0)
                       ? 0
                       : (SF[x][y] - map(128, 1, 255, 1, 16));
      leds[XY(x, y)] = CHSV(FF[x][y], 255, SF[x][y]);
    }
  }
  delay(FLASH_LIGHT_2_DELAY);
  FastLED.show();
}

static void drawFlickeringLights3()
{
  fadeToBlackBy(leds, MATRIX_LEDS, 20);
  uint16_t pos = random16(MATRIX_LEDS);
  leds[pos] += CHSV(HUE_PURPLE, 255, 255);
  FastLED.show();
  FastLED.delay(FLASH_LIGHT_3_DELAY);
}