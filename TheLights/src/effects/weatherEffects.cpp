#include "effects.h"

/*
  Эта группа эффектов только для матрицы 12х12
  Настраиваемые параметры: нет
*/

struct Particle {
  float x, y;
  float speed;
};

Particle particles[25];

void initWeather();
void drawWeather(uint8_t count, uint32_t color, uint8_t minSpeed, uint8_t maxSpeed, uint8_t drift, uint8_t trail);

void drawWeatherEffects(uint8_t subMode) {

  if (checkCommandReceived()) {
    initWeather();
  }

  static uint32_t lastUpdate = 0;

  if (millis() - lastUpdate < 30) return; 
  lastUpdate = millis();

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
  }
  stripShow();
}

void initWeather() {
  for (uint8_t i = 0; i < 25; i++) {
    particles[i].y = -10; 
  }
  fill_solid(leds, MATRIX_LEDS, CRGB::Black);
  stripShow();
}

void drawWeather(uint8_t count, uint32_t color, uint8_t minSpeed, uint8_t maxSpeed, uint8_t drift, uint8_t trail) {

  if (count > 25) count = 25;

  fadeToBlackBy(leds, MATRIX_LEDS, trail);

  float xDrift = (drift != 10) ? (float)(drift - 10) / 20.0 : 0.0;

  for (uint8_t i = 0; i < count; i++) {

    particles[i].y -= particles[i].speed;
    particles[i].x += xDrift;

    if (particles[i].y < -1 || particles[i].x < -1 || particles[i].x > MATRIX_WIDTH) {
      particles[i].y = MATRIX_HEIGHT + random8(15); 
      particles[i].x = random8(MATRIX_WIDTH);
      particles[i].speed = (float)random8(minSpeed, maxSpeed) / 100.0;
    }

    int16_t x = (int16_t)particles[i].x;
    int16_t y = (int16_t)particles[i].y;

    if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
      int16_t index = XY(x, y);
      if (index >= 0 && index < MATRIX_LEDS) {
        leds[index] = CRGB(color);
      }
    }
  }
}