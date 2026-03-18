#include "effects.h"
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
  RUNNING_LIGHTS_FAST_DELAY = 20,
  COLORFUL_SNAKE_DELAY = 50
};

static void drawColorfulLight();
static void drawLights(uint8_t lightsMode);
static void drawLight(uint16_t lightDelay);
static void drawColorfulSnake();

static uint8_t color;

void drawRunningLights(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawLight(RUNNING_LIGHTS_SLOW_DELAY);
    break;

  case 2:
    drawLight(RUNNING_LIGHTS_FAST_DELAY);
    break;

  case 3:
    drawColorfulLight();
    break;

  case 4:
    drawLights(1);
    break;

  case 5:
    drawLights(2);
    break;

  case 6:
    drawColorfulSnake();
    break;

  case 7:
    drawLights(3);
    break;

  default:
    break;
  }
}

void drawColorfulLight()
{
  static uint32_t previousMillis = 0;
  static uint16_t currentLED = 0;
  static bool firstPass = true;

  if (checkCommandReceived())
  {
    previousMillis = 0;
    currentLED = 0;
    firstPass = true;
  }

  uint32_t currentMillis = millis();

  if (currentMillis - previousMillis >= 20)
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

static void drawLights(uint8_t lightsMode)
{
  static uint8_t color = 0;
  static uint8_t stepJ = 0;
  static uint8_t stepQ = 0;
  static uint32_t lastTime = 0;
  static bool isNewCycle = true;

  const int delays[] = {
      RUNNING_LIGHTS_1_DELAY,
      RUNNING_LIGHTS_2_DELAY,
      RUNNING_LIGHTS_3_DELAY};

  int currentDelay = RUNNING_LIGHTS_1_DELAY;
  if (lightsMode >= 1 && lightsMode <= 3)
  {
    currentDelay = delays[lightsMode - 1];
  }

  if (millis() - lastTime < currentDelay)
  {
    return;
  }
  lastTime = millis();

  if (isNewCycle && lightsMode == 1)
  {
    color = ESP8266TrueRandom.random(0, 128);
    isNewCycle = false;
  }

  uint8_t prevQ = (stepQ == 0) ? 2 : stepQ - 1;
  for (uint16_t i = 0; i < MATRIX_LEDS; i = i + 3)
  {
    if (i + prevQ < MATRIX_LEDS)
    {
      leds[i + prevQ] = CRGB::Black;
    }
  }

  for (uint16_t i = 0; i < MATRIX_LEDS; i = i + 3)
  {
    CRGB pixelColor;

    switch (lightsMode)
    {
    case 1:
      pixelColor = CRGB(pgm_read_dword_near(&mainColors[color]));
      break;

    case 2:
      pixelColor = Wheel((i + stepJ) % 255);
      break;

    case 3:
      pixelColor = CRGB(pgm_read_dword_near(&mainColors[ESP8266TrueRandom.random(0, 128)]));
      break;
    }

    if (i + stepQ < MATRIX_LEDS)
    {
      leds[i + stepQ] = pixelColor;
    }
  }

  FastLED.show();

  stepQ++;

  if (stepQ >= 3)
  {
    stepQ = 0;
    stepJ++;

    if (stepJ >= 2)
    {
      stepJ = 0;
      isNewCycle = true;
    }
  }
}

void drawLight(uint16_t lightDelay)
{
  static uint16_t ledsCount = 0;
  static uint32_t lastTime = 0;

  if (checkCommandReceived())
  {
    ledsCount = 0;
    lastTime = 0;
  }

  if (millis() - lastTime < lightDelay)
  {
    return;
  }

  lastTime = millis();

  if (ledsCount <= MATRIX_LEDS)
  {
    if (ledsCount > 0)
    {
      leds[ledsCount - 1] = CRGB::Black;
    }

    uint32_t rawColor = pgm_read_dword_near(&mainColors[color]);
    leds[ledsCount] = CRGB(rawColor);

    FastLED.show();

    ledsCount++;
  }
  else
  {
    ledsCount = 0;

    if (MATRIX_LEDS > 0)
    {
      leds[MATRIX_LEDS - 1] = CRGB::Black;
      FastLED.show();
    }
    color = ESP8266TrueRandom.random(0, 128);
  }
}

static void drawColorfulSnake()
{
  static uint16_t head = 0;
  static uint16_t tail = 0;
  static uint16_t pixelCounter = 0;
  static uint32_t previousMillis = 0;

  static uint16_t snake[10];

  if (checkCommandReceived())
  {
    head = 0;
    tail = 0;
    pixelCounter = 0;
    previousMillis = 0;

    memset(snake, 0, sizeof(snake));
  }

  if (millis() - previousMillis >= COLORFUL_SNAKE_DELAY)
  {
    previousMillis = millis();

    head = (head + 1) % MATRIX_LEDS;

    tail = (tail + 1) % 10;
    snake[tail] = head;

    pixelCounter++;
    if (pixelCounter >= MATRIX_LEDS)
    {
      pixelCounter = 0;
      color = ESP8266TrueRandom.random(0, 128);
    }

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    uint32_t rawColor = pgm_read_dword_near(&mainColors[color]);
    CRGB snakeColor = CRGB(rawColor);

    for (uint16_t i = 0; i < 10; i++)
    {
      if (snake[i] < MATRIX_LEDS)
      {
        leds[snake[i]] = snakeColor;
      }
    }

    FastLED.show();
  }
}