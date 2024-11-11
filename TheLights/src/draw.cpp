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

unsigned long previousMillisD = 0;
const long intervalD = 70;  // Интервал в миллисекундах

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

    default:
      break;
  }
}

void draw1() {
  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, getStripColorByIndex(col));
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
    strip.setPixelColor(ledsCount, getStripColorByIndex(col));
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
  matrix.fillScreen(getMatrixColorByIndex(col));
  matrix.show();
  delay(5000);
  if (++col >= 128) {
    col = 0;
  }
}
void draw4() {

  static int randcolD = random(128);
  unsigned long currentMillisD = millis();

  if (currentMillisD - previousMillisD >= intervalD) {
    previousMillisD = currentMillisD;

    if (g5D <= (ilD + chameleon[chaD])) {
      strip.setPixelColor(g5D, getStripColorByIndex(randcolD));
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