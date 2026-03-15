#include "modes.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (spaceSettings):
    1) Скорости эффектов
*/

enum spaceSettings
{
  STAR_SKY_DELAY = 1500,
  PULSATING_STAR_DELAY = 500,
  STARFALL_DELAY = 100,
  GALAXY_DELAY = 130
};

static void drawStarSky();
static void drawSpaceship();
static void drawStarFall();
static void drawPulsatingStar();
static void drawSpiralNebula();

void drawSpaceEffects(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawStarSky();
    break;

  case 2:
    drawSpaceship();
    break;

  case 3:
    drawPulsatingStar();
    break;

  case 4:
    drawStarFall();
    break;

  case 5:
    drawSpiralNebula();
    break;

  default:
    break;
  }
}

static void addStar(uint8_t starNum, uint32_t color)
{
  strip.setPixelColor(starNum, color);
  strip.show();
}

static void removeStar(uint8_t starNum)
{
  strip.setPixelColor(starNum, strip.Color(0, 0, 0));
  strip.show();
}

const uint32_t starColors[] = { // Основные цвета звездного неба
    strip.Color(149, 167, 232),
    strip.Color(224, 232, 255),
    strip.Color(255, 159, 19),
    strip.Color(252, 127, 20),
    strip.Color(242, 95, 106),
    strip.Color(199, 0, 17),
    strip.Color(56, 112, 255),
    strip.Color(180, 200, 255),
    strip.Color(110, 140, 220),
    strip.Color(70, 100, 190),
    strip.Color(255, 220, 180),
    strip.Color(200, 210, 230),
    strip.Color(170, 150, 220),
    strip.Color(230, 240, 255)
};
const uint8_t starColorsCount = 14;

void drawStarSky()
{
  static uint8_t starCycle = 0;
  static uint8_t step = 0;
  static uint32_t lastTime = 0;

  uint32_t currentMillis = millis();

  uint16_t neededDelay = STAR_SKY_DELAY;
  if (step >= 3 && step < 13)
  {
    neededDelay = 5;
  }

  if (currentMillis - lastTime < neededDelay)
  {
    return;
  }
  lastTime = currentMillis;

  if (starCycle < 60)
  {
    if (step == 0)
    {
      uint8_t rndColor = ESP8266TrueRandom.random(0, starColorsCount);
      uint16_t rndLed = ESP8266TrueRandom.random(0, MATRIX_LEDS);
      addStar(rndLed, starColors[rndColor]);
      step++;
    }
    else if (step == 1)
    {
      uint8_t rndColor = ESP8266TrueRandom.random(0, starColorsCount);
      uint16_t rndLed = ESP8266TrueRandom.random(0, MATRIX_LEDS);
      addStar(rndLed, starColors[rndColor]);
      step++;
    }
    else if (step >= 2 && step < 13)
    {
      uint16_t rndLed = ESP8266TrueRandom.random(0, MATRIX_LEDS);
      removeStar(rndLed);
      step++;
    }
    else
    {
      starCycle++;
      step = 0;
    }
  }
  else
  {
    uint16_t rndLed = ESP8266TrueRandom.random(0, MATRIX_LEDS);
    addStar(rndLed, strip.Color(255, 255, 255));
  }
}

uint16_t XY7(uint8_t x, uint8_t y)
{
  if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT)
    return 0;
  return (y * MATRIX_WIDTH + x);
}

byte dir7 = 3;

static void MoveX(int8_t delta)
{
  if (delta > 0)
  {
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
    {
      for (uint8_t x = 0; x < MATRIX_WIDTH - 1; x++)
      {
        leds[XY7(x, y)] = leds[XY7(x + 1, y)];
      }
      leds[XY7(MATRIX_WIDTH - 1, y)] = CRGB(0, 0, 0);
    }
  }
  else if (delta < 0)
  {
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
    {
      for (uint8_t x = MATRIX_WIDTH - 1; x > 0; x--)
      {
        leds[XY7(x, y)] = leds[XY7(x - 1, y)];
      }
      leds[XY7(0, y)] = CRGB(0, 0, 0);
    }
  }
}

