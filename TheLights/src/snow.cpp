#include <snow.h>

void snow() {
  snowx();
  delay(20);
  FastLED.show();
}

byte FF16[16][16];
byte SF16[16][16];

bool loadingFlag16 = true;
uint8_t speed16 = 128;

int shift16;

uint16_t XY16(uint8_t x, uint8_t y) { return ((16 - 1 - y) * 16 + x); }

void snowx() {
  if (loadingFlag16) {
    memset8(SF16, 0, NUM_LEDS);
    memset8(FF16, 0, NUM_LEDS);
    shift16 = 0;
    loadingFlag16 = 0;
  }
  for (byte y = 0; y < 16; y++) {
    for (byte x = 0; x < 16; x++) {
      CRGB col = CHSV(0, 0, (FF16[y][x]) ? 255 : 0);
      leds[XY16(y, x)] =
          nblend(col, CHSV(0, 0, (SF16[y][x]) ? 255 : 0), shift16);
    }
  }
  shift16 += speed16;
  if (shift16 >= 255) {
    memcpy(FF16, SF16, 16 * 16);
    for (byte y = 0; y < 16; y++) {
      for (byte x = 0; x < 16; x++) {
        if (SF16[y][x] && y) {
          SF16[y][x - 1] = 255;
          SF16[y][x] = 0;
        } else if (SF16[y][x] && !y) {
          SF16[y][x] = 0;
        }
      }
    }
    if (random() % 2) SF16[random(0, 16)][16 - 1] = 255;
    shift16 %= 255;
  }
}