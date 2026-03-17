#include "effects.h"

/*
  Эта группа эффектов только для матриц 8x8, 12х13 и 16х16

  Настраиваемые параметры: нет
*/

const uint32_t colorsCanvas[] PROGMEM = {0x000000,  // Чёрный
                                    0xffffff,  // Белый
                                    0x0000ff,  // Синий
                                    0x00ff00,  // Лайм
                                    0x00bfff,  // Морозное небо
                                    0xff1493,  // Малиновый
                                    0xffff00,  // Желтый
                                    0x7fffd4,  // Аквамариновый
                                    0x00ff7f,  // Весенне-зеленый
                                    0xffa500,  // Оранжевый
                                    0xff0000,  // Красный
                                    0x9400d3,  // Фиолетово-баклажанный
                                    0xffb6c1,  // Светлорозовый
                                    0x4b0082,  // Индиго
                                    0xffd700,  // Золотой
                                    0x008000,  // Зеленый
                                    0x22262c,  // Серый
                                    0x964b00,  // Коричневый
                                    0x6600ff,  // Персидский синий
                                    0xd76e00,  // Темно-оранжевый
                                    0x7cfc00}; // Зеленая лужайка

void drawOnCanvas(String canvasMode, uint8_t color, uint16_t ledNum)
{
  if (canvasMode.indexOf("A") != -1)
  {
    if (ledNum == 257)
    {
      uint32_t rawColor = pgm_read_dword_near(&colorsCanvas[color]);
      fill_solid(leds, MATRIX_LEDS, CRGB(rawColor));
      FastLED.show();
    }
    else
    {
      uint16_t index = --ledNum;
      if (index < MATRIX_LEDS) {
        uint32_t rawColor = pgm_read_dword_near(&colorsCanvas[color]);
        leds[index] = CRGB(rawColor);
        FastLED.show();
      }
    }
  }
  else if (canvasMode.indexOf("B") != -1)
  {
    if (ledNum == 257)
    {
      FastLED.clear();
      FastLED.show();
    }
    else
    {
      uint16_t index = --ledNum;
      if (index < MATRIX_LEDS) {
        leds[index] = CRGB::Black;
        FastLED.show();
      }
    }
  }
}