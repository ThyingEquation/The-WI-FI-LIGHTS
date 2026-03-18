#include "effects.h"

/*
  Эта группа эффектов только для матриц 12х13

  Наиболее оптимальная яркость для визуализации эффекта - 50%

  Настраиваемые параметры: нет
*/

static void drawMatrixFallingLines();

void drawMatrixMovieEffect(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawMatrixFallingLines();
    break;
  }
}

static void drawMatrixFallingLines()
{
  static uint8_t currentCol[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 6};
  static uint8_t currentRow[11] = {7, 8, 5, 6, 13, 2, 5, 11, 8, 7, 11};
  static uint8_t lineLength[11] = {9, 6, 5, 7, 8, 9, 7, 5, 7, 8, 5};

  auto drawLine = [](uint8_t col, uint8_t row, uint8_t length, bool clear)
  {
    for (uint8_t j = 0; j < length; ++j)
    {
      uint8_t rowIndex = row - j;
      if (rowIndex >= 0 && rowIndex < MATRIX_HEIGHT)
      {
        uint16_t pixelIndex = XY(col, rowIndex);
        if (clear)
        {
          leds[pixelIndex] = CRGB::Black;
        }
        else
        {
          uint8_t brightness = (j == length - 1) ? 255 : 5 + (5 * j);
          leds[pixelIndex] = hsv2rgb_spectrum(CHSV(90, 255, brightness));
        }
      }
    }
  };

  for (uint8_t i = 0; i < 11; i++)
  {
    drawLine(currentCol[i], currentRow[i], lineLength[i], false);
  }
  FastLED.show();
  delay(120);

  for (uint8_t i = 0; i < 11; i++)
  {
    drawLine(currentCol[i], currentRow[i], lineLength[i], true);
  }
  FastLED.show();

  for (uint8_t i = 0; i < 11; i++)
  {
    if (currentRow[i] > 0)
    {
      --currentRow[i];
    }
    else
    {
      currentCol[i] = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
      currentRow[i] = MATRIX_HEIGHT + 8 + ESP8266TrueRandom.random(0, MATRIX_HEIGHT + 3);
      lineLength[i] = 5 + ESP8266TrueRandom.random(0, 5);
    }
  }
}