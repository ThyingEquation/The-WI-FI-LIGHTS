#include <water.h>

CRGBPalette16 pacifica_palette_1 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                    0x000210, 0x000212, 0x000114, 0x000117,
                                    0x000019, 0x00001C, 0x000026, 0x000031,
                                    0x00003B, 0x000046, 0x14554B, 0x28AA50};
CRGBPalette16 pacifica_palette_2 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                    0x000210, 0x000212, 0x000114, 0x000117,
                                    0x000019, 0x00001C, 0x000026, 0x000031,
                                    0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};
CRGBPalette16 pacifica_palette_3 = {0x000208, 0x00030E, 0x000514, 0x00061A,
                                    0x000820, 0x000927, 0x000B2D, 0x000C33,
                                    0x000E39, 0x001040, 0x001450, 0x001860,
                                    0x001C70, 0x002080, 0x1040BF, 0x2060FF};

void water() {
  switch (choosenModeD2) {
    case 1:
      EVERY_N_MILLISECONDS(20) { pacifica_loop(); }
      break;

    case 2:
      PoolNoise();
      break;

    default:
      break;
  }
}

void pacifica_loop() {
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
  sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
  sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
  sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

  fill_solid(leds, NUM_LEDS, CRGB(2, 6, 10));

  pacifica_one_layer(pacifica_palette_1, sCIStart1,
                     beatsin16(3, 11 * 256, 14 * 256), beatsin8(10, 70, 130),
                     0 - beat16(301));
  pacifica_one_layer(pacifica_palette_2, sCIStart2,
                     beatsin16(4, 6 * 256, 9 * 256), beatsin8(17, 40, 80),
                     beat16(401));
  pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256,
                     beatsin8(9, 10, 38), 0 - beat16(503));
  pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256,
                     beatsin8(8, 10, 28), beat16(601));

  pacifica_add_whitecaps();

  pacifica_deepen_colors();
  FastLED.show();
}

void pacifica_one_layer(CRGBPalette16& p, uint16_t cistart, uint16_t wavescale,
                        uint8_t bri, uint16_t ioff) {
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16(waveangle) + 32768;
    uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16(ci) + 32768;
    uint8_t sindex8 = scale16(sindex16, 240);
    CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

void pacifica_add_whitecaps() {
  uint8_t basethreshold = beatsin8(9, 55, 65);
  uint8_t wave = beat8(7);

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if (l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8(overage, overage);
      leds[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
    }
  }
}

void pacifica_deepen_colors() {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8(leds[i].blue, 145);
    leds[i].green = scale8(leds[i].green, 200);
    leds[i] |= CRGB(2, 5, 7);
  }
}

uint16_t XY9(uint8_t x, uint8_t y) { return (y * kMatrixWidth + x); }

bool loadingFlag9 = true;
CRGBPalette16 currentPalette9(PartyColors_p);
uint8_t hue9;

void N() {
  for (byte y = 0; y < kMatrixHeight; y++) {
    for (byte x = 0; x < kMatrixWidth; x++) {
      uint8_t pixelHue8 = inoise8(x * 30, y * 30, millis() / 16);
      leds[XY9(x, y)] = ColorFromPalette(currentPalette9, pixelHue8);
    }
  }
  blur2d(leds, kMatrixWidth, kMatrixHeight, 32, xyMap);
}

void PoolNoise() {
  if (loadingFlag9) {
    loadingFlag9 = false;
    hue9 = 150;
  }

  if (BRIGHTNESS + 115 < 255) {
    fill_solid(currentPalette9, 16, CHSV(hue9, 255, BRIGHTNESS + 90));
    currentPalette9[10] = CHSV(hue9, 195, BRIGHTNESS + 115);
    currentPalette9[9] = CHSV(hue9, 0, BRIGHTNESS + 80);
    currentPalette9[8] = CHSV(hue9, 0, BRIGHTNESS + 70);
    currentPalette9[7] = CHSV(hue9, 195, BRIGHTNESS + 115);
  } else {
    fill_solid(currentPalette9, 16, CHSV(hue9, 255, 230));
    currentPalette9[10] = CHSV(hue9, 195, 255);
    currentPalette9[9] = CHSV(hue9, 0, 220);
    currentPalette9[8] = CHSV(hue9, 0, 210);
    currentPalette9[7] = CHSV(hue9, 195, 255);
  }

  blur2d(leds, kMatrixWidth, kMatrixHeight, 100, xyMap);
  N();
  FastLED.show();
  FastLED.delay(1000 / 60);
}