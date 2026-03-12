#include "modes.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (spaceSettings):
    1) Скорости эффектов
*/

enum spaceSettings
{
  STAR_SKY_DELAY = 0, // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  drawPulsar_DELAY = 50,
  drawStarFall_DELAY = 100,
  GALAXY_DELAY = 130
};

static void drawStarSky();
static void drawSpaceship();
static void drawStarFall();
static void drawPulsar();
static void drawSpiralGalaxy();

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
    drawPulsar();
    break;

  case 4:
    drawStarFall();
    break;

  case 5:
    drawSpiralGalaxy();
    break;

  default:
    break;
  }
}

void showStar(uint8_t starNum, uint32_t color)
{
  strip.setPixelColor(starNum, color);
  strip.show();
}

void hideStar(uint8_t starNum)
{
  strip.setPixelColor(starNum, strip.Color(0, 0, 0));
  strip.show();
}

void drawStarSky()
{
  const uint32_t color[] = {
      strip.Color(149, 167, 232), strip.Color(224, 232, 255),
      strip.Color(255, 159, 19), strip.Color(252, 127, 20),
      strip.Color(242, 95, 106), strip.Color(199, 0, 17),
      strip.Color(56, 112, 255)};

  static uint8_t starCycle = 0;

  if (starCycle < 60)
  {
    uint8_t starNum = ESP8266TrueRandom.random(0, 256);
    uint8_t colorNum = ESP8266TrueRandom.random(0, 7);

    showStar(starNum, strip.Color(255, 240, 245));
    delay(500);

    starNum = ESP8266TrueRandom.random(0, 256);
    showStar(starNum, color[colorNum]);
    delay(1100);

    for (uint8_t i = 0; i < 6; i++)
    {
      starNum = ESP8266TrueRandom.random(0, 256);
      hideStar(starNum);
      delay(10);
    }

    starCycle++;
  }
  else
  {
    uint8_t starNum = ESP8266TrueRandom.random(0, 256);
    showStar(starNum, strip.Color(255, 0, 0));
    delay(1000);

    starNum = ESP8266TrueRandom.random(0, 256);
    showStar(starNum, strip.Color(0, 0, 255));
    starCycle = 0;
  }
}

uint16_t XY7(uint8_t x, uint8_t y) { return (y * MATRIX_WIDTH + x); }

byte dir7 = 3;

static void MoveX(int8_t delta)
{
  if (delta)
  {
    if (delta > 0)
    {
      for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
      {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
        {
          leds[XY7(x, y)] = leds[XY7(x + delta, y)];
        }
      }
    }
    else
    {
      for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
      {
        for (uint8_t x = MATRIX_WIDTH - 1; x > 0; x--)
        {
          leds[XY7(x, y)] = leds[XY7(x + delta, y)];
        }
      }
    }
  }
}

static void MoveY(int8_t delta)
{
  if (delta)
  {
    if (delta > 0)
    {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
      {
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
        {
          leds[XY7(x, y)] = leds[XY7(x, y + delta)];
        }
      }
    }
    else
    {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
      {
        for (uint8_t y = MATRIX_HEIGHT - 1; y > 0; y--)
        {
          leds[XY7(x, y)] = leds[XY7(x, y + delta)];
        }
      }
    }
  }
}

static void drawPixel(byte x, byte y, CRGB color)
{
  leds[XY7(x, y)] += color;
  if (MATRIX_WIDTH > 24 || MATRIX_HEIGHT > 24)
  {
    leds[XY7(x + 1, y)] += color;
    leds[XY7(x - 1, y)] += color;
    leds[XY7(x, y + 1)] += color;
    leds[XY7(x, y - 1)] += color;
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
    byte x = beatsin8(12 + i, 2, MATRIX_WIDTH - 3);
    byte y = beatsin8(15 + i, 2, MATRIX_HEIGHT - 3);
    drawPixel(x, y,
              ColorFromPalette(RainbowColors_p, beatsin8(12 + i, 0, 255), 255));
  }
  XYMap xyMap(MATRIX_WIDTH, MATRIX_HEIGHT);
  blur2d(leds, MATRIX_WIDTH, MATRIX_HEIGHT, 32, xyMap);
  FastLED.show();
  EVERY_N_SECONDS(5)
  {
    if (dir7 == 7)
      dir7 = 0;
    else
      dir7++;
  }
}

static void drawVortex(int16_t radius)
{
  const int16_t centerX = 6;
  const int16_t centerY = 6;

  static int16_t rotationAngle = 0;

  strip.clear();
  for (int16_t angle = 0; angle < 360; angle += 10)
  {
    float rad = radians(angle + rotationAngle);
    int16_t x = centerX + radius * cos(rad);
    int16_t y = centerY + radius * sin(rad);

    if (x >= 0 && x < 12 && y >= 0 && y < 13)
    {
      strip.setPixelColor(XY(x, y), Wheel((angle + radius) & 255));
    }
  }
  strip.show();
  rotationAngle = (rotationAngle + 5) % 360;
}

static void drawPulsar()
{
  const int16_t centerX = 6;
  const int16_t centerY = 6;
  const int16_t maxRadius = max(centerX, centerY);
  for (uint16_t radius = 0; radius <= maxRadius; radius++)
  {
    drawVortex(radius);
    delay(drawPulsar_DELAY);
  }

  for (int16_t radius = maxRadius - 1; radius >= 0; radius--)
  {
    drawVortex(radius);
    delay(drawPulsar_DELAY * 2);
  }
}

void drawStarFall()
{
  static uint8_t currentCol[8] = {11, 7, 3, 0, 5, 10, 6, 4};
  static uint8_t currentRow[8] = {11, 7, 3, 0, 5, 10, 6, 4};
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

  for (uint8_t i = 0; i < 8; ++i)
  {
    drawLine(currentCol[i], currentRow[i], lineLength, false);
  }
  FastLED.show();
  delay(drawStarFall_DELAY);

  for (uint8_t i = 0; i < 8; ++i)
  {
    drawLine(currentCol[i], currentRow[i], lineLength, true);
  }
  FastLED.show();

  for (uint8_t i = 0; i < 8; ++i)
  {
    if (currentRow[i] > 0)
    {
      --currentRow[i];
      --currentCol[i];
    }
    else
    {
      currentCol[i] = MATRIX_WIDTH + 4 + ESP8266TrueRandom.random(0, 8);
      currentRow[i] = MATRIX_WIDTH + 6 + ESP8266TrueRandom.random(0, 8);
    }
  }
}

void drawSpiralGalaxy()
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