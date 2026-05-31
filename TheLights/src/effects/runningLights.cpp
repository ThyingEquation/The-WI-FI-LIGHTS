#include "colors.h"
#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (RunningLightsDelays):
    1) Скорости эффектов
*/

namespace {
    enum class RunningLightsDelays : uint16_t {
        SLOW_LIGHT_DELAY = 100U,
        FAST_LIGHT_DELAY = 20U,
        COLORFUL_LIGHT_DELAY = 35U,
        COLORFUL_SNAKE_DELAY = 50U,
        RUNNING_LIGHTS_1_DELAY = 200U,
        RUNNING_LIGHTS_2_DELAY = 200U,
        RUNNING_LIGHTS_3_DELAY = 200U
    };

    void drawColorfulLight();
    void drawLights(uint32_t lightsMode);
    void drawLight(uint32_t lightDelay);
    void drawColorfulSnake();
} // namespace

namespace Effects {
    void drawRunningLights(const uint32_t subMode) {
        switch (subMode) {
            case 0U:
                drawLight(static_cast<uint32_t>(RunningLightsDelays::SLOW_LIGHT_DELAY));
                break;

            case 1U:
                drawLight(static_cast<uint32_t>(RunningLightsDelays::FAST_LIGHT_DELAY));
                break;

            case 2U:
                drawColorfulLight();
                break;

            case 3U:
                drawColorfulSnake();
                break;

            case 4U:
                drawLights(1U);
                break;

            case 5U:
                drawLights(2U);
                break;

            case 6U:
                drawLights(3U);
                break;

            default:
                break;
        }
    }
} // namespace Effects

namespace {
    void drawLight(const uint32_t lightDelay) {
        static uint8_t color = 0U;
        static uint16_t ledsCount = 0U;
        static uint32_t lastTime = 0U;

        if (Effects::checkCommandReceived()) {
            ledsCount = 0U;
            lastTime = 0U;
            color = 0U;
            fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
        }

        if (millis() - lastTime < lightDelay) {
            return;
        }

        lastTime = millis();

        if (ledsCount < StripControl::MATRIX_LEDS) {
            if (ledsCount > 0U) {
                StripControl::leds[ledsCount - 1U] = CRGB::Black;
            }
            const uint32_t rawColor = pgm_read_dword_near(&Colors::mainColors[color]);
            StripControl::leds[ledsCount] = CRGB(rawColor);
            StripControl::show();
            ledsCount++;
        } else {
            ledsCount = 0U;

            if constexpr (StripControl::MATRIX_LEDS > 0U) {
                StripControl::leds[StripControl::MATRIX_LEDS - 1U] = CRGB::Black;
                StripControl::show();
            }
            color = static_cast<uint8_t>(ESP8266TrueRandom.random(0, 128));
        }
    }

    void drawColorfulLight() {
        static uint32_t previousMillis = 0U;
        static uint16_t currentLED = 0U;
        static bool firstPass = true;

        if (Effects::checkCommandReceived()) {
            previousMillis = 0U;
            currentLED = 0U;
            firstPass = true;
        }

        if (const uint32_t currentMillis = millis();
            currentMillis - previousMillis >= static_cast<uint32_t>(RunningLightsDelays::COLORFUL_LIGHT_DELAY)) {
            previousMillis = currentMillis;

            if (firstPass) {
                StripControl::leds[currentLED] = CHSV(static_cast<uint8_t>(currentLED), 255U, 255U);
            } else {
                StripControl::leds[currentLED] =
                        CHSV(static_cast<uint8_t>(ESP8266TrueRandom.random(0, 256)), 255U, 255U);
            }

            StripControl::show();
            StripControl::leds[currentLED] = CRGB::Black;

            currentLED++;
            if (currentLED >= StripControl::MATRIX_LEDS) {
                currentLED = 0U;
                firstPass = !firstPass;
            }
        }
    }

    void drawColorfulSnake() {
        static uint32_t color = 0U;
        static uint32_t head = 0U;
        static uint32_t tail = 0U;
        static uint32_t pixelCounter = 0U;
        static uint32_t previousMillis = 0U;

        static uint16_t snake[10] = {};

        if (Effects::checkCommandReceived()) {
            head = 0U;
            tail = 0U;
            pixelCounter = 0U;
            previousMillis = 0U;
            (void) memset(&snake[0], 0xFF, sizeof(snake));
        }

        if (millis() - previousMillis >= static_cast<uint32_t>(RunningLightsDelays::COLORFUL_SNAKE_DELAY)) {
            previousMillis = millis();

            head = (head + 1U) % StripControl::MATRIX_LEDS;

            tail = (tail + 1U) % 10U;
            snake[tail] = static_cast<uint16_t>(head);

            pixelCounter++;
            if (pixelCounter >= StripControl::MATRIX_LEDS) {
                pixelCounter = 0U;
                color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            }

            fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

            const uint32_t rawColor = pgm_read_dword_near(&Colors::mainColors[color]);
            const auto snakeColor = CRGB(rawColor);

            for (const unsigned short i: snake) {
                if (i < StripControl::MATRIX_LEDS) {
                    StripControl::leds[i] = snakeColor;
                }
            }
            StripControl::show();
        }
    }

    void drawLights(const uint32_t lightsMode) {
        static uint32_t color = 0U;
        static uint32_t stepJ = 0U;
        static uint32_t stepQ = 0U;
        static uint32_t lastTime = 0U;
        static bool isNewCycle = true;

        constexpr uint32_t delays[] = {static_cast<uint32_t>(RunningLightsDelays::RUNNING_LIGHTS_1_DELAY),
                                       static_cast<uint32_t>(RunningLightsDelays::RUNNING_LIGHTS_2_DELAY),
                                       static_cast<uint32_t>(RunningLightsDelays::RUNNING_LIGHTS_3_DELAY)};

        if (const uint32_t currentDelay = delays[constrain(lightsMode, 1, 3) - 1U];
            millis() - lastTime < currentDelay) {
            return;
        }
        lastTime = millis();

        if (isNewCycle && lightsMode == 1U) {
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            isNewCycle = false;
        }

        const uint8_t prevQ = stepQ == 0U ? 2U : stepQ - 1U;
        for (uint32_t i = 0U; i < StripControl::MATRIX_LEDS; i = i + 3U) {
            if (i + prevQ < StripControl::MATRIX_LEDS) {
                StripControl::leds[i + prevQ] = CRGB::Black;
            }
        }

        for (uint32_t i = 0U; i < StripControl::MATRIX_LEDS; i = i + 3U) {
            CRGB pixelColor;

            switch (lightsMode) {
                case 1U:
                    pixelColor = CRGB(pgm_read_dword_near(&Colors::mainColors[color]));
                    break;

                case 2U:
                    pixelColor = Effects::wheel((i + stepJ) % 255U);
                    break;

                case 3U:
                    pixelColor = CRGB(pgm_read_dword_near(&Colors::mainColors[ESP8266TrueRandom.random(0, 128)]));
                    break;
                default:;
            }

            if (i + stepQ < StripControl::MATRIX_LEDS) {
                StripControl::leds[i + stepQ] = pixelColor;
            }
        }

        StripControl::show();

        stepQ++;
        if (stepQ >= 3U) {
            stepQ = 0U;
            stepJ++;

            if (stepJ >= 2U) {
                stepJ = 0U;
                isNewCycle = true;
            }
        }
    }
} // namespace