static void MoveY(int8_t delta)
{
  if (delta > 0)
  {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
    {
      for (uint8_t y = MATRIX_HEIGHT - 1; y > 0; y--)
      {
        leds[XY7(x, y)] = leds[XY7(x, y - 1)];
      }
      leds[XY7(x, 0)] = CRGB(0, 0, 0);
    }
  }
  else if (delta < 0)
  {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
    {
      for (uint8_t y = 0; y < MATRIX_HEIGHT - 1; y++)
      {
        leds[XY7(x, y)] = leds[XY7(x, y + 1)];
      }
      leds[XY7(x, MATRIX_HEIGHT - 1)] = CRGB(0, 0, 0);
    }
  }
}

static void drawPixel(byte x, byte y, CRGB color)
{
  if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT)
    return;

  leds[XY7(x, y)] += color;

  if (MATRIX_WIDTH > 24 || MATRIX_HEIGHT > 24)
  {
    if (x > 0)
      leds[XY7(x - 1, y)] += color;
    if (x < MATRIX_WIDTH - 1)
      leds[XY7(x + 1, y)] += color;
    if (y > 0)
      leds[XY7(x, y - 1)] += color;
    if (y < MATRIX_HEIGHT - 1)
      leds[XY7(x, y + 1)] += color;
  }
}

void drawSpaceship()
{
  fadeToBlackBy(leds, MATRIX_LEDS, 16);

  switch (dir7)
  {
  case 0:
    MoveX(1);
    break;
  case 1:
    MoveX(1);
    MoveY(-1);
    break;
  case 2:
    MoveY(-1);
    break;
  case 3:
    MoveX(-1);
    MoveY(-1);
    break;
  case 4:
    MoveX(-1);
    break;
  case 5:
    MoveX(-1);
    MoveY(1);
    break;
  case 6:
    MoveY(1);
    break;
  case 7:
    MoveX(1);
    MoveY(1);
    break;
  }

  for (byte i = 0; i < 8; i++)
  {
    byte x = beatsin8(12 + i, 1, MATRIX_WIDTH - 2);
    byte y = beatsin8(15 + i, 1, MATRIX_HEIGHT - 2);

    drawPixel(x, y,
              ColorFromPalette(RainbowColors_p, beatsin8(12 + i, 0, 255), 255));
  }

  FastLED.show();

  EVERY_N_SECONDS(5)
  {
    dir7++;
    if (dir7 > 7)
      dir7 = 0;
  }
}

static void drawVortex(int16_t currentRadius, int16_t maxRadius)
{
  const float centerX = (MATRIX_WIDTH - 1) / 2.0f;
  const float centerY = (MATRIX_HEIGHT - 1) / 2.0f;

  static float rotationAngle = 0;

  strip.clear();

  for (int16_t r = 0; r <= currentRadius; r++)
  {
    float progress = (maxRadius > 0) ? (float)r / maxRadius : 0.0f;

    uint8_t red = 255 * progress;
    uint8_t green = 255 * progress;
    uint8_t blue = 255 * (1.0 - progress);

    uint32_t color = strip.Color(red, green, blue);

    for (int16_t angle = 0; angle < 360; angle += 2)
    {
      float rad = radians(angle + rotationAngle);

      int16_t x = round(centerX + r * cos(rad));
      int16_t y = round(centerY + r * sin(rad));

      if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT)
      {
        strip.setPixelColor(XY(x, y), color);
      }
    }
  }

  strip.show();
  rotationAngle += 2.5f;
  if (rotationAngle >= 360)
    rotationAngle -= 360;
}

