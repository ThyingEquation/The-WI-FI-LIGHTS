#include "effects.h"

/*
  Эта группа эффектов только для матрицы 12х12

  Настраиваемые параметры: нет
*/

static void weatherEffects(uint8_t pieceCount, uint8_t speed, uint8_t *currentCol,
                           uint8_t *currentRow, uint32_t color);

void drawWeatherEffects(uint8_t subMode)
{
  static uint8_t currentCols[][22] = {
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11},
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11, 9, 4, 2},
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11},
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11, 9, 4, 2, 0, 2, 5}};

  static uint8_t currentRows[][22] = {
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14},
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14, 4, 9, 12, 3},
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14},
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14, 4, 9, 12, 3, 3, 6, 9}};

  static const uint8_t lengths[] = {15, 19, 15, 22};
  static const uint8_t delays[] = {180, 95, 60, 30};
  static const uint32_t colors[] = {0xf2f3f4, 0xf2f3f4, 0x007dff, 0x0014a8};

  uint8_t index = subMode - 1;
  weatherEffects(lengths[index], delays[index], currentCols[index],
                 currentRows[index], colors[index]);
}

void weatherEffects(uint8_t pieceCount, uint8_t speed, uint8_t *currentCol,
                    uint8_t *currentRow, uint32_t color)
{
  auto drawPixels = [&](bool clear)
  {
    for (uint8_t i = 0; i < pieceCount; i++)
    {
      if (currentRow[i] < MATRIX_HEIGHT)
      {
        uint16_t pixelIndex = XY(currentCol[i], currentRow[i]);

        if (clear)
          leds[pixelIndex] = CRGB::Black;
        else
          leds[pixelIndex] = CRGB(color);
      }
    }
  };
  drawPixels(true);

  for (uint8_t i = 0; i < pieceCount; i++)
  {
    if (currentRow[i] > 0)
    {
      --currentRow[i];
    }
    else
    {
      currentCol[i] = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
      currentRow[i] = MATRIX_HEIGHT + ESP8266TrueRandom.random(0, 5);
    }
  }

  drawPixels(false);
  FastLED.show();

  delay(speed);
}