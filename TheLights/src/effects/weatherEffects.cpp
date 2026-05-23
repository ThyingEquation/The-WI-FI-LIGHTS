#include "effects.h"

/*
  Эта группа эффектов только для матрицы 12х12
  Настраиваемые параметры: нет
*/

struct Particle {
    float x, y;
    float speed;
};

static Particle particles[25];

void initWeather();
void drawWeather(uint8_t count, uint32_t color, uint8_t minSpeed, uint8_t maxSpeed, uint8_t drift, uint8_t trail);

void drawWeatherEffects(const uint8_t subMode) {

    static uint32_t lastUpdate = 0;

    if (millis() - lastUpdate < 30)
        return;
    lastUpdate = millis();

    if (checkCommandReceived()) {
        initWeather();
    }

    switch (subMode) {
        case 0:
            drawWeather(10, 0xE0E0E0, 5, 15, 10, 200);
            break;
        case 1:
            drawWeather(18, 0xFFFFFF, 20, 50, 11, 150);
            break;
        case 2:
            drawWeather(10, 0x007DFF, 30, 60, 10, 100);
            break;
        case 3:
            drawWeather(20, 0x0014A8, 60, 100, 10, 80);
            break;
        default:;
    }
    stripShow();
}

void initWeather() {
    for (auto &particle: particles) {
        particle.y = -10;
    }
    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    stripShow();
}

void drawWeather(uint8_t count, const uint32_t color, const uint8_t minSpeed, const uint8_t maxSpeed,
                 const uint8_t drift, const uint8_t trail) {
    static constexpr uint8_t DRIFT_NEUTRAL = 10;

    if (count > 25)
        count = 25;

    fadeToBlackBy(leds, MATRIX_LEDS, trail);

    const float xDrift = drift != DRIFT_NEUTRAL ? static_cast<float>(drift - DRIFT_NEUTRAL) / 20.0f : 0.0f;

    for (uint8_t i = 0; i < count; i++) {

        particles[i].y -= particles[i].speed;
        particles[i].x += xDrift;

        if (particles[i].y < -1 || particles[i].x < -1 || particles[i].x > MATRIX_WIDTH) {
            particles[i].y = static_cast<float>(MATRIX_HEIGHT + random8(15));
            particles[i].x = random8(MATRIX_WIDTH);
            particles[i].speed = static_cast<float>(random8(minSpeed, maxSpeed) / 100.0);
        }

        const int16_t x = static_cast<int16_t>(particles[i].x);
        const int16_t y = static_cast<int16_t>(particles[i].y);

        if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
            if (const auto index = static_cast<int16_t>(xyToIndex(x, y)); index < MATRIX_LEDS) {
                leds[index] = CRGB(color);
            }
        }
    }
}
