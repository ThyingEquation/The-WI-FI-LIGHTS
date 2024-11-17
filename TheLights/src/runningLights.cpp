#include <runningLights.h>

XYMap xyMap(mWidth, mHeight);

bool setUp = true;
int8_t pos[2][8];
byte dir[8];
byte hue6;
bool fade = false;
uint16_t snake2[10];
uint16_t head = 0;
uint16_t tail = 0;
uint16_t pixelCounter = 0;
unsigned long previousMillisE = 0;

uint16_t XY6(uint8_t x, uint8_t y) { return (y * mWidth + x); }

void runningLights() {
  switch (choosenModeD2) {
    case 1:
      draw5R();
      break;

    case 2:
      draw6R();
      break;

    case 3:
      draw3R();
      break;

    case 4:
      draw4R();
      break;

    case 5:
      jumplight();
      break;

    case 6:
      runningColorLED();
      break;

    case 7:
      theaterChase(pgm_read_dword(&(mainColors[col])));
      if (++col >= 128) {
        col = 0;
      }
      break;

    case 8:
      theaterChaseRainbow();
      break;

    case 9:
      drawB3();
      break;

    case 10:
      snake();
      break;

    default:
      break;
  }
}

void move(byte id) {
  switch (dir[id]) {
    case 0:
      pos[1][id]++;
      break;
    case 1:
      pos[0][id]++;
      break;
    case 2:
      pos[1][id]--;
      break;
    case 3:
      pos[0][id]--;
      break;
  }
}
void check2(byte id) {
  if (leds[XY6(pos[0][id], pos[1][id])] == CRGB(0, 0, 0))
    dir[id]++;
  else
    dir[id]--;
  if (dir[id] > 3)
    dir[id] = 0;
  else if (dir[id] < 0)
    dir[id] = 3;
}
void check3(byte id) {
  if (pos[0][id] > mHeight - 1) pos[0][id] = 0;
  if (pos[1][id] > mWidth - 1) pos[1][id] = 0;
  if (pos[0][id] < 0) pos[0][id] = mHeight - 1;
  if (pos[1][id] < 0) pos[1][id] = mWidth - 1;
}
void check1(byte id) {
  if (leds[XY6(pos[0][id], pos[1][id])] == CRGB(0, 0, 0))
    leds[XY(pos[0][id], pos[1][id])] = CHSV(hue6, 255, 255);
  else
    leds[XY(pos[0][id], pos[1][id])] = 0;
}

void draw3R() {
  if (setUp) {
    setUp = false;
    FastLED.clear();
    for (byte i = 0; i < 8; i++) {
      pos[0][i] = random(0, mHeight);
      pos[1][i] = random(0, mWidth);
      dir[i] = random(0, 3);
    }
  }

  for (byte i = 0; i < 8; i++) {
    check1(i);
    move(i);
    check3(i);
    check2(i);
  }
  hue6++;
  if (fade) fadeToBlackBy(leds, NUM_LEDS, 6);
  EVERY_N_SECONDS(1) { setUp = true; }
  FastLED.show();
}

int lightersPosX6[32];
int lightersPosY6[32];
uint16_t lightersSpeedX6[32];
uint16_t lightersSpeedY6[32];
byte lightersSpeedZ[32];
byte lcolor6[32];
byte mass6[32];
bool loadingFlag6 = true;

uint16_t XY3(uint8_t x, uint8_t y) {
  if ((y % 2 == 0) || 1) {
    return ((uint32_t)y * mWidth + x) % (mWidth * mHeight);
  } else {
    return ((uint32_t)y * mWidth + mWidth - x - 1) %
           (mWidth * mHeight);
  }
}

void drawPixelXYF(float x, float y, CRGB color) {
  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx,
          iy = 255 - yy;

#define WU_WEIGHT(a, b) ((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy), WU_WEIGHT(ix, yy),
                   WU_WEIGHT(xx, yy)};

  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XY(xn, yn)];
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    leds[XY(xn, yn)] = clr;
  }
}

