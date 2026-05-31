#pragma once

#include <LittleFS.h>

namespace Settings {
    struct AppSettings // Настройки по умолчанию
    {
        char ssid[33] = "wifiLights";
        char password[64] = "11111111";
        uint8_t localIpVal[4] = {192U, 168U, 1U, 1U};
        uint8_t gatewayVal[4] = {192U, 168U, 1U, 1U};
        uint8_t subnetVal[4] = {255U, 255U, 255U, 0U};

        bool isWifiAutoOffEnable = false; // Авто отключение WIFI через 3 минуты после запуска

        // Стартовый режим
        uint8_t startingEffectsGroup = 255U; // ==255 - отключен, иначе включен
        uint8_t startingEffectSubmode = 255U;

        // 50%, самая оптимальная яркость для всех эффектов
        uint8_t globalBrightness = 128U;

        // 1 - случайный порядок; 0 - строго по порядку массива mainModes. Эффект "Все эффекты"
        uint8_t allModesWorkType = 0U;
        // Задержка между эффектами 10 минут. Эффект "Все эффекты"
        uint32_t allModeDelay = 600000U;
    };

    extern AppSettings settings;

    void saveSettings();
    bool loadSettings();
    void resetSettings();
} // namespace Settings
