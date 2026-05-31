#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (FlickeringLightsDelays):
    1) Скорости эффектов
*/

namespace {
    enum class FlickeringLightsDelays : uint16_t {
        FLYING_LIGHTS_DELAY = 75U,
        CONFETTI_DELAY = 20U,
        FLASH_LIGHT_DELAY = 17U
    };

    void drawFlyingLights();
    void drawConfetti();
    void drawLocalFlickeringLights();
}

namespace Effects {
    void drawFlickeringLights(const uint32_t subMode) {
        switch (subMode) {
            case 0U:
                drawFlyingLights();
                break;

            case 1U:
                drawConfetti();
                break;

            case 2U:
                drawLocalFlickeringLights();
                break;

            default:
                break;
        }
    }
}

namespace {
    void drawFlyingLights() {
        static constexpr auto HEAD_COLOR = CRGB(175U, 255U, 175U);
        static constexpr auto TAIL_COLOR = CRGB(27U, 130U, 39U);

        EVERY_N_MILLIS(static_cast<uint32_t>(FlickeringLightsDelays::FLYING_LIGHTS_DELAY)) {
            bool emptyScreen = true;
            for (int32_t row = static_cast<int16_t>(StripControl::MATRIX_HEIGHT - 1U); row >= 0; row--) {
                for (int8_t col = 0; col < static_cast<int8_t>(StripControl::MATRIX_WIDTH); col++) {
                    if (StripControl::leds[Effects::getIndex(static_cast<uint32_t>(col), static_cast<uint32_t>(row))] == HEAD_COLOR) {
                        StripControl::leds[Effects::getIndex(static_cast<uint32_t>(col), static_cast<uint32_t>(row))] = TAIL_COLOR;
                        if (row < static_cast<int16_t>(StripControl::MATRIX_HEIGHT - 1U)) {
                            auto nextCol = static_cast<int16_t>(col + static_cast<int16_t>(random8(3U) - 1U));
                            if (nextCol < 0) {
                                nextCol = 0;
                            }
                            if (nextCol >= static_cast<int16_t>(StripControl::MATRIX_WIDTH)) {
                                nextCol = static_cast<int16_t>(StripControl::MATRIX_WIDTH - 1U);
                            }

                            StripControl::leds[Effects::getIndex(static_cast<uint32_t>(nextCol), static_cast<uint32_t>(row) + 1U)] = HEAD_COLOR;
                        }
                    }
                }
            }

            for (auto &led: StripControl::leds) {
                if (led.g != 255U) {
                    (void)led.nscale8(180U);
                }
                if (led) {
                    emptyScreen = false;
                }
            }

            if (random8(5U) == 0U || emptyScreen) {
                const uint32_t spawnX = random8(static_cast<uint8_t>(StripControl::MATRIX_WIDTH));
                StripControl::leds[Effects::getIndex(spawnX, 0U)] = CRGB(175U, 255U, 175U);
            }
            StripControl::show();
        }
    }

    void drawConfetti() {
        static bool loadingFlag = true;
        static uint8_t FF[StripControl::MATRIX_WIDTH][StripControl::MATRIX_HEIGHT] = {};
        static uint8_t SF[StripControl::MATRIX_WIDTH][StripControl::MATRIX_HEIGHT] = {};
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(FlickeringLightsDelays::CONFETTI_DELAY)) {
            return;
        }
        lastTime = millis();

        if (loadingFlag) {
            memset8(&SF[0], 0, sizeof(SF));
            memset8(&FF[0], 0, sizeof(FF));
            loadingFlag = false;
        }

        for (byte i = 0U; i < 8U; i++) {
            const auto x = static_cast<uint32_t>(ESP8266TrueRandom.random(0, static_cast<long>(StripControl::MATRIX_WIDTH)));
            const auto y = static_cast<uint32_t>(ESP8266TrueRandom.random(0, static_cast<long>(StripControl::MATRIX_HEIGHT)));
            if (SF[x][y] == 0U) {
                SF[x][y] = 255U;
                FF[x][y] = static_cast<uint8_t>(ESP8266TrueRandom.random(0, 255));
            }
        }

        for (byte x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
            for (byte y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
                if (SF[x][y] <= 30U) {
                    SF[x][y] = 0U;
                } else {
                    SF[x][y] = (SF[x][y] <= 8U) ? static_cast<uint8_t>(0) : static_cast<uint8_t>(SF[x][y] - 8U);
                    StripControl::leds[Effects::getIndex(x, y)] = CHSV(FF[x][y], 255U, SF[x][y]);
                }
            }
        }
        StripControl::show();
    }

    void drawLocalFlickeringLights() {
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(FlickeringLightsDelays::FLASH_LIGHT_DELAY)) {
            return;
        }
        lastTime = millis();

        fadeToBlackBy(&StripControl::leds[0], static_cast<uint16_t>(StripControl::MATRIX_LEDS), 20U);
        StripControl::leds[random16(static_cast<uint16_t>(StripControl::MATRIX_LEDS))] += CHSV(static_cast<uint8_t>(HUE_PURPLE), 255U, 255U);
        StripControl::show();
    }
}