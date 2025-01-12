/*

Нужно в библиотеке Adafruit_GFX_library файл
glcdfont.c заменить на аналогичный с этого проекта или с сайта автора данного
эффекта (Адресная лента WS2812b, Бегущая строка, Управление шрифтами.):
http://arduino-kid.ru/blog/adresnaya-lenta-ws2812b-beguschaya-stroka-upravlenie-shriftami

- для точки отсчета брать if (--g < -125), где 125 зависит от длины текста, так
- например 125 при длине текста 21
- число 3 - отступ сверху

*/

#include "runningLine.h"
#include "colors.h"

static String utf8rus(String source);

void runningLine(uint8_t subMode) {
  const char* messages[] = {"С НОВЫМ ГОДОМ!!!",
                            "ЗДЕСЬ МОГЛА БЫТЬ ВАША РЕКЛАМА",
                            "ПРИВЕТ, Я УМНАЯ ГИРЛЯНДА",
                            "ВВЕДИТЕ ТЕКСТ",
                            "С НАСТУПАЮЩИМ НОВЫМ ГОДОМ!!!",
                            "С РОЖДЕСТВОМ!!!",
                            "HAPPY NEW YEAR!!!"};

  const int messageLengths[] = {100, 173, 137, 85, 183, 98, 98};

  static int messageIndex = -1;
  static int g = mWidth;

  switch (subMode) {
    case 1:
      messageIndex = 0;
      break;

    case 2:
      messageIndex = 1;
      break;

    case 3:
      messageIndex = 2;
      break;

    case 4:
      messageIndex = 3;
      break;

    case 5:
      messageIndex = 4;
      break;

    case 6:
      messageIndex = 5;
      break;

    case 7:
      messageIndex = 6;
      break;

    default:
      g = mWidth;
      break;
  }

  if (messageIndex != -1) {
    matrix.fillScreen(matrix.Color(0, 0, 0));
    matrix.setCursor(g, 3);
    matrix.print(utf8rus(messages[messageIndex]));
    if (--g < -messageLengths[messageIndex]) {
      g = matrix.width();
      matrix.setTextColor(pgm_read_dword(&(mainColors[rand() % 127])));
    }
    matrix.show();
    delay(250);
  }
}

String utf8rus(String source) {
  int a, k;
  String target;
  unsigned char n;
  char m[2] = {'0', '\0'};

  k = source.length();
  a = 0;

  while (a < k) {
    n = source[a];
    a++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[a];
          a++;
          if (n == 0x81) {
            n = 0xA8;
            break;
          }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[a];
          a++;
          if (n == 0x91) {
            n = 0xB8;
            break;
          }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n;
    target = target + String(m);
  }
  return target;
}
