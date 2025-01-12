#include "draw.h"
#include "colors.h"

static void snakeFast();
static void snakeSlow();
static void fullFill();
static void snakeChameleon();
static void breathingLight();

void draw(uint8_t subMode) {
  switch (subMode) {
    case 1:
      snakeFast();
      break;

    case 2:
      snakeSlow();
      break;

    case 3:
      fullFill();
      break;

    case 4:
      snakeChameleon();
      break;

    case 5:
      breathingLight();
      break;

    default:
      break;
  }
}

void snakeFast() {
  static unsigned int ledsCount = 0;
  static uint8_t color = 0;

  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    delay(6);
    ledsCount++;
  } else {
    if (++color >= 128) {
      color = 0;
    }
    ledsCount = 0;
  }
}

void snakeSlow() {
  static unsigned int ledsCount = 0;
  static uint8_t color = 0;

  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    delay(25);
    ledsCount++;
  } else {
    if (++color >= 128) {
      color = 0;
    }
    ledsCount = 0;
  }
}

void fullFill() {
  const int maxDistance = max(6, 6);
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  static uint8_t color = 0;

  if (currentMillis - previousMillis >= 5000) {
    previousMillis = currentMillis;

    for (int distance = 0; distance <= maxDistance; distance++) {
      for (int x = 0; x < 12; x++) {
        for (int y = 0; y < 13; y++) {
          int dx = abs(x - 6);
          int dy = abs(y - 6);
          if (dx <= distance && dy <= distance) {
            strip.setPixelColor(XY(x, y), pgm_read_dword(&(mainColors[color])));
          }
        }
      }
      strip.show();
      delay(50);
    }
    if (++color >= 128) {
      color = 0;
    }
  }
}

void snakeChameleon() {
  const int chameleon[] = {
      3,  20, 5, 10, 5, 1,  3, 6,  2, 8, 7, 2,
      15, 8,  3, 7,  4, 10, 2, 15, 6, 8, 2, 4};  // длина массива 24

  // int chameleon [] = {3, 20, 5, 10, 5, 1, 3, 6, 2, 8, 7, 2, 15, 8, 3, 7, 4,
  // 10, 2, 15, 6, 8, 2, 4, 3, 1, 10, 13, 5, 20, 1, 14, 7, 9, 3, 11, 2, 1}; //
  // длина массива 38 для 16х16

  static unsigned long previousMillis = 0;

  static unsigned char pos = 0;
  static unsigned char arrPos = 0;
  static int arrVolume = 0;
  static int n = 0;

  static uint8_t color = 0;

  unsigned long currentMillisD3 = millis();

  if (currentMillisD3 - previousMillis >= 70) {
    previousMillis = currentMillisD3;

    if (n <= (pos + chameleon[arrPos])) {
      strip.setPixelColor(n, pgm_read_dword(&(mainColors[color])));
      strip.show();
      n++;
    } else {
      pos = (pos + chameleon[arrPos]) + 1;
      arrPos++;
      color = rand() % 128;
      n = pos;
      arrVolume++;
    }

    if (arrVolume > 23) {
      arrVolume = 0;
      arrPos = 0;
      pos = 0;
      n = 0;
      color = rand() % 128;
    }
  }
}

void breathingLight() {
  static uint8_t color = 0;
  static int brightness = 0;
  static int step = 0;

  if (brightness < 255 && step == 0) {
    brightness++;
    if (brightness == 255) {
      step = 1;
    }
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(
          i,
          strip.Color(
              (uint8_t)((pgm_read_dword(&(mainColors[color])) >> 16) & 0xFF) *
                  brightness / 255,
              (uint8_t)((pgm_read_dword(&(mainColors[color])) >> 8) & 0xFF) *
                  brightness / 255,
              (uint8_t)(pgm_read_dword(&(mainColors[color])) & 0xFF) *
                  brightness / 255));
    }
    strip.show();
    delay(10);
  } else if (brightness > 0 && step == 1) {
    brightness--;
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(
          i,
          strip.Color(
              (uint8_t)((pgm_read_dword(&(mainColors[color])) >> 16) & 0xFF) *
                  brightness / 255,
              (uint8_t)((pgm_read_dword(&(mainColors[color])) >> 8) & 0xFF) *
                  brightness / 255,
              (uint8_t)(pgm_read_dword(&(mainColors[color])) & 0xFF) *
                  brightness / 255));
    }
    strip.show();
    delay(10);

    if (brightness == 0) {
      step = 0;
      color = rand() % 128;
    }
  }
}
