#include "runningLights.h"
#include "colors.h"

static void runningColorfulLight();
static void runningLights1();
static void runningLights2();
static void runningLights3();
static void runningLightSlow();
static void runningLightFast();
static void colorfulSnake();

void runningLights(uint8_t subMode) {
  switch (subMode) {
    case 1:
      runningLightSlow();
      break;

    case 2:
      runningLightFast();
      break;

    case 3:
      runningColorfulLight();
      break;

    case 4:
      runningLights1();
      break;

    case 5:
      runningLights2();
      break;

    case 6:
      colorfulSnake();
      break;

    case 7:
      runningLights3();
      break;

    default:
      break;
  }
}

void runningColorfulLight() {
  static unsigned long previousMillis = 0;
  static const long interval = 20;
  static int currentLED = 0;
  static bool firstPass = true;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (firstPass) {
      leds[currentLED] = CHSV(currentLED, 255, 255);
    } else {
      leds[currentLED] = CHSV(rand() % 256, 255, 255);
    }

    FastLED.show();
    delay(20);
    leds[currentLED] = CRGB::Black;

    currentLED++;
    if (currentLED >= NUM_LEDS) {
      currentLED = 0;
      if (firstPass) {
        firstPass = false;
      } else {
        firstPass = true;
      }
    }
  }
}

void runningLights1() {
  static uint8_t color = rand() % 129;

  for (int j = 0; j < 2; j++) {
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, pgm_read_dword(&(mainColors[color])));
      }
      strip.show();

      delay(200);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }

  color = rand() % 129;
}

void runningLights2() {
  for (int j = 0; j < 2; j++) {
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel((i + j) % 255));
      }
      strip.show();

      delay(200);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

void runningLights3() {
  for (int j = 0; j < 2; j++) {
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, pgm_read_dword(&(mainColors[rand() % 129])));
      }
      strip.show();

      delay(200);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

void runningLightSlow() {
  static uint8_t color = rand() % 129;
  static uint16_t ledsCount = 0;

  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    strip.setPixelColor(ledsCount - 1, strip.Color(0, 0, 0));
    strip.show();
    delay(100);
    ledsCount++;
  } else {
    ledsCount = 0;
    color = rand() % 129;
  }
}

void runningLightFast() {
  static uint8_t color = rand() % 129;
  static uint16_t ledsCount = 0;

  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    strip.setPixelColor(ledsCount - 1, strip.Color(0, 0, 0));
    strip.show();
    delay(10);
    ledsCount++;
  } else {
    ledsCount = 0;
    color = rand() % 129;
  }
}

void colorfulSnake() {
  static uint16_t head = 0;
  static uint16_t tail = 0;
  static uint16_t pixelCounter = 0;
  static unsigned long previousMillis = 0;

  static uint16_t snake[10];

  static uint8_t color = rand() % 129;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 50) {
    previousMillis = currentMillis;
    head = (head + 1) % strip.numPixels();
    tail = (tail + 1) % 10;
    snake[tail] = head;
    pixelCounter++;
    if (pixelCounter >= NUM_LEDS) {
      pixelCounter = 0;
      color = rand() % 129;
    }
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    for (uint16_t i = 0; i < 10; i++) {
      strip.setPixelColor(snake[i], pgm_read_dword(&(mainColors[color])));
    }
    strip.show();
  }
}