void draw4R() {

  if (loadingFlag6) {
    loadingFlag6 = false;
    randomSeed(millis());
    for (byte i = 0; i < 32; i++) {
      lightersSpeedX6[i] = random(-10, 10);
      lightersSpeedY6[i] = random(-10, 10);
      mass6[i] = random(5, 10);
      lightersSpeedZ[i] = random(3, 25);
      lightersPosX6[i] = random(0, mWidth * 10);
      lightersPosY6[i] = random(0, mHeight * 10);
      lcolor6[i] = random(0, 9) * 28;
    }
  }

  switch (2) {
    case 0:
      FastLED.clear();
      break;
    case 1:
      fadeToBlackBy(leds, NUM_LEDS, 50);
      break;
    case 2:
      blur2d(leds, mWidth, mHeight, 30, xyMap);
      fadeToBlackBy(leds, NUM_LEDS, 5);
      break;
    case 3:
      fadeToBlackBy(leds, NUM_LEDS, 200);
      break;
  }

  for (byte i = 0; i < 32; i++) {
    lcolor6[i]++;
    switch (3) {
      case 0:
        lightersPosX6[i] +=
            beatsin88(lightersSpeedX6[0] * 255, 0,
                      mass6[i] / 10 * ((mHeight + mWidth) / 8)) -
            mass6[i] / 10 * ((mHeight + mWidth) / 16);
        lightersPosY6[i] +=
            beatsin88(lightersSpeedY6[0] * 255, 0,
                      mass6[i] / 10 * ((mHeight + mWidth) / 8)) -
            mass6[i] / 10 * ((mHeight + mWidth) / 16);
        break;
      case 1:
        lightersPosX6[i] = beatsin16(
            lightersSpeedX6[i] / map(255, 1, 255, 10, 1), 0, (mWidth - 1) * 10);
        lightersPosY6[i] =
            beatsin16(lightersSpeedY6[i] / map(255, 1, 255, 10, 1), 0,
                      (mHeight - 1) * 10);
        break;
      case 2:
        lightersPosX6[i] += lightersSpeedX6[i] / map(255, 1, 255, 10, 1);
        lightersPosY6[i] += lightersSpeedY6[i] / map(255, 1, 255, 10, 1);
        break;
      case 3:
        lightersPosX6[i] += mass6[i] * cos(radians(lightersSpeedY6[i])) /
                            map(255, 1, 255, 10, 1);
        lightersPosY6[i] += mass6[i] * sin(radians(lightersSpeedY6[i])) /
                            map(255, 1, 255, 10, 1);
        lightersSpeedY6[i] += lightersSpeedX6[i] / map(255, 1, 255, 20, 2);
        break;
    }

    if (lightersPosY6[i] < 0) {
      lightersPosY6[i] = 1;
      lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
    }
    if (lightersPosX6[i] < 0) {
      lightersPosX6[i] = 1;
      lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
    }
    if (lightersPosY6[i] >= (mHeight - 1) * 10) {
      lightersPosY6[i] = ((mHeight - 1) * 10) - 1;
      lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
    }
    if (lightersPosX6[i] >= (mWidth - 1) * 10) {
      lightersPosX6[i] = ((mWidth - 1) * 10) - 1;
      lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
    }

    CRGB color =
        CHSV(lcolor6[i], 255,
             beatsin8(lightersSpeedZ[i] / map(255, 1, 255, 10, 1), 128, 255));
    drawPixelXYF((float)lightersPosX6[i] / 10, (float)lightersPosY6[i] / 10,
                 color);
  }

  EVERY_N_SECONDS(10) {
    randomSeed(millis());
    for (byte i = 0; i < 32; i++) {
      lightersSpeedX6[i] = random(-10, 10);
      lightersSpeedY6[i] = random(0, 360);
      mass6[i] = random(5, 10);
      lightersSpeedZ[i] = random(3, 25);
    }
  }

  FastLED.delay(20);
  FastLED.show();
}

void jumplight() {
  for (byte i = 8; i--;) {
    leds[XY6(beatsin8(12 + i, 0, mWidth - 1),
             beatsin8(15 - i, 0, mHeight - 1))] =
        CHSV(beatsin8(12, 0, 255), 255, 255);
    blur2d(leds, mWidth, mHeight, 16, xyMap);
  }
  FastLED.show();
}

unsigned long previousMillisR = 0;
const long interval = 20;
int currentLED = 0;
bool firstPass = true;

