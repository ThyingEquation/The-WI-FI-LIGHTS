#include <mode11.h>

int sinNum = 0;

void mode11(String choosenMode) {

if (choosenMode.indexOf("/C1") != -1) {
sinNum = 0;
Sinusoid();
delay(20);
FastLED.show();

}

else if (choosenMode.indexOf("/C2") != -1) {
sinNum = 1;
Sinusoid();
delay(20);
FastLED.show();
}

else if (choosenMode.indexOf("/C3") != -1) {
sinNum = 2;
Sinusoid();
delay(20);
FastLED.show();
}

else if (choosenMode.indexOf("/C4") != -1) {
sinNum = 3;
Sinusoid();
delay(20);
FastLED.show();
}

else if (choosenMode.indexOf("/C5") != -1) {
sinNum = 4;
Sinusoid();
delay(20);
FastLED.show();
}

else if (choosenMode.indexOf("/C6") != -1) {
sinNum = 5;
Sinusoid();
delay(20);
FastLED.show();
}

else if (choosenMode.indexOf("/C7") != -1) {
sinNum = 6;
Sinusoid();
delay(20);
FastLED.show();
}

else if (choosenMode.indexOf("/C8") != -1) {
sinNum = 7;
Sinusoid();
delay(20);
FastLED.show();
}

else if (choosenMode.indexOf("/C9") != -1) {
sinNum = 8;
Sinusoid();
delay(20);
FastLED.show();
}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// цветные волны

#define HEIGHT 16
#define WIDTH 16
#define NUM_LEDS WIDTH * HEIGHT

uint8_t speed1 = 15; 
uint8_t amplitude = 255;
uint8_t scale17 = 55;

static float fmap(const float x, const float in_min, const float in_max, const float out_min, const float out_max) {
  return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

void Sinusoid(void) {
  const uint8_t semiHeightMajor = HEIGHT / 2 + (HEIGHT % 2);
  const uint8_t semiWidthMajor = WIDTH / 2 + (WIDTH % 2);
  float e_s3_speed = 0.004 * speed1 + 0.015; // speed of the movement along the Lissajous curves
  float e_s3_size = fmap(amplitude, 1, 255, 3, 9); // amplitude of the curves
  uint8_t _scale = map8(scale17, 50, 150);
  float time_shift = millis();

  switch (sinNum) {
    case 0: //волна I
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          int8_t v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy)))) / 32767.0);
          leds[XY(x, y)].r = ~v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 134.3447 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 170.3884 * time_shift))) / 32767.0;
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy)))) / 32767.0);
          leds[XY(x, y)].b = ~v;
          
        }
      }
      break;
    case 1: //волна II
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          CRGB color;
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          int8_t v = 127 * (float(0.001 * time_shift * e_s3_speed) + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy)))) / 32767.0);
          color.r = ~v;
          cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 68.8107 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 65.534 * time_shift))) / 32767.0;
          v = 127 * (float(0.001 * time_shift * e_s3_speed) + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy)))) / 32767.0);
          color.r = (uint8_t(~v) > color.r)? ~v : color.r;
          color.g = uint8_t(~v) >> 1;
          leds[XY(x, y)] = color;
        }
      }
      break;
    case 2: //волна III
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          int8_t v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy)))) / 32767.0);
          leds[XY(x, y)].r = ~v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 68.8107 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 65.534 * time_shift))) / 32767.0;
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy)))) / 32767.0);
          leds[XY(x, y)].g = ~v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 134.3447 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 170.3884 * time_shift))) / 32767.0;
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy)))) / 32767.0);
          leds[XY(x, y)].b = ~v;
          
        }
      }
      break;
    case 3: //волна IV
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          int8_t v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 100)) / 32767.0);
          leds[XY(x, y)].r = ~v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 68.8107 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 65.534 * time_shift))) / 32767.0;
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 100)) / 32767.0);
          leds[XY(x, y)].g = ~v;
          
          cx = (y - semiHeightMajor); // + float(e_s3_size * (sin16(e_s3_speed * 134.3447 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor); // + float(e_s3_size * (cos16(e_s3_speed * 170.3884 * time_shift))) / 32767.0;
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 100)) / 32767.0);
          leds[XY(x, y)].b = ~v;
          
        }
      }
      break;
    case 4: //волна анаглиф
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          int8_t v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 10)) / 32767.0);
          leds[XY(x, y)].r = ~v;
          
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 1)) / 32767.0);
          leds[XY(x, y)].b = ~v;
          
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 1)) / 32767.0);
          leds[XY(x, y)].g = ~v;
          
        }
      }
      break;
    case 5: //разноцvетная волна
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          int8_t v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 5)) / 32767.0);
          leds[XY(x, y)].r = v;
          
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 10)) / 32767.0);
          leds[XY(x, y)].b = v;
          
          v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 50)) / 32767.0);
          leds[XY(x, y)].g = v;
          
        }
      }
      break;
    case 6: //волна в сети
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          int8_t v = 127 * (1 + sin16(127 * _scale * sqrt((((float) cx * cx) + ((float) cy * cy))) + (time_shift * e_s3_speed * 5)) / 32767.0);
          leds[XY(x, y)].g = ~v;
          
          v = 127 * (1 + sinf(_scale * x * 10 + float(0.01 * time_shift * e_s3_speed)));
          leds[XY(x, y)].b = ~v;
          
          v = 127 * (1 + sinf(_scale * y * 10 + float(0.011 * time_shift * e_s3_speed)));
          leds[XY(x, y)].r = ~v;
          
        }
      }
      break;
    case 7: //спиральная волна
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          float cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 98.301 * time_shift))) / 32767.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 72.0874 * time_shift))) / 32767.0;
          uint8_t v = 127 * (1 + sin16(127 * _scale * (2 * atan2(cy, cx) + hypot(cy, cx)) + (time_shift * e_s3_speed * 5)) / 32767.0);
          leds[XY(x, y)].r = v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 68.8107 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 65.534 * time_shift))) / 32767.0;
          v = 127 * (1 + sin16(127 * _scale * (2 * atan2(cy, cx) + hypot(cy, cx)) + (time_shift * e_s3_speed * 5)) / 32767.0);
          leds[XY(x, y)].b = v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (sin16(e_s3_speed * 134.3447 * time_shift))) / 32767.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (cos16(e_s3_speed * 170.3884 * time_shift))) / 32767.0;
          v = 127 * (1 + sin16(127 * _scale * (2 * atan2(cy, cx) + hypot(cy, cx)) + (time_shift * e_s3_speed * 5)) / 32767.0);;
          leds[XY(x, y)].g = v;
          
        }
      }
      break;

    case 8: //быстрая волна
      for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
          
          float cx = (y - semiHeightMajor) + float(e_s3_size * (127 - sin8(e_s3_speed * .48 * time_shift))) / 64.0; // the 8 centers the middle on a 16x16
          float cy = (x - semiWidthMajor) + float(e_s3_size * (127 - cos8(e_s3_speed * .126 * time_shift))) / 64.0;
          int8_t v = sin8((_scale / 2) * sqrt(((cx * cx) + (cy * cy))));
          leds[XY(x, y)].r = ~v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (127 - sin8(e_s3_speed * .168 * time_shift))) / 64.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (127 - cos8(e_s3_speed * .26 * time_shift))) / 64.0;
          v = sin8((_scale / 2) * sqrt(((cx * cx) + (cy * cy))));
          leds[XY(x, y)].g = ~v;
          
          cx = (y - semiHeightMajor) + float(e_s3_size * (127 - sin8(e_s3_speed * .328 * time_shift))) / 64.0;
          cy = (x - semiWidthMajor) + float(e_s3_size * (127 - cos8(e_s3_speed * .416 * time_shift))) / 64.0;
          v = sin8((_scale / 2) * sqrt(((cx * cx) + (cy * cy))));
          leds[XY(x, y)].b = ~v;
          
        }
      }
      break;
  }
}
