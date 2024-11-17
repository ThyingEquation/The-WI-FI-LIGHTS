#include <canvas.h>

unsigned char x = 0;
unsigned char y = 0;
unsigned char z = 0;

void canvas(String mode, uint8_t color, int led) {

  if (mode.indexOf("A") != -1) {
    if (led == 257) {
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, chooseColor(color));
      }
      strip.show();
      //delay(150); ///////////////////////////////////////////////////////////////////
    } else {
      strip.setPixelColor(--led, chooseColor(color));
      strip.show();
      //delay(150); ///////////////////////////////////////////////////////////////////
    }
  }

  else if (mode.indexOf("B") != -1) {
    if (led == 257) {
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      //delay(150); ///////////////////////////////////////////////////////////////////////
      } else {
        strip.setPixelColor(--led, strip.Color(0, 0, 0));
        strip.show();
        //delay(150); //////////////////////////////////////////////////////////////////////
    }
  }
}

int chooseColor(uint8_t color) {
  switch (color) {
    case 1:  // черный
      return 0x000000;
      break;
    case 2:  // белый
      return 0xffffff;
      break;
    case 3:  // синий
      return 0x0000ff;
      break;
    case 4:  // лайм
      return 0x00ff00;
      break;
    case 5:  // морозное небо
      return 0x00bfff;
      break;
    case 6:  // малиновый
      return 0xff1493;
      break;
    case 7:  // желтый
      return 0xffff00;
      break;
    case 8:  // аквамариновый
      return 0x7fffd4;
      break;
    case 9:  // весенне-зеленый
      return 0x00ff7f;
      break;
    case 10:  // оранжевый
      return 0xffa500;
      break;
    case 11:  // красный
      return 0xff0000;
      break;
    case 12:  // фиолетово-баклажанный
      return 0x9400d3;
      break;
    case 13:  // светлорозовый
      return 0xffb6c1;
      break;
    case 14:  // индиго
      return 0x4b0082;
      break;
    case 15:  // золотой
      return 0xffd700;
      break;
    case 16:  // зеленый
      return 0x008000;
      break;
    case 17:  // Cерый
      return 0x22262c;
      break;
    case 18:  // Коричневый
      return 0x964b00;
      break;
    case 19:  // Персидский синий
      return 0x6600ff;
      break;
    case 20:  // Темно-оранжевый
      return 0xd76e00;
      break;
    case 21:  // Зеленая лужайка
      return 0x7cfc00;
      break;
    default:
      break;
  }
  return 0x000000;
}
