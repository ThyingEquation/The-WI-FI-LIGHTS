#include <deque>

#include "effects.h"
#include "colors.h"
#include "images.h"

/*
  Эта группа эффектов только для матриц 12х12

  Настраиваемые параметры (drawSettings):
    1) Скорости эффектов

  Отдельно. Режим "Рисовалка" работает на матрицах размера 8x8, 12х12 и 16х16
*/

enum drawSettings
{
  SNAKE_FAST_DELAY = 15,
  SNAKE_SLOW_DELAY = 50,
  FULL_FILL_DELAY = 5000,
  CHAMELEON_SNAKE_DELAY = 70,
  LIGHT_BREATH_DELAY = 20,
  DRAW_IMAGES_DELAY = 5000
};

const uint32_t colorsCanvas[] PROGMEM = {0x000000,  // Чёрный
                                    0xffffff,  // Белый
                                    0x0000ff,  // Синий
                                    0x00ff00,  // Лайм
                                    0x00bfff,  // Морозное небо
                                    0xff1493,  // Малиновый
                                    0xffff00,  // Желтый
                                    0x7fffd4,  // Аквамариновый
                                    0x00ff7f,  // Весенне-зеленый
                                    0xffa500,  // Оранжевый
                                    0xff0000,  // Красный
                                    0x9400d3,  // Фиолетово-баклажанный
                                    0xffb6c1,  // Светлорозовый
                                    0x4b0082,  // Индиго
                                    0xffd700,  // Золотой
                                    0x008000,  // Зеленый
                                    0x22262c,  // Серый
                                    0x964b00,  // Коричневый
                                    0x6600ff,  // Персидский синий
                                    0xd76e00,  // Темно-оранжевый
                                    0x7cfc00}; // Зеленая лужайка

static const uint32_t* const images[] = {pacman1, pacman2, pacman3, pacman4, pacman5,
                            mushroom, amogus, cup, pineapple, alien,
                            hummer, cat, teaCup, dino, hammerAndSickle,
                            apple, bird, rabbit, question, goldenKey,
                            star, sun, pepe, pokeball, microsoft, battery,
                            redHeart, thundercloud};

static void drawSnake(uint16_t delay);
static void fillFull();
static void drawChameleonSnake();
static void drawLightBreath();
static void drawImages(uint8_t subMode);

void drawOnCanvas(std::string_view mode, uint8_t color, uint16_t ledNum)
{
  if (mode == "A")
  {
    if (ledNum == 257)
    {
      uint32_t rawColor = pgm_read_dword_near(&colorsCanvas[color]);
      fill_solid(leds, MATRIX_LEDS, CRGB(rawColor));
      stripShow();
    }
    else
    {
      uint16_t index = --ledNum;
      if (index < MATRIX_LEDS) {
        uint32_t rawColor = pgm_read_dword_near(&colorsCanvas[color]);
        leds[index] = CRGB(rawColor);
        stripShow();
      }
    }
  }
  else if (mode == "B")
  {
    if (ledNum == 257)
    {
      fill_solid(leds, MATRIX_LEDS, CRGB::Black);
      stripShow();
    }
    else
    {
      uint16_t index = --ledNum;
      if (index < MATRIX_LEDS) {
        leds[index] = CRGB::Black;
        stripShow();
      }
    }
  }
}

void drawCanvasEffects(uint8_t subMode)
{
  switch (subMode)
  {
  case 0:
    fillFull();
    break;

  case 1:
    drawSnake(SNAKE_FAST_DELAY);
    break;

  case 2:
    drawSnake(SNAKE_SLOW_DELAY);
    break;

  case 3:
    drawChameleonSnake();
    break;

  case 4:
    drawLightBreath();
    break;

  case 99 ... 255:
    drawImages(subMode);
    break;

  default:
    break;
  }
}

static void drawSnake(uint16_t delay)
{
  static uint8_t color = 0;
  static uint16_t ledsCount = 0;
  static uint32_t previousMillis = 0;

  if (checkCommandReceived())
  {
    color = 0;
    ledsCount = 0;
    previousMillis = 0;
  }

  if (millis() - previousMillis < delay)
  {
    return;
  }

  previousMillis = millis();

  if (ledsCount >= MATRIX_LEDS)
  {
    color = ESP8266TrueRandom.random(0, 128);
    ledsCount = 0;
  }

  CRGB newColor = pgm_read_dword(&mainColors[color]);
  leds[ledsCount] = newColor;

  stripShow();
  ledsCount++;
}

