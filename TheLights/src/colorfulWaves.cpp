#include <colorfulWaves.h>

int sinNum = 0;

void colorfulWaves() {
  switch (choosenModeD2) {
    case 1:
      sinNum = 0;
      Sinusoid();
      delay(20);
      FastLED.show();
      break;

    case 2:
      sinNum = 1;
      Sinusoid();
      delay(20);
      FastLED.show();
      break;

    case 3:
      sinNum = 2;
      Sinusoid();
      delay(20);
      FastLED.show();
      break;

    case 4:
      sinNum = 3;
      Sinusoid();
      delay(20);
      FastLED.show();
      break;

    case 5:
      wavesExtra();
      break;

    default:
      break;
  }
}

uint8_t speed1 = 15;
uint8_t amplitude = 255;
uint8_t scale17 = 55;

static float fmap(const float x, const float in_min, const float in_max,
                  const float out_min, const float out_max) {
  return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

void Sinusoid(void) {
  const uint8_t semikMatrixHeightMajor =
      mHeight / 2 + (mHeight % 2);
  const uint8_t semikMatrixWidthMajor = mWidth / 2 + (mWidth % 2);
  float e_s3_speed = 0.004 * speed1 + 0.015;
  float e_s3_size = fmap(amplitude, 1, 255, 3, 9);
  uint8_t _scale = map8(scale17, 50, 150);
  float time_shift = millis();

  switch (sinNum) {
    case 0:
      for (uint8_t y = 0; y < mHeight; y++) {
        for (uint8_t x = 0; x < mWidth; x++) {
          float cx =
              (y - semikMatrixHeightMajor) +
              float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) /
                  32767.0;
          float cy =
              (x - semikMatrixWidthMajor) +
              float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) /
                  32767.0;
          int8_t v =
              127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          leds[XY(x, y)].r = ~v;

          cx = (y - semikMatrixHeightMajor) +
               float(e_s3_size * (sin16(e_s3_speed * 134.3447 * time_shift))) /
                   32767.0;
          cy = (x - semikMatrixWidthMajor) +
               float(e_s3_size * (cos16(e_s3_speed * 170.3884 * time_shift))) /
                   32767.0;
          v = 127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          leds[XY(x, y)].b = ~v;
        }
      }
      break;
    case 1:
      for (uint8_t y = 0; y < mHeight; y++) {
        for (uint8_t x = 0; x < mWidth; x++) {
          CRGB color;
          float cx =
              (y - semikMatrixHeightMajor) +
              float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) /
                  32767.0;
          float cy =
              (x - semikMatrixWidthMajor) +
              float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) /
                  32767.0;
          int8_t v =
              127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          color.r = ~v;

          cx = (y - semikMatrixHeightMajor) +
               float(e_s3_size * (sin16(e_s3_speed * 68.8107 * time_shift))) /
                   32767.0;
          cy = (x - semikMatrixWidthMajor) +
               float(e_s3_size * (cos16(e_s3_speed * 65.534 * time_shift))) /
                   32767.0;
          v = 127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          color.g = ~v;

          cx = (y - semikMatrixHeightMajor) +
               float(e_s3_size * (sin16(e_s3_speed * 134.3447 * time_shift))) /
                   32767.0;
          cy = (x - semikMatrixWidthMajor) +
               float(e_s3_size * (cos16(e_s3_speed * 170.3884 * time_shift))) /
                   32767.0;
          v = 127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          color.b = ~v;

          leds[XY(x, y)] = color;
        }
      }
      break;
    case 2:
      for (uint8_t y = 0; y < mHeight; y++) {
        for (uint8_t x = 0; x < mWidth; x++) {
          float cx =
              (y - semikMatrixHeightMajor) +
              float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) /
                  32767.0;
          float cy =
              (x - semikMatrixWidthMajor) +
              float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) /
                  32767.0;
          int8_t v =
              127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          leds[XY(x, y)].r = ~v;

          cx = (y - semikMatrixHeightMajor) +
               float(e_s3_size * (sin16(e_s3_speed * 68.8107 * time_shift))) /
                   32767.0;
          cy = (x - semikMatrixWidthMajor) +
               float(e_s3_size * (cos16(e_s3_speed * 65.534 * time_shift))) /
                   32767.0;
          v = 127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          leds[XY(x, y)].g = ~v;

          cx = (y - semikMatrixHeightMajor) +
               float(e_s3_size * (sin16(e_s3_speed * 134.3447 * time_shift))) /
                   32767.0;
          cy = (x - semikMatrixWidthMajor) +
               float(e_s3_size * (cos16(e_s3_speed * 170.3884 * time_shift))) /
                   32767.0;
          v = 127 * (1 + sin16(127 * _scale *
                               sqrt((((float)cx * cx) + ((float)cy * cy)))) /
                             32767.0);
          leds[XY(x, y)].b = ~v;
        }
      }
      break;
    case 3:
      for (uint8_t y = 0; y < mHeight; y++) {
        for (uint8_t x = 0; x < mWidth; x++) {
          float cx =
              (y - semikMatrixHeightMajor) +
              float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) /
                  32767.0;  // the 8 centers the middle on a 16x16
          float cy =
              (x - semikMatrixWidthMajor) +
              float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) /
                  32767.0;
          int8_t v =
              127 * (1 + sin16(127 * _scale *
                                   sqrt((((float)cx * cx) + ((float)cy * cy))) +
                               (time_shift * e_s3_speed * 100)) /
                             32767.0);
          leds[XY(x, y)].r = ~v;

          cx = (y - semikMatrixHeightMajor) +
               float(e_s3_size * (sin16(e_s3_speed * 68.8107 * time_shift))) /
                   32767.0;
          cy = (x - semikMatrixWidthMajor) +
               float(e_s3_size * (cos16(e_s3_speed * 65.534 * time_shift))) /
                   32767.0;
          v = 127 * (1 + sin16(127 * _scale *
                                   sqrt((((float)cx * cx) + ((float)cy * cy))) +
                               (time_shift * e_s3_speed * 100)) /
                             32767.0);
          leds[XY(x, y)].g = ~v;

          cx = (y - semikMatrixHeightMajor);
          cy = (x - semikMatrixWidthMajor);
          v = 127 * (1 + sin16(127 * _scale *
                                   sqrt((((float)cx * cx) + ((float)cy * cy))) +
                               (time_shift * e_s3_speed * 100)) /
                             32767.0);
          leds[XY(x, y)].b = ~v;
        }
      }
      break;
  }
}

int xyToIndexW(int x, int y) { return mainScheme[y * 12 + x]-1; }

void wavesExtra() {
  static uint8_t hue = 0;
  for (int x = 0; x < 12; x++) {
    for (int y = 0; y < 13; y++) {
      int index = xyToIndexW(x, y);
      uint8_t brightness = sin8(x * 8 + y * 8 + hue);
      leds[index] = CHSV(hue, 255, brightness);
    }
  }
  hue++;
  FastLED.show();
  FastLED.delay(10);
}
