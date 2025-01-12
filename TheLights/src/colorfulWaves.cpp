#include "colorfulWaves.h"

static void sinusoidWaves(int sinNum);
static void diagonalWaves();

void colorfulWaves(uint8_t subMode) {
  switch (subMode) {
    case 1:
      sinusoidWaves(0);
      delay(20);
      FastLED.show();
      break;

    case 2:
      sinusoidWaves(1);
      delay(20);
      FastLED.show();
      break;

    case 3:
      sinusoidWaves(2);
      delay(20);
      FastLED.show();
      break;

    case 4:
      sinusoidWaves(3);
      delay(20);
      FastLED.show();
      break;

    case 5:
      diagonalWaves();
      break;

    default:
      break;
  }
}

static float fmap(const float x, const float in_min, const float in_max,
                  const float out_min, const float out_max) {
  return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

void sinusoidWaves(int sinNum) {
  const uint8_t semikMatrixHeightMajor = mHeight / 2 + (mHeight % 2);
  const uint8_t semikMatrixWidthMajor = mWidth / 2 + (mWidth % 2);
  const float e_s3_speed = 0.004 * 15 + 0.015;
  const float e_s3_size = fmap(255, 1, 255, 3, 9);
  const uint8_t _scale = map8(55, 50, 150);
  const float time_shift = millis();

  auto calculateColor = [&](uint8_t x, uint8_t y, float freq1, float freq2,
                            float timeMult = 1.0f) -> uint8_t {
    float cx =
        (y - semikMatrixHeightMajor) +
        float(e_s3_size * sin16(e_s3_speed * freq1 * time_shift * timeMult)) /
            32767.0;
    float cy =
        (x - semikMatrixWidthMajor) +
        float(e_s3_size * cos16(e_s3_speed * freq2 * time_shift * timeMult)) /
            32767.0;
    return static_cast<uint8_t>(
        127 * (1 + sin16(127 * _scale * sqrt(cx * cx + cy * cy)) / 32767.0));
  };

  for (uint8_t y = 0; y < mHeight; y++) {
    for (uint8_t x = 0; x < mWidth; x++) {
      switch (sinNum) {
        case 0:
          leds[XY(x, y)].r = 255 - calculateColor(x, y, 98.301, 72.0874);
          leds[XY(x, y)].b = 255 - calculateColor(x, y, 134.3447, 170.3884);
          break;

        case 1: {
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

void diagonalWaves() {
  static uint8_t hue = 0;
  for (int x = 0; x < 12; x++) {
    for (int y = 0; y < 13; y++) {
      int index = XY(x, y);
      uint8_t brightness = sin8(x * 8 + y * 8 + hue);
      leds[index] = CHSV(hue, 255, brightness);
    }
  }
  hue++;
  FastLED.show();
  FastLED.delay(10);
}
