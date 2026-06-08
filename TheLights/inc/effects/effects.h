#pragma once

#include <ESP8266TrueRandom.h>
#include <string_view>

#include "main.h"

namespace StripControl {
    void show();
}

namespace Effects {
    uint32_t getIndex(uint32_t x, uint32_t y);
    void drawPixel(float x, float y, const CRGB &color);
    CRGB wheel(uint32_t WheelPos);

    bool checkCommandReceived();
    void drawPicture(const uint32_t *p2);
    void initRunningLine();

    void drawColorfulEffects(uint32_t subMode);
    void drawAnimations(uint32_t subMode);
    void drawOnCanvas(std::string_view mode, uint32_t color, uint32_t ledNum);
    void drawCanvasEffects(uint32_t subMode);
    void drawFlickeringLights(uint32_t subMode);
    void drawGamesEffects(uint32_t subMode);
    void drawJumpingLights(uint32_t subMode);
    void drawRunningLights(uint32_t subMode);
    void drawRunningLine(uint32_t subMode);
    void drawSpaceEffects(uint32_t subMode);
    void drawWaterEffects(uint32_t subMode);
    void drawWeatherEffects(uint32_t subMode);

    extern const uint8_t mainMatrixScheme[144];

    constexpr uint32_t EFFECT_DISABLED = 255U;

    struct EffectsState {
        bool isScreenClearEnable = true;
        bool isAllModesEnable = false;
        bool isWifiActive = true;
        uint32_t effectsGroup = 255U;
        uint32_t effectSubmode = 255U;
        uint32_t currentIndex = 255U;
    };

    extern EffectsState state;
}
