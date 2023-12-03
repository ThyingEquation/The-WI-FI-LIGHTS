#include <mode7.h>

const uint32_t colorStar[] = { strip.Color(149, 167, 232), strip.Color(224, 232, 255), strip.Color(255, 159, 19), strip.Color(252,127,20), 
strip.Color(242,95,106), strip.Color(199,0,17), strip.Color(56,112,255)};

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

unsigned long starnumber = 0;
unsigned long starcolor = 0;
int starcycle = 0; 

void mode7(String Mode7) {

  if (Mode7.indexOf("/Y1") != -1) {

    if (starcycle < 60) {

      starnumber = random(256);
      starcolor = random(7);

      strip.setPixelColor(starnumber, strip.Color(255, 240, 245));
      strip.show();
      delay(500);
      starnumber = random(256);
      strip.setPixelColor(starnumber, colorStar[starcolor]);
      strip.show();
      delay(1100);
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(0, 0, 0));
      strip.show();
      delay(10);
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(0, 0, 0));
      strip.show();
      delay(10);
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(0, 0, 0));
      strip.show();
      delay(10);
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(0, 0, 0));
      strip.show();
      delay(10);
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(0, 0, 0));
      strip.show();
      delay(10);
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(0, 0, 0));
      strip.show();
      delay(10);
      starcycle++;
    } else {
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(255, 0, 0));
      strip.show();
      delay(1000);
      starnumber = random(256);
      strip.setPixelColor(starnumber, strip.Color(0, 0, 255));
      strip.show();
      starcycle = 0;
    }
  }

  if (Mode7.indexOf("/Y2") != -1) {

   spaceship();
  // FastLED.show();
  }

  if (Mode7.indexOf("/Y3") != -1) {
  SolarSystem();
  FastLED.show();
  }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// космическме корабли

uint16_t XY7 (uint8_t x, uint8_t y) {
  return (y * kMatrixWidth + x);
}

byte dir7 = 3;
void drawPixel(byte x, byte y, CRGB color) {
  leds[XY7(x, y)] += color;
  if (kMatrixWidth > 24 || kMatrixHeight > 24) {
    leds[XY7(x + 1, y)] += color;
    leds[XY7(x - 1, y)] += color;
    leds[XY7(x, y + 1)] += color;
    leds[XY7(x, y - 1)] += color;
  }
}

void MoveX(int8_t delta) {
  if (delta) {
    if (delta > 0) {
      for (uint8_t y = 0; y < kMatrixHeight; y++) {
        for (uint8_t x = 0; x < kMatrixWidth; x++) {
          leds[XY7(x, y)] = leds[XY7(x + delta, y)];
        }
      }
    } else {
      for (uint8_t y = 0; y < kMatrixHeight; y++) {
        for (uint8_t x = kMatrixWidth - 1; x > 0; x--) {
          leds[XY7(x, y)] = leds[XY7(x + delta, y)];
        }
      }
    }
  }
}

void MoveY(int8_t delta) {
  if (delta) {
    if (delta > 0) {
      for (uint8_t x = 0; x < kMatrixWidth; x++) {
        for (uint8_t y = 0; y < kMatrixHeight; y++) {
          leds[XY7(x, y)] = leds[XY7(x, y + delta)];
        }
      }
    } else {
      for (uint8_t x = 0; x < kMatrixWidth; x++) {
        for (uint8_t y = kMatrixHeight - 1; y > 0; y--) {
          leds[XY7(x, y)] = leds[XY7(x, y + delta)];
        }
      }
    }
  }
}


void spaceship() {
  fadeToBlackBy(leds, NUM_LEDS, 16);
  switch (dir7) {
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
    for (byte i = 0; i < 8; i++) {
    byte x = beatsin8(12 + i, 2, kMatrixWidth - 3);
    byte y = beatsin8(15 + i, 2, kMatrixHeight - 3);
    drawPixel(x, y, ColorFromPalette(RainbowColors_p, beatsin8(12 + i, 0, 255), 255));
  }
  blur2d(leds, kMatrixWidth, kMatrixHeight, 32);
  FastLED.show();
  EVERY_N_SECONDS(5) {
    if (dir7 == 7) dir7 = 0;
    else dir7++;
  }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// солнечная система

#define MATRIX_TYPE 1
#define WIDTH kMatrixWidth
#define HEIGHT kMatrixHeight
#define CENTER_X WIDTH/2
#define CENTER_Y HEIGHT/2
#define LIGHTERS_AM (32)/4 //////////////////////////////////////////////////////////////////////////////////////////////////////
byte lightersColor[LIGHTERS_AM];
byte loopCounter;

int angle[LIGHTERS_AM];
int8_t angleSpeed[LIGHTERS_AM];


bool loadingFlag7 = true;

uint8_t scale7 = 16;

uint16_t XY77 (uint8_t x, uint8_t y) {

  if ((y % 2 == 0) || MATRIX_TYPE)                     // if steam line
  {
    return ((uint32_t)y * WIDTH + x) % (WIDTH * HEIGHT);
  }
  else                                                      // if not steam line
  {
    return ((uint32_t)y  * WIDTH + WIDTH - x - 1) % (WIDTH * HEIGHT);
  }
}


void SolarSystem() {
  if (loadingFlag7) {
    loadingFlag7 = false;
    randomSeed(millis());
    for (byte i = 0; i < LIGHTERS_AM; i++) {
      lightersColor[i] = random(0, 9) * 28;
      angle[i] = random(0, 360);
      angleSpeed[i] = 1 + i * random(0, 2) - 1;
    }
  }
  fadeToBlackBy (leds, NUM_LEDS, 10);
  //FastLED.clear();
  for (byte i = 0; i < map(scale7, 1, 16, 1, LIGHTERS_AM); i++) {
    angle[i] += angleSpeed[i];
    if (angle[i] > 360) angle[i] = 360 - angle[i];
    if (angle[i] < 0) angle[i] = angle[i];
    leds[XY77(CENTER_X + sin(radians(angle[i]))*  angleSpeed[i], CENTER_Y + cos(radians(angle[i]))* angleSpeed[i])] = CHSV(lightersColor[i] + 128, 255, 255);
    /*  EVERY_N_SECONDS(random(10,20)){
      angleSpeed[i] = random(-10, 10);
      }*/
  }
}



