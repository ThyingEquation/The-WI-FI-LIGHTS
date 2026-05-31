#include "effects.h"

/*
  Эта группа эффектов только для матрицы 12х12
  Настраиваемые параметры: нет
*/

namespace {
    struct Particle {
        float x;
        float y;
        float speed;
    };

    Particle particles[25];

    void initWeather();
    void drawWeather(uint32_t count, uint32_t color, uint32_t minSpeed, uint32_t maxSpeed, uint32_t drift,
                     uint32_t trail);
} // namespace

namespace Effects {
    void drawWeatherEffects(const uint32_t subMode) {

        static uint32_t lastUpdate = 0U;

        if (millis() - lastUpdate < 30U) {
            return;
        }
        lastUpdate = millis();

        if (checkCommandReceived()) {
            initWeather();
        }

        switch (subMode) {
            case 0U:
                drawWeather(10U, 0xE0E0E0U, 5U, 15U, 10U, 200U);
                break;
            case 1U:
                drawWeather(18U, 0xFFFFFFU, 20U, 50U, 11U, 150U);
                break;
            case 2U:
                drawWeather(10U, 0x007DFFU, 30U, 60U, 10U, 100U);
                break;
            case 3U:
                drawWeather(20U, 0x0014A8U, 60U, 100U, 10U, 80U);
                break;
            default:;
        }
        StripControl::show();
    }
} // namespace Effects

namespace {
    void initWeather() {
        for (auto &particle: particles) {
            particle.y = -10.0F;
        }
        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
        StripControl::show();
    }

    void drawWeather(const uint32_t count, const uint32_t color, const uint32_t minSpeed, const uint32_t maxSpeed,
                     const uint32_t drift, const uint32_t trail) {
        static constexpr uint32_t DRIFT_NEUTRAL = 10U;

        fadeToBlackBy(&StripControl::leds[0], static_cast<uint16_t>(StripControl::MATRIX_LEDS),
                      static_cast<uint8_t>(trail));

        const float xDrift = drift != DRIFT_NEUTRAL ? static_cast<float>(drift - DRIFT_NEUTRAL) / 20.0F : 0.0F;

        for (uint32_t i = 0U; i < count; i++) {

            particles[i].y -= particles[i].speed;
            particles[i].x += xDrift;

            if (particles[i].y < -1.0F || particles[i].x < -1.0F ||
                particles[i].x > static_cast<float>(StripControl::MATRIX_WIDTH)) {
                particles[i].y = static_cast<float>(StripControl::MATRIX_HEIGHT + random8(15U));
                particles[i].x = static_cast<float>(random8(static_cast<uint8_t>(StripControl::MATRIX_WIDTH)));
                particles[i].speed = static_cast<float>(
                        static_cast<float>(random8(static_cast<uint8_t>(minSpeed), static_cast<uint8_t>(maxSpeed))) /
                        100.0F);
            }

            const auto x = static_cast<int32_t>(particles[i].x);
            const auto y = static_cast<int32_t>(particles[i].y);

            if (x >= 0 && x < static_cast<int32_t>(StripControl::MATRIX_WIDTH) && y >= 0 &&
                y < static_cast<int32_t>(StripControl::MATRIX_HEIGHT)) {
                if (const auto index =
                            static_cast<int32_t>(Effects::getIndex(static_cast<uint32_t>(x), static_cast<uint32_t>(y)));
                    index < static_cast<int32_t>(StripControl::MATRIX_LEDS)) {
                    StripControl::leds[index] = CRGB(color);
                }
            }
        }
    }
} // namespace