static void drawPulsatingStar()
{
  static const int16_t maxRadius = min((MATRIX_WIDTH - 1) / 2, (MATRIX_HEIGHT - 1) / 2);

  static int16_t currentRadius = 0;
  static int8_t direction = 1;
  static uint32_t lastUpdateTime = 0;

  uint32_t currentMillis = millis();

  if (currentMillis - lastUpdateTime >= PULSATING_STAR_DELAY)
  {
    lastUpdateTime = currentMillis;
    drawVortex(currentRadius, maxRadius);
    currentRadius += direction;

    if (currentRadius >= maxRadius)
    {
      direction = -1;
    }
    else if (currentRadius <= 0)
    {
      direction = 1;
    }
  }
}

void drawStarFall()
{
  static uint8_t currentCol[4] = {11, 7, 3, 0};
  static uint8_t currentRow[4] = {11, 7, 3, 0};
  const uint8_t lineLength = 7;

  auto drawLine = [](uint8_t col, uint8_t row, uint8_t length, bool clear)
  {
    for (uint8_t j = 0; j < length; ++j)
    {
      uint8_t rowIndex = row - j;
      uint8_t colIndex = col - j;
      if (rowIndex >= 0 && rowIndex < MATRIX_HEIGHT && colIndex >= 0 &&
          colIndex < MATRIX_WIDTH)
      {
        uint16_t pixelIndex = XY(colIndex, rowIndex);
        if (clear)
        {
          leds[pixelIndex] = CRGB::Black;
        }
        else
        {
          uint8_t brightness = (j == length - 1) ? 255 : 5 * j;
          leds[pixelIndex] = hsv2rgb_spectrum(CHSV(0, 0, brightness));
        }
      }
    }
  };

  for (uint8_t i = 0; i < 4; ++i)
  {
    drawLine(currentCol[i], currentRow[i], lineLength, false);
  }
  FastLED.show();
  delay(STARFALL_DELAY);

  for (uint8_t i = 0; i < 4; ++i)
  {
    drawLine(currentCol[i], currentRow[i], lineLength, true);
  }
  FastLED.show();

  for (uint8_t i = 0; i < 4; ++i)
  {
    if (currentRow[i] > 0)
    {
      --currentRow[i];
      --currentCol[i];
    }
    else
    {
      currentCol[i] = MATRIX_WIDTH + 4 + ESP8266TrueRandom.random(0, 10);
      currentRow[i] = MATRIX_WIDTH + 6 + ESP8266TrueRandom.random(0, 10);
    }
  }
}

void drawSpiralNebula()
{
  static bool forward = true;
  static int16_t spiralIndex = 0;

  fill_solid(leds, MATRIX_LEDS, CRGB::Black);

  int16_t x = 0, y = 0;
  int16_t dx = 0, dy = -1;
  int16_t maxI = max(MATRIX_WIDTH, MATRIX_HEIGHT) * max(MATRIX_WIDTH, MATRIX_HEIGHT);

  for (uint16_t i = 0; i < maxI; i++)
  {
    if (x >= -MATRIX_WIDTH / 2 && x < MATRIX_WIDTH / 2 && y >= -MATRIX_HEIGHT / 2 &&
        y < MATRIX_HEIGHT / 2)
    {
      uint16_t ledIndex = XY(x + MATRIX_WIDTH / 2, y + MATRIX_HEIGHT / 2);
      if (ledIndex >= 0 && ledIndex < MATRIX_LEDS)
      {
        leds[ledIndex] = CHSV((i + spiralIndex) % 256, 255, 255);
      }
    }

    if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1 - y))
    {
      uint16_t temp = dx;
      dx = -dy;
      dy = temp;
    }

    x += dx;
    y += dy;
  }
  FastLED.show();

  spiralIndex = (spiralIndex + (forward ? 1 : -1)) % 256;

  if (spiralIndex == 0 || spiralIndex == 255)
  {
    forward = !forward;
  }

  FastLED.delay(GALAXY_DELAY);
}