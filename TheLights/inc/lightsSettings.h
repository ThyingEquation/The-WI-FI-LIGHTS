#pragma once

#include <LittleFS.h>

typedef struct // Настройки по умолчанию
{
  char ssid[33] = "wifiLights";
  char password[64] = "11111111";
  uint8_t localIpVal[4] = {192, 168, 1, 1};
  uint8_t gatewayVal[4] = {192, 168, 1, 1};
  uint8_t subnetVal[4] = {255, 255, 255, 0};

  bool isWifiAutoOffEnable = 0; // Автоотключение WIFI через 3 минуты после запуска

  // Стартовый режим
  uint8_t startingEffectsGroup = 255; // ==255 - отключен, иначе включен
  uint8_t startingEffectSubmode = 255;

  uint8_t globalBrightness = 128; // 50%, самая оптимальная яркость для всех эффектов

  uint8_t allModesWorkType = 0;   // 1 - случайный порядок; 0 - строго по порядку массива mainModes. Эффект "Все эффекты"
  uint32_t allModeDelay = 600000; // задержка между эффектами 10 минут. Эффект "Все эффекты"
  // bool isAllModesEffectEnable;
} appSettings_s;

extern appSettings_s settings;

void saveSettings();
bool loadSettings();
void resetSettings();