#include "effects.h"

/*
  Эта группа эффектов только для матрицы 12х12

  Настраиваемые параметры: нет
*/

struct Particle {
  float x, y;
  float speed;
};

#define MAX_PARTICLES 25
Particle particles[MAX_PARTICLES];

void initWeather();
void drawWeather(uint8_t count, uint32_t color, float minSpeed, float maxSpeed, uint8_t drift, uint8_t trail);

void drawWeatherEffects(uint8_t subMode) {

  static bool isInit = false;
  if (!isInit) {
    isInit = true;
    initWeather();
  }

  static uint32_t lastUpdate = 0;

  if (millis() - lastUpdate < 30) return; 
  lastUpdate = millis();

  switch (subMode) {
    case 0:
      drawWeather(10, 0xE0E0E0, 0.05, 0.15, 10, 200); 
      break;
    case 1:
      drawWeather(18, 0xFFFFFF, 0.2, 0.5, 11, 150); 
      break;
    case 2:
      drawWeather(10, 0x007DFF, 0.3, 0.6, 10, 100); 
      break;
    case 3:
      drawWeather(20, 0x0014A8, 0.6, 1.0, 10, 80); 
      break;
  }
  FastLED.show();
}

void initWeather() {
  for (uint8_t i = 0; i < MAX_PARTICLES; i++) {
    particles[i].x = random8(MATRIX_WIDTH);
    particles[i].y = MATRIX_HEIGHT + random8(8); 
    particles[i].speed = (float)random8(5, 15) / 10.0;
  }
  FastLED.clear();
}

void drawWeather(uint8_t count, uint32_t color, float minSpeed, float maxSpeed, uint8_t drift, uint8_t trail) {
  fadeToBlackBy(leds, MATRIX_LEDS, trail);

  for (uint8_t i = 0; i < count; i++) {

    particles[i].y -= particles[i].speed;
    
    if (drift != 10) {
      particles[i].x += (float)(drift - 10) / 20.0; 
    }

    if (particles[i].y < -1 || particles[i].x < -1 || particles[i].x > MATRIX_WIDTH) {
      particles[i].y = MATRIX_HEIGHT; 
      particles[i].x = random8(MATRIX_WIDTH);
      particles[i].speed = (float)random(minSpeed * 100, maxSpeed * 100) / 100.0;
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