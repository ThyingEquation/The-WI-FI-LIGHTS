#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (colorfulWavesSettings):
    1) Частота кадров волны (задержка)
*/

enum colorfulWavesSettings
{
  WAVES_DELAY = 20
};

static void drawSinusoidWaves(uint8_t sinNum);
static void drawDiagonalWaves();

void drawColorfulWaves(uint8_t subMode)
{
  switch (subMode)
  {
  case 1:
    drawSinusoidWaves(0);
    delay(WAVES_DELAY);
    FastLED.show();
    break;

  case 2:
    drawSinusoidWaves(1);
    delay(WAVES_DELAY);
    FastLED.show();
    break;

  case 3:
    drawSinusoidWaves(2);
    delay(WAVES_DELAY);
    FastLED.show();
    break;

  case 4:
    drawSinusoidWaves(3);
    delay(WAVES_DELAY);
    FastLED.show();
    break;

  case 5:
    drawDiagonalWaves();
    break;

  default:
    break;
  }
}

static float fmap(const float x, const float inMin, const float inMax,
                  const float outMin, const float outMax)
{
  return (outMax - outMin) * (x - inMin) / (inMax - inMin) + outMin;
}

static void drawSinusoidWaves(uint8_t sinNum)
{
  const uint8_t semikMatrixHeightMajor = MATRIX_HEIGHT / 2 + (MATRIX_HEIGHT % 2);
  const uint8_t semikMatrixWidthMajor = MATRIX_WIDTH / 2 + (MATRIX_WIDTH % 2);
  const float eS3Speed = 0.004 * 15 + 0.015;
  const float eS3Size = fmap(255, 1, 255, 3, 9);
  const uint8_t scale = map8(55, 50, 150);
  const float timeShift = millis();

  auto calculateColor = [&](uint8_t x, uint8_t y, float freq1, float freq2,
                            float timeMult = 1.0f) -> uint8_t
  {
    float cx =
        (y - semikMatrixHeightMajor) +
        float(eS3Size * sin16(eS3Speed * freq1 * timeShift * timeMult)) /
            32767.0;
    float cy =
        (x - semikMatrixWidthMajor) +
        float(eS3Size * cos16(eS3Speed * freq2 * timeShift * timeMult)) /
            32767.0;
    return static_cast<uint8_t>(
        127 * (1 + sin16(127 * scale * sqrt(cx * cx + cy * cy)) / 32767.0));
  };

  for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
  {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
    {
      switch (sinNum)
      {
      case 0:
        leds[XY(x, y)].r = 255 - calculateColor(x, y, 98.301, 72.0874);
        leds[XY(x, y)].b = 255 - calculateColor(x, y, 134.3447, 170.3884);
        break;

      case 1:
      {
        CRGB color;
        color.r = 255 - calculateColor(x, y, 98.301, 72.0874);
        color.g = 255 - calculateColor(x, y, 68.8107, 65.534);
        color.b = 255 - calculateColor(x, y, 134.3447, 170.3884);
        leds[XY(x, y)] = color;
        break;
      }

      case 2:
        leds[XY(x, y)].r = 255 - calculateColor(x, y, 98.301, 72.0874);
        leds[XY(x, y)].g = 255 - calculateColor(x, y, 68.8107, 65.534);
        leds[XY(x, y)].b = 255 - calculateColor(x, y, 134.3447, 170.3884);
        break;

      case 3:
        leds[XY(x, y)].r = 255 - calculateColor(x, y, 98.301, 72.0874, 1.0f);
        leds[XY(x, y)].g = 255 - calculateColor(x, y, 68.8107, 65.534, 1.0f);
        leds[XY(x, y)].b = 255 - calculateColor(x, y, 0, 0, 1.0f);
        break;

      default:
        break;
      }
    }
  }
}

static void drawDiagonalWaves()
{
  static uint8_t hue = 0;

  for (uint8_t x = 0; x < MATRIX_WIDTH; x++)
  {
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++)
    {
      uint16_t index = XY(x, y);
      uint8_t brightness = sin8(x * 8 + y * 8 + hue);
      leds[index] = CHSV(hue, 255, brightness);
    }
  }
  hue++;
  FastLED.show();
  FastLED.delay(10);
}