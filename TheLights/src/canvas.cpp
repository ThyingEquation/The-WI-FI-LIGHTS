#include "canvas.h"

const int colorsCanvas[] PROGMEM = {0x000000,
                                    0x000000 /*черный*/,
                                    0xffffff /*белый*/,
                                    0x0000ff /*синий*/,
                                    0x00ff00 /*лайм*/,
                                    0x00bfff /*морозное небо*/,
                                    0xff1493 /*малиновый*/,
                                    0xffff00 /*желтый*/,
                                    0x7fffd4 /*аквамариновый*/,
                                    0x00ff7f /*весенне-зеленый*/,
                                    0xffa500 /*оранжевый*/,
                                    0xff0000 /*красный*/,
                                    0x9400d3 /*фиолетово-баклажанный*/,
                                    0xffb6c1 /*светлорозовый*/,
                                    0x4b0082 /*индиго*/,
                                    0xffd700 /*золотой*/,
                                    0x008000 /*зеленый*/,
                                    0x22262c /*серый*/,
                                    0x964b00 /*коричневый*/,
                                    0x6600ff /*персидский синий*/,
                                    0xd76e00 /*темно-оранжевый*/,
                                    0x7cfc00 /*зеленая лужайка*/};

void canvas(String mode, uint8_t color, int led) {
  if (mode.indexOf("A") != -1) {
    if (led == 257) {
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, colorsCanvas[color]);
      }
      strip.show();
    } else {
      strip.setPixelColor(--led, colorsCanvas[color]);
      strip.show();
    }
  }

  else if (mode.indexOf("B") != -1) {
    if (led == 257) {
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
    } else {
      strip.setPixelColor(--led, strip.Color(0, 0, 0));
      strip.show();
    }
  }
}