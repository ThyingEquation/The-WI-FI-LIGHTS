/*
  * Баранов К.Д. - Версия кода: 4.0 (2026) - Версия андроид приложения - хх.хх.хх (2026)

  * В основной массе эффекты сделаны и протестированы на матрице размером 12х12 (стандартное ПВХ окно 1,5м Х 1,5м)
  * Для всех режимов в их .cpp файлах приведены пояснения и локальные настройки режима
  * Настройки используемые при инициализации и сохраняемые в энергонезависимой памяти приведены в файле "settings.h"
  * Глобальные настройки гирлянды-матрицы ниже в enum globalSettings

*/

#pragma once

#include <FastLED.h>
#include <NeoPixelBus.h>

namespace StripControl {
    constexpr uint32_t MATRIX_WIDTH  = 12U;
    constexpr uint32_t MATRIX_HEIGHT = 12U;
    constexpr uint32_t MATRIX_LEDS = MATRIX_WIDTH * MATRIX_HEIGHT;

    extern CRGB leds[MATRIX_LEDS];
    extern NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip;
}
