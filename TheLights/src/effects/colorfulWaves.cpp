#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (colorfulWavesSettings):
    1) Частота кадров волны (задержка)
*/

enum colorfulWavesSettings
{
  
};

static void drawSinusoidWaves(uint8_t sinNum);
static void drawDiagonalWaves();

void drawColorfulWaves(uint8_t subMode)
{
  switch (subMode)
  {
  case 5:
    drawDiagonalWaves();
    break;

  default:
    break;
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