void runningColorLED() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisR >= interval) {
    previousMillisR = currentMillis;

    if (firstPass) {
      leds[currentLED] = CHSV(currentLED, 255, 255);
    } else {
      leds[currentLED] = CHSV(rand() % 256, 255, 255);
    }

    FastLED.show();
    delay(20);
    leds[currentLED] = CRGB::Black;

    currentLED++;
    if (currentLED >= NUM_LEDS) {
      currentLED = 0;
      if (firstPass) {
        firstPass = false;
      } else {
        firstPass = true;
      }
    }
  }
}

void theaterChase(uint32_t c) {
  for (int j = 0; j < 2; j++) {
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);
      }
      strip.show();

      delay(200);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

void theaterChaseRainbow() {
  for (int j = 0; j < 2; j++) {
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel((i + j) % 255));
      }
      strip.show();

      delay(200);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

uint16_t XYB3(uint8_t x, uint8_t y) { return (y * mWidth + x); }

void drawPixelXYFB3(float x, float y, const CRGB& color) {
  if (x < 0 || y < 0 || x > ((float)mWidth - 1) ||
      y > ((float)mHeight - 1))
    return;

  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx,
          iy = 255 - yy;

#define WU_WEIGHT(a, b) ((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy), WU_WEIGHT(ix, yy),
                   WU_WEIGHT(xx, yy)};

  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XYB3(xn, yn)];
    if (xn < (int)mWidth - 1 && yn < (int)mHeight - 1 && yn > 0 &&
        xn > 0) {
      clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
      clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
      clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    } else {
      clr.r = qadd8(clr.r, (color.r * 85) >> 8);
      clr.g = qadd8(clr.g, (color.g * 85) >> 8);
      clr.b = qadd8(clr.b, (color.b * 85) >> 8);
    }
    leds[XYB3(xn, yn)] = clr;
  }
#undef WU_WEIGHT
}

void drawCurve(float x, float y, float x2, float y2, float x3, float y3,
               CRGB coll) {
  float xu = 0.0, yu = 0.0, u = 0.0;
  for (u = 0.0; u <= 1.0; u += 0.01) {
    xu = pow(1 - u, 3) * x + 3 * u * pow(1 - u, 2) * x2 +
         3 * pow(u, 2) * (1 - u) * x3 + pow(u, 3) * x3;
    yu = pow(1 - u, 3) * y + 3 * u * pow(1 - u, 2) * y2 +
         3 * pow(u, 2) * (1 - u) * y3 + pow(u, 3) * y3;
    drawPixelXYFB3(xu, yu, coll);
  }
}
byte hue;

void drawB3() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
  byte x1 = beatsin8(18 + 100, 1, (mWidth - 2));
  byte x2 = beatsin8(23 + 100, 1, (mWidth - 2));
  byte x3 = beatsin8(27 + 100, 1, (mWidth - 2));

  byte y1 = beatsin8(20 + 100, 1, (mHeight - 2));
  byte y2 = beatsin8(26 + 100, 1, (mHeight - 2));
  byte y3 = beatsin8(15 + 100, 1, (mHeight - 2));

  drawCurve(x1, y1, x2, y2, x3, y3, CHSV(hue, 255, 255));
  hue++;
  FastLED.show();
}

void draw5R() {
  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[col])));
    strip.show();
    strip.setPixelColor(ledsCount - 1, strip.Color(0, 0, 0));
    strip.show();
    delay(100);
    ledsCount++;
  } else {
    col = random(127);
    ledsCount = 0;
  }
}

void draw6R() {
  if (ledsCount <= NUM_LEDS) {
    strip.setPixelColor(ledsCount, pgm_read_dword(&(mainColors[col])));
    strip.show();
    strip.setPixelColor(ledsCount - 1, strip.Color(0, 0, 0));
    strip.show();
    delay(10);
    ledsCount++;
  } else {
    col = random(127);
    ledsCount = 0;
  }
}

void snake() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisE >= 50) {
    previousMillisE = currentMillis;
    head = (head + 1) % strip.numPixels();
    tail = (tail + 1) % 10;
    snake2[tail] = head;
    pixelCounter++;
    if (pixelCounter >= NUM_LEDS) {
      pixelCounter = 0;
      col = random(127);
    }
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    for (uint16_t i = 0; i < 10; i++) {
      strip.setPixelColor(snake2[i], pgm_read_dword(&(mainColors[col])));
    }
    strip.show();
  }
}