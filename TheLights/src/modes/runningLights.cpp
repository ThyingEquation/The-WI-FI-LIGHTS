#include "modes.h"
#include "colors.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (runningLightsSettings):
    1) Скорости эффектов
*/

enum runningLightsSettings
{
  RUNNING_COLORFUL_LIGHT_DELAY = 35,
  RUNNING_LIGHTS_1_DELAY = 200,
  RUNNING_LIGHTS_2_DELAY = 200,
  RUNNING_LIGHTS_3_DELAY = 200,
  RUNNING_LIGHTS_SLOW_DELAY = 100,
  RUNNING_LIGHTS_FAST_DELAY = 10,
  COLORFUL_SNAKE_DELAY = 50
};

static void drawRunningColorfulLight();
static void drawRunningLights1();
static void drawRunningLights2();
static void drawRunningLights3();
static void drawRunningLightSlow();
static void drawRunningLightFast();
static void drawColorfulSnake();

static uint8_t color;

void drawRunningLights(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawRunningLightSlow();
    break;

  case 2:
    drawRunningLightFast();
    break;

  case 3:
    drawRunningColorfulLight();
    break;

  case 4:
    drawRunningLights1();
    break;

  case 5:
    drawRunningLights2();
    break;

  case 6:
    drawColorfulSnake();
    break;

  case 7:
    drawRunningLights3();
    break;

  default:
    break;
  }
}

void drawRunningColorfulLight()
{
  static uint32_t previousMillis = 0;
  static const uint32_t interval = 20;
  static uint16_t currentLED = 0;
  static bool firstPass = true;

  uint32_t currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    if (firstPass)
    {
      leds[currentLED] = CHSV(currentLED, 255, 255);
    }
    else
    {
      leds[currentLED] = CHSV(ESP8266TrueRandom.random(0, 256), 255, 255);
    }

    FastLED.show();
    delay(RUNNING_COLORFUL_LIGHT_DELAY);
    leds[currentLED] = CRGB::Black;

    currentLED++;
    if (currentLED >= MATRIX_LEDS)
    {
      currentLED = 0;
      if (firstPass)
      {
        firstPass = false;
      }
      else
      {
        firstPass = true;
      }
    }
  }
}

void drawRunningLights1()
{
  color = ESP8266TrueRandom.random(0, 128);

  for (uint8_t j = 0; j < 2; j++)
  {
    for (uint8_t q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, pgm_read_dword(&(mainColors[color])));
      }
      strip.show();

      delay(RUNNING_LIGHTS_1_DELAY);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0);
      }
    }
  }

  color = ESP8266TrueRandom.random(0, 128);
}

void drawRunningLights2()
{
  for (uint8_t j = 0; j < 2; j++)
  {
    for (uint8_t q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, Wheel((i + j) % 255));
      }
      strip.show();

      delay(RUNNING_LIGHTS_2_DELAY);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

void drawRunningLights3()
{
  for (uint8_t j = 0; j < 2; j++)
  {
    for (uint8_t q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(0, 128)])));
      }
      strip.show();

      delay(RUNNING_LIGHTS_3_DELAY);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

void drawRunningLightSlow()
{
  static uint16_t ledsCount = 0;

  if (ledsCount <= MATRIX_LEDS)
  {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    strip.setPixelColor(ledsCount - 1, strip.Color(0, 0, 0));
    strip.show();
    delay(RUNNING_LIGHTS_SLOW_DELAY);
    ledsCount++;
  }
  else
  {
    ledsCount = 0;
    color = ESP8266TrueRandom.random(0, 128);
  }
}

void drawRunningLightFast()
{
  static uint16_t ledsCount = 0;

  if (ledsCount <= MATRIX_LEDS)
  {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    strip.setPixelColor(ledsCount - 1, strip.Color(0, 0, 0));
    strip.show();
    delay(RUNNING_LIGHTS_FAST_DELAY);
    ledsCount++;
  }
  else
  {
    ledsCount = 0;
    color = ESP8266TrueRandom.random(0, 128);
  }
}

void drawColorfulSnake()
{
  static uint16_t head = 0;
  static uint16_t tail = 0;
  static uint16_t pixelCounter = 0;
  static uint32_t previousMillis = 0;

  static uint16_t snake[10];

  uint32_t currentMillis = millis();
  if (currentMillis - previousMillis >= COLORFUL_SNAKE_DELAY)
  {
    previousMillis = currentMillis;
    head = (head + 1) % strip.numPixels();
    tail = (tail + 1) % 10;
    snake[tail] = head;
    pixelCounter++;
    if (pixelCounter >= MATRIX_LEDS)
    {
      pixelCounter = 0;
      color = ESP8266TrueRandom.random(0, 128);
    }
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    for (uint16_t i = 0; i < 10; i++)
    {
      strip.setPixelColor(snake[i], pgm_read_dword(&(mainColors[color])));
    }
    strip.show();
  }
}