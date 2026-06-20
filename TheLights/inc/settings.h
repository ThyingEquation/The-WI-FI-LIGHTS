#pragma once

#include <LittleFS.h>

namespace Settings {
    enum class AnimationsDelays : uint32_t {
        ANDROID_AP = 1U,
        ANDROID_STA = 2U,
        WEB_PAGE_AP = 3U
    };

    constexpr uint8_t EFFECTS_BITMASK_SIZE = 10U;

    struct SettingsParameters // Настройки по умолчанию
    {
        char localNetworkSsid[32] = "wifiLights";
        char localNetworkPassword[64] = "11111111";
        char stationNetworkSsid[32] = "your Wi-Fi network name";
        char stationNetworkPassword[64] = "11111111";
        uint8_t localIpVal[4] = {192U, 168U, 1U, 1U};
        uint8_t gatewayVal[4] = {192U, 168U, 1U, 1U};
        uint8_t subnetVal[4] = {255U, 255U, 255U, 0U};

        uint32_t workMode = static_cast<uint32_t>(AnimationsDelays::ANDROID_AP);

        bool isWifiAutoOffEnable = false; // Авто отключение WIFI через 3 минуты после запуска

        // Стартовый режим
        uint32_t startingEffectsGroup = 255U; // ==255 - отключен, иначе включен
        uint32_t startingEffectSubmode = 255U;

        // 50%, самая оптимальная яркость для всех эффектов
        uint32_t globalBrightness = 128U;

        // 1 - случайный порядок; 0 - строго по порядку массива mainModes. Эффект "Все эффекты"
        uint32_t allModesWorkType = 0U;
        // Задержка между эффектами 10 минут. Эффект "Все эффекты"
        uint32_t allModeDelay = 600000U;

        uint8_t enabledEffectsMask[EFFECTS_BITMASK_SIZE]{};
    };

    extern SettingsParameters parameters;

    void save();
    bool load();
    void reset();
} // namespace Settings
