#include "modes.h"
#include "colors.h"
#include "lightsSettings.h"

#include <Adafruit_NeoMatrix.h>

/*
  Эта группа эффектов для любого размера матриц

  В библиотеке Adafruit_GFX_library (-> TheLights -> .pio -> libdeps -> Adafruit_GFX_library) заменить файл glcdfont.c на одноименный
  из репозитория

  Настраиваемые параметры (runningLineSettings):
    1) Скорость бегущей строки
    2) Отступ текста от верхней части матрицы
    3) TEXT_POS позиция текста для режима "Бегущая строка". NEO_MATRIX_LEFT - Текст снаружи; NEO_MATRIX_RIGHT - Текст внутри

    P.S. Ваша гирлянда висит на окне, при этом первый светодиод находится в правом нижнем углу, в таком случае если выбрать "Текст внутри"
  то бегущая строка будет двигаться справа налево и будет виден внутри помещения. Соответсвенно "Текст снаружи" наоборот

*/

enum runningLineSettings
{
  RUNNING_LINE_DELAY = 250,
  TOP_MARGIN = 3,
  TEXT_POS = NEO_MATRIX_LEFT
};

static String utf8rus(String source);

const char *text[] = {"С НОВЫМ ГОДОМ!!!",
                      "ЗДЕСЬ МОГЛА БЫТЬ ВАША РЕКЛАМА",
                      "ПРИВЕТ, Я УМНАЯ ГИРЛЯНДА",
                      "ВВЕДИТЕ ТЕКСТ",
                      "С НАСТУПАЮЩИМ НОВЫМ ГОДОМ!!!",
                      "С РОЖДЕСТВОМ!!!",
                      "HAPPY NEW YEAR!!!"};

const int16_t textLength[] = {100, 173, 137, 85, 183, 98, 98};

static Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    MATRIX_WIDTH, MATRIX_HEIGHT, PIN,
    NEO_MATRIX_BOTTOM + TEXT_POS + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);

void initRunningLine() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(settings.intBrightness);
  matrix.setTextColor(pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(0, 128)])));
}

void drawRunningLine(uint8_t subMode)
{

  static int16_t g = MATRIX_WIDTH;

  if (subMode > 7)
  {
    g = MATRIX_WIDTH;
  }

  if ((subMode - 1) != -1)
  {
    matrix.fillScreen(matrix.Color(0, 0, 0));
    matrix.setCursor(g, TOP_MARGIN);
    matrix.print(utf8rus(text[subMode - 1]));

    if (--g < -textLength[subMode - 1])
    {
      g = matrix.width();
      matrix.setTextColor(pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(0, 128)])));
    }
    matrix.show();
    delay(RUNNING_LINE_DELAY);
  }
}

static String utf8rus(String source)
{
  uint8_t a, k;
  String target;
  uint8_t n;
  char m[2] = {'0', '\0'};

  k = source.length();
  a = 0;

  while (a < k)
  {
    n = source[a];
    a++;

    if (n >= 0xC0)
    {
      switch (n)
      {
      case 0xD0:
      {
        n = source[a];
        a++;
        if (n == 0x81)
        {
          n = 0xA8;
          break;
        }
        if (n >= 0x90 && n <= 0xBF)
          n = n + 0x30;
        break;
      }
      case 0xD1:
      {
        n = source[a];
        a++;
        if (n == 0x91)
        {
          n = 0xB8;
          break;
        }
        if (n >= 0x80 && n <= 0x8F)
          n = n + 0x70;
        break;
      }
      }
    }
    m[0] = n;
    target = target + String(m);
  }
  return target;
}