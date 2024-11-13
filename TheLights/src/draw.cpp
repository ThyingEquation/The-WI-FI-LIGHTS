#include <draw.h>

int chameleon[] = {3, 20, 5, 10, 5,  1, 3,  6, 2, 8, 7, 2, 15,
                   8, 3,  7, 4,  10, 2, 15, 6, 8, 2, 4};  // длина массива 24
// int chameleon [] = {3, 20, 5, 10, 5, 1, 3, 6, 2, 8, 7, 2, 15, 8, 3, 7, 4, 10,
// 2, 15, 6, 8, 2, 4,
// 3, 1, 10, 13, 5, 20, 1, 14, 7, 9, 3, 11, 2, 1}; // длина массива 38 для 16х16

unsigned char ilD = 0;
unsigned char chaD = 0;
static int sD = 0;
static int g5D = 0;
int col = 0;
unsigned int ledsCount = 0;

unsigned long previousMillisD3 = 0;
unsigned long previousMillisD2 = 0;
unsigned long previousMillisD1 = 0;

const long intervalD = 70;

void draw() {
  switch (choosenModeD2) {
    case 1:
      draw1();
      break;

    case 2:
      draw2();
      break;

    case 3:
      draw3();
      break;

    case 4:
      draw4();
      break;

    case 5:
      breathingLight();
      break;

    default:
      break;
  }
}

void draw1() {
  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[col])));
    strip.show();
    delay(6);
    ledsCount++;
  } else {
    if (++col >= 128) {
      col = 0;
    }
    ledsCount = 0;
  }
}
void draw2() {
  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[col])));
    strip.show();
    delay(25);
    ledsCount++;
  } else {
    if (++col >= 128) {
      col = 0;
    }
    ledsCount = 0;
  }
}

void draw3() {
  int centerX = 6;
  int centerY = 6;
  int maxDistance = max(centerX, centerY);
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisD2 >= 5000) {
    previousMillisD2 = currentMillis;

    for (int distance = 0; distance <= maxDistance; distance++) {
      for (int x = 0; x < 12; x++) {
        for (int y = 0; y < 13; y++) {
          int dx = abs(x - centerX);
          int dy = abs(y - centerY);
          if (dx <= distance && dy <= distance) {
            strip.setPixelColor(XY(x, y), pgm_read_dword(&(mainColors[col])));
          }
        }
      }
      strip.show();
      delay(50);
    }
    if (++col >= 128) {
      col = 0;
    }
  }
}

void draw4() {
  static int randcolD = random(128);
  unsigned long currentMillisD3 = millis();

  if (currentMillisD3 - previousMillisD3 >= intervalD) {
    previousMillisD3 = currentMillisD3;

    if (g5D <= (ilD + chameleon[chaD])) {
      strip.setPixelColor(g5D, pgm_read_dword(&(mainColors[randcolD])));
      strip.show();
      g5D++;
    } else {
      ilD = (ilD + chameleon[chaD]) + 1;
      chaD++;
      randcolD = random(128);
      g5D = ilD;
      sD++;
    }

    if (sD > 23) {
      sD = 0;
      chaD = 0;
      ilD = 0;
      g5D = 0;
      randcolD = random(128);
    }
  }
}

int brightnessD = 0;
int stepD = 0;

void breathingLight() {
  if (brightnessD < 255 && stepD == 0) {
    brightnessD++;
    if (brightnessD == 255) {
      stepD = 1;
    }
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(
          i, strip.Color(
                 (uint8_t)((pgm_read_dword(&(mainColors[col])) >> 16) & 0xFF) *
                     brightnessD / 255,
                 (uint8_t)((pgm_read_dword(&(mainColors[col])) >> 8) & 0xFF) *
                     brightnessD / 255,
                 (uint8_t)(pgm_read_dword(&(mainColors[col])) & 0xFF) *
                     brightnessD / 255));
    }
    strip.show();
    delay(10);
  } else if (brightnessD > 0 && stepD == 1) {
    brightnessD--;
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(
          i, strip.Color(
                 (uint8_t)((pgm_read_dword(&(mainColors[col])) >> 16) & 0xFF) *
                     brightnessD / 255,
                 (uint8_t)((pgm_read_dword(&(mainColors[col])) >> 8) & 0xFF) *
                     brightnessD / 255,
                 (uint8_t)(pgm_read_dword(&(mainColors[col])) & 0xFF) *
                     brightnessD / 255));
    }
    strip.show();
    delay(10);

    if (brightnessD == 0) {
      stepD = 0;
      col = random(127);
    }
  }
}
