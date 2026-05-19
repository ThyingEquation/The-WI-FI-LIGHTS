#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (waterSettings):
    1) Скорости эффектов
*/

enum waterSettings
{
  POOL_DELAY = 17
};

static void drawLagoon();
static void drawPool();

void drawWaterEffects(uint8_t subMode)
{
  switch (subMode)
  {
  case 0:
    EVERY_N_MILLISECONDS(20) { drawLagoon(); }
    break;

  case 1:
    drawPool();
    break;

  default:
    break;
  }
}

static void drawLagoonOneLayer(CRGBPalette16 &p, uint16_t ciStart,
                               uint16_t waveScale, uint8_t brightness, uint16_t iOff)
{
  uint16_t ci = ciStart;
  uint16_t waveAngle = iOff;
  uint16_t waveScaleHalf = (waveScale / 2) + 20;
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    waveAngle += 250;
    uint16_t s16 = sin16(waveAngle) + 32768;
    uint16_t cs = scale16(s16, waveScaleHalf) + waveScaleHalf;
    ci += cs;
    uint16_t sindex16 = sin16(ci) + 32768;
    uint8_t sindex8 = scale16(sindex16, 240);
    CRGB c = ColorFromPalette(p, sindex8, brightness, LINEARBLEND);
    leds[i] += c;
  }
}

static void drawLagoonAddWhitecaps()
{
  uint8_t baseThreshold = beatsin8(9, 55, 65);
  uint8_t wave = beat8(7);

  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    uint8_t threshold = scale8(sin8(wave), 20) + baseThreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if (l > threshold)
    {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8(overage, overage);
      leds[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
    }
  }
}

static void drawLagoonDeepenColors()
{
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    leds[i].blue = scale8(leds[i].blue, 145);
    leds[i].green = scale8(leds[i].green, 200);
    leds[i] |= CRGB(2, 5, 7);
  }
}

void drawLagoon()
{
  static CRGBPalette16 drawLagoonPalette1 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                      0x000210, 0x000212, 0x000114, 0x000117,
                                      0x000019, 0x00001C, 0x000026, 0x000031,
                                      0x00003B, 0x000046, 0x14554B, 0x28AA50};
  static CRGBPalette16 drawLagoonPalette2 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                      0x000210, 0x000212, 0x000114, 0x000117,
                                      0x000019, 0x00001C, 0x000026, 0x000031,
                                      0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};
  static CRGBPalette16 drawLagoonPalette3 = {0x000208, 0x00030E, 0x000514, 0x00061A,
                                      0x000820, 0x000927, 0x000B2D, 0x000C33,
                                      0x000E39, 0x001040, 0x001450, 0x001860,
                                      0x001C70, 0x002080, 0x1040BF, 0x2060FF};

  static uint16_t sciStart1, sciStart2, sciStart3, sciStart4;
  static uint32_t sLastMs = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastMs;
  sLastMs = ms;
  uint16_t speedFactor1 = beatsin16(3, 179, 269);
  uint16_t speedFactor2 = beatsin16(4, 179, 269);
  uint32_t deltaMs1 = (deltams * speedFactor1) / 256;
  uint32_t deltaMs2 = (deltams * speedFactor2) / 256;
  uint32_t deltams21 = (deltaMs1 + deltaMs2) / 2;
  sciStart1 += (deltaMs1 * beatsin88(1011, 10, 13));
  sciStart2 -= (deltams21 * beatsin88(777, 8, 11));
  sciStart3 -= (deltaMs1 * beatsin88(501, 5, 7));
  sciStart4 -= (deltaMs2 * beatsin88(257, 4, 6));

  fill_solid(leds, MATRIX_LEDS, CRGB(2, 6, 10));

  drawLagoonOneLayer(drawLagoonPalette1, sciStart1, beatsin16(3, 11 * 256, 14 * 256),
                     beatsin8(10, 70, 130), 0 - beat16(301));
  drawLagoonOneLayer(drawLagoonPalette2, sciStart2, beatsin16(4, 6 * 256, 9 * 256),
                     beatsin8(17, 40, 80), beat16(401));
  drawLagoonOneLayer(drawLagoonPalette3, sciStart3, 6 * 256, beatsin8(9, 10, 38),
                     0 - beat16(503));
  drawLagoonOneLayer(drawLagoonPalette3, sciStart4, 5 * 256, beatsin8(8, 10, 28),
                     beat16(601));

  drawLagoonAddWhitecaps();

  drawLagoonDeepenColors();
  stripShow();
}

void drawPool()
{
  static CRGBPalette16 currentPalette(PartyColors_p);
  static XYMap xyMap(MATRIX_WIDTH, MATRIX_HEIGHT);
  static bool loadingFlag = true;
  static uint8_t hue;
  static uint32_t lastTime = 0;

  if (millis() - lastTime < POOL_DELAY)
    return;
  lastTime = millis();

  uint32_t t = millis() / 16;

  if (loadingFlag)
  {
    loadingFlag = false;
    hue = 150;
  }

  fill_solid(currentPalette, 16, CHSV(hue, 255, 230));
  currentPalette[10] = CHSV(hue, 195, 255);
  currentPalette[9] = CHSV(hue, 0, 220);
  currentPalette[8] = CHSV(hue, 0, 210);
  currentPalette[7] = CHSV(hue, 195, 255);

  blur2d(leds, MATRIX_WIDTH, MATRIX_HEIGHT, 100, xyMap);

  for (byte y = 0; y < MATRIX_HEIGHT; y++)
  {
    for (byte x = 0; x < MATRIX_WIDTH; x++)
    {
      uint8_t pixelHue8 = inoise8(x * 30, y * 30, t);
      leds[XY(x, y)] = ColorFromPalette(currentPalette, pixelHue8);
    }
  }
  blur2d(leds, MATRIX_WIDTH, MATRIX_HEIGHT, 32, xyMap);

  stripShow();
}