static void fillFull()
{
  static uint8_t color = 0;
  static bool isPaused = true;
  static uint16_t currentDistance = 0;
  static uint32_t lastUpdateTime = 0;
  static uint32_t pauseStartTime = 0;

  if (checkCommandReceived())
  {
    color = 0;
    isPaused = true;
    currentDistance = 0;
    lastUpdateTime = 0;
    pauseStartTime = 0;
  }

  if (isPaused)
  {
    if (millis() - pauseStartTime >= FULL_FILL_DELAY)
    {
      isPaused = false;
      currentDistance = 0;
      color = ESP8266TrueRandom.random(0, 128);
    }
    return;
  }

  if (millis() - lastUpdateTime < 65)
  {
    return;
  }
  lastUpdateTime = millis();
  fill_solid(leds, MATRIX_LEDS, CRGB::Black);

  uint32_t rawColor = pgm_read_dword_near(&mainColors[color]);
  CRGB frameColor = CRGB(rawColor);

  for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
  {
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
    {
      uint16_t dx = abs(x - 6);
      uint16_t dy = abs(y - 6);
      if (dx <= currentDistance && dy <= currentDistance)
      {
        leds[XY(x, y)] = frameColor;
      }
    }
  }
  stripShow();

  currentDistance++;

  if (currentDistance > 6)
  {
    isPaused = true;
    pauseStartTime = millis();
  }
}

static void drawChameleonSnake()
{
  const uint8_t chameleon[] = {3, 20, 5, 10, 5, 1, 3, 6, 2, 8, 7, 2, 15, 8, 3, 7, 4, 10, 2, 15, 6, 2};

  static uint8_t arrPos = 0;
  static uint8_t arrVolume = 0;
  static uint8_t color = 0;
  static uint16_t pos = 0;
  static uint16_t n = 0;
  static uint32_t previousMillis = 0;

  if (checkCommandReceived())
  {
    arrPos = 0;
    arrVolume = 0;
    color = 0;
    pos = 0;
    n = 0;
    previousMillis = 0;
  }

  if (millis() - previousMillis < CHAMELEON_SNAKE_DELAY)
  {
    return;
  }
  previousMillis = millis();

  if (n >= MATRIX_LEDS)
  {
    arrVolume = 0;
    arrPos = 0;
    pos = 0;
    n = 0;
    color = ESP8266TrueRandom.random(0, 128);
    return;
  }

  if (n <= (pos + chameleon[arrPos]))
  {
    uint32_t rawColor = pgm_read_dword_near(&mainColors[color]);
    leds[n] = CRGB(rawColor);
    stripShow();
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

static void drawLightBreath()
{
  static uint8_t color = 0;
  static uint8_t brightness = 0;
  static int8_t direction = 1;
  static uint32_t lastTime = 0;

  if (checkCommandReceived())
  {
    color = 0;
    brightness = 0;
    direction = 1;
    lastTime = 0;
  }

  if (millis() - lastTime < LIGHT_BREATH_DELAY) return;
  lastTime = millis();

  brightness += direction;
  uint32_t rawColor = pgm_read_dword_near(&mainColors[color]);
  CRGB baseColor = CRGB(rawColor);
  baseColor.nscale8_video(brightness);
  fill_solid(leds, MATRIX_LEDS, baseColor);

  stripShow();

  if (brightness >= 255)
  {
    direction = -1;
  }
  else if (brightness == 0)
  {
    direction = 1;
    color = ESP8266TrueRandom.random(0, 128);
  }
}

static void drawImages(uint8_t subMode)
{
  static uint8_t imageNum = 0;
  static uint8_t locImgNum = 0;
  static uint32_t previousMillis = 0;
  static uint32_t previousMillisImgNum = 0;
  static bool enableNewImg = false;

  static uint8_t randomCounter = 0;
  std::deque<uint8_t> usedImg;

  uint32_t currentMillis = millis();

  if (checkCommandReceived())
  {
    imageNum = 0;
    locImgNum = 0;
    previousMillis = 0;
    previousMillisImgNum = 0;
    enableNewImg = true;
  }

  if (subMode == 254)
  {

    if (currentMillis - previousMillis >= DRAW_IMAGES_DELAY)
    {
      previousMillis = currentMillis;

      if (imageNum >= 0 && imageNum < 28)
      {
        drawPicture(mainMatrixScheme, images[imageNum]);
        stripShow();
        ++imageNum;
      }
      else
      {
        imageNum = 0;
      }
    }
  }
  else if (subMode == 255)
  {
    if (currentMillis - previousMillis >= DRAW_IMAGES_DELAY)
    {
      previousMillis = currentMillis;

      if (imageNum >= 0 && imageNum < 28)
      {
        drawPicture(mainMatrixScheme, images[imageNum]);
        stripShow();
      do
      {
        imageNum = ESP8266TrueRandom.random(0, 29);
        randomCounter++;
        if (randomCounter > 15)
          break;
      } while (std::find(usedImg.begin(), usedImg.end(), imageNum) != usedImg.end());

      usedImg.push_back(imageNum);
      if (usedImg.size() > 14)
      {
        usedImg.pop_front();
      }
      }
      else
      {
        imageNum = 0;
      }
    }
  }
  else if ((subMode > 100 && subMode < 129) && (locImgNum != subMode) && enableNewImg)
  {
    enableNewImg = false;
    locImgNum = subMode;
    drawPicture(mainMatrixScheme, images[subMode - 101]);
    stripShow();
  }

  if (currentMillis - previousMillisImgNum >= 5000)
  {
    previousMillisImgNum = currentMillis;
    locImgNum = 0;
  }
}