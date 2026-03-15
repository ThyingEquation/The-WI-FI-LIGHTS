/*
  * Баранов К.Д. - Версия кода: 4.0 (2026) - Версия андроид приложения - хх.хх.хх (2026)

  * В основной массе эффекты сделаны и протестированы на матрице размером 12х13 (стандартное окно 1,5м Х 1,5м)
  * Для всех режимов в их .cpp файлах приведены пояснения и локальные настройки режима
  * Настройки используемые при инициализации и сохраняемые в энергонезависимой памяти приведены в файле "settings.h"
  * Глобальные настройки гирлянды-матрицы ниже в enum globalSettings
    - TEXT_POS позиция текста для режима "Бегущая строка". NEO_MATRIX_LEFT - текст внутри; NEO_MATRIX_RIGHT - текст снаружи

*/

#pragma once

#include <Adafruit_NeoMatrix.h>
#include <FastLED.h>

enum globalSettings
{
  MATRIX_WIDTH = 12,
  MATRIX_HEIGHT = 13,
  MATRIX_LEDS = MATRIX_WIDTH * MATRIX_HEIGHT,
  TEXT_POS = NEO_MATRIX_LEFT,
  PIN = 4,
  ///
  ALL_MODES_EFFECT_COUNT = 60
};

extern Adafruit_NeoPixel strip;
extern Adafruit_NeoMatrix matrix;
extern CRGB *const leds;