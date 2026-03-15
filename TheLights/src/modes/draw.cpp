#include "modes.h"
#include "colors.h"
#include "images.h"

/*
  Эта группа эффектов только для матриц 12х13

  Настраиваемые параметры (drawSettings):
    1) Скорость быстрой змейки
    2) Скорость медленной змейки
    3) Время удержания цвета в полной заливке
    4) Скорость цветной (хамелион) змейки
    5) Скорость цветного дыхания
    6) Скорость всех картинок подряд
*/

enum drawSettings
{
  SNAKE_FAST_DELAY = 6,
  SNAKE_SLOW_DELAY = 25,
  FULL_FILL_DELAY = 5000,
  CHAMELEON_SNAKE_DELAY = 70,
  LIGHT_BREATH_DELAY = 8,
  DRAW_IMAGES_DELAY = 5000
};

static void drawSnakeFast();
static void drawSnakeSlow();
static void fillFull();
static void drawChameleonSnake();
static void drawLightBreath();
static void drawImages(uint8_t subMode);

static uint8_t color;

void drawCanvasEffects(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawSnakeFast();
    break;

  case 2:
    drawSnakeSlow();
    break;

  case 3:
    fillFull();
    break;

  case 4:
    drawChameleonSnake();
    break;

  case 5:
    drawLightBreath();
    break;

  case 99 ... 255:
    drawImages(subMode);
    break;

  default:
    break;
  }
}

void drawSnakeFast()
{
  static uint16_t ledsCount = 0;

  if (ledsCount <= MATRIX_LEDS)
  {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    delay(SNAKE_FAST_DELAY);
    ledsCount++;
  }
  else
  {
    color = ESP8266TrueRandom.random(0, 128);
    ledsCount = 0;
  }
}

void drawSnakeSlow()
{
  static uint16_t ledsCount = 0;

  if (ledsCount <= MATRIX_LEDS)
  {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[color])));
    strip.show();
    delay(SNAKE_SLOW_DELAY);
    ledsCount++;
  }
  else
  {
    color = ESP8266TrueRandom.random(0, 128);
    ledsCount = 0;
  }
}

void fillFull()
{
  const uint16_t maxDistance = max(6, 6);
  uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;

  if (currentMillis - previousMillis >= FULL_FILL_DELAY)
  {
    previousMillis = currentMillis;

    for (uint16_t distance = 0; distance <= maxDistance; distance++)
    {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
      {
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
        {
          uint16_t dx = abs(x - 6);
          uint16_t dy = abs(y - 6);
          if (dx <= distance && dy <= distance)
          {
            strip.setPixelColor(XY(x, y), pgm_read_dword(&(mainColors[color])));
          }
        }
      }
      strip.show();
      delay(50);
    }
    color = ESP8266TrueRandom.random(0, 128);
  }
}

void drawChameleonSnake()
{
  const uint8_t chameleon[] = {
      3, 20, 5, 10, 5, 1, 3, 6, 2, 8, 7, 2, 15, 8, 3, 7, 4, 10, 2, 15, 6, 8, 2, 4};

  static uint32_t previousMillis = 0;

  static uint8_t pos = 0;
  static uint8_t arrPos = 0;
  static uint8_t arrVolume = 0;
  static uint8_t n = 0;

  uint32_t currentMillisD3 = millis();

  if (currentMillisD3 - previousMillis >= CHAMELEON_SNAKE_DELAY)
  {
    previousMillis = currentMillisD3;

    if (n <= (pos + chameleon[arrPos]))
    {
      strip.setPixelColor(n, pgm_read_dword(&(mainColors[color])));
      strip.show();
      n++;
    }
    else
    {
      pos = (pos + chameleon[arrPos]) + 1;
      arrPos++;
      color = ESP8266TrueRandom.random(0, 128);
      n = pos;
      arrVolume++;
    }

    if (arrVolume > 23)
    {
      arrVolume = 0;
      arrPos = 0;
      pos = 0;
      n = 0;
      color = ESP8266TrueRandom.random(0, 128);
    }
  }
}

void drawLightBreath()
{
  static uint8_t brightness = 0;
  static uint8_t step = 0;

  if (brightness < 255 && step == 0)
  {
    brightness++;
    if (brightness == 255)
    {
      step = 1;
    }
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
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
    delay(LIGHT_BREATH_DELAY);
  }
  else if (brightness > 0 && step == 1)
  {
    brightness--;
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
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
    delay(LIGHT_BREATH_DELAY);

    if (brightness == 0)
    {
      step = 0;
      color = ESP8266TrueRandom.random(0, 128);
    }
  }
}

static void drawImages(uint8_t subMode)
{
  const uint32_t *images[] = {pacman1, pacman2, pacman3, pacman4, pacman5,
                              mushroom, amogus, cup, pineapple, alien,
                              hummer, cat, teaCup, dino, hammerAndSickle,
                              apple, bird, rabbit, question, goldenKey,
                              star, sun, pepe};

  static uint32_t previousMillis = 0;
  static uint32_t previousMillisImgNum = 0;
  static uint8_t imageNum = 0;
  static uint8_t locImgNum = 0;

  uint32_t currentMillis = millis();

  if (subMode == 255)
  {

    if (currentMillis - previousMillis >= DRAW_IMAGES_DELAY)
    {
      previousMillis = currentMillis;

      if (imageNum >= 0 && imageNum < 23)
      {
        drawPicture(mainMatrixScheme, images[imageNum]);
        strip.show();
        ++imageNum;
      }
      else
      {
        imageNum = 0;
      }
    }
  }
  else if ((subMode > 100 && subMode < 124) && (locImgNum != subMode))
  {
    locImgNum = subMode;
    drawPicture(mainMatrixScheme, images[subMode - 101]);
    strip.show();
  }

  if (currentMillis - previousMillisImgNum >= 5000)
  {
    previousMillisImgNum = currentMillis;
    locImgNum = 0;
  }
}