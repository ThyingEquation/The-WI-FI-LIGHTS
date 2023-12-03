#include <mode10.h>

#define MATRIX_TYPE 1
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

byte effect = 3;

const int SPARKS_AM = 16 * 4; /////////////////////////////////////////////// оптимизировать
float FADE_KOEF = .6;
float SpeedK = 2;
float SpeedDecX = .1;
float SpeedDecY = 0;

int sparksPos[2][SPARKS_AM];
float sparksSpeed[2][SPARKS_AM];
float sparksFade[SPARKS_AM];
byte sparksColor[SPARKS_AM];
float sparksSat[SPARKS_AM];
static byte period = 10;

int genPos[2];
int gravityPos[2];
bool run = true; 

bool loadingFlag = true;



uint16_t XY10 (uint8_t x, uint8_t y) { // одинаково для В1, В2

  if ((y % 2 == 0) || MATRIX_TYPE)                     // если чётная строка
  {
    return ((uint32_t)y * kMatrixWidth + x) % (kMatrixWidth * kMatrixHeight);
  }
  else                                                      // если нечётная строка
  {
    return ((uint32_t)y  * kMatrixWidth + kMatrixWidth - x - 1) % (kMatrixWidth * kMatrixHeight);
  }
}

uint16_t XYB3 (uint8_t x, uint8_t y) { // для В3
  return (y * kMatrixWidth + x);
}


void mode10(String choosenMode) {

if (choosenMode.indexOf("/B1") != -1) {
  EVERY_N_SECONDS(10) {(effect== 3) ? effect = 0 : effect++; FastLED.clear(); }
  switch (effect) {
    case 0: drawFire(); break;
    case 1: drawGravityDemo(); break;
    case 2: drawBengalFire(); break;
    case 3: drawWind(); break;
  }
  FastLED.show();
}

if (choosenMode.indexOf("/B2") != -1) {
  draw13();
  FastLED.show();
  delay(20);
}


if (choosenMode.indexOf("/B3") != -1) {

  drawB3();
  FastLED.show();
  //delay(5);
}

if (choosenMode.indexOf("/B4") != -1) {

snow();
delay(20);
FastLED.show(); 

}



} 

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// атоматическое отображение //B1

void regB1(byte id) {
  sparksPos[0][id] = (genPos[0] == -1205)? random(0, kMatrixWidth * 10) : genPos[0];
  sparksPos[1][id] = (genPos[1] == -1205)? random(0,kMatrixHeight * 10) : genPos[1];
  byte al = random8();
  sparksSpeed[0][id] = random(-10, 10);
  sparksSpeed[1][id] = random(-5, 20);
  sparksColor[id] = random();
  sparksFade[id] = 255;
}

void phisicsB1(byte id) {
  if (SpeedK) {
    if (gravityPos[0] != -1205) {
      if (gravityPos[0] < sparksPos[0][id])
        sparksSpeed[0][id] -= SpeedK;
      else
        sparksSpeed[0][id] += SpeedK;
    }
    if (gravityPos[1] != -1205) {
      if (gravityPos[1] < sparksPos[1][id])
        sparksSpeed[1][id] -= SpeedK;
      else
        sparksSpeed[1][id] += SpeedK;
    }
  }
  sparksFade[id] -= (255. / float(kMatrixHeight * FADE_KOEF));
  sparksSat[id] += (255. / (float)((kMatrixWidth + kMatrixWidth) * (FADE_KOEF - 0.2)));
  if (SpeedDecX && sparksSpeed[0][id]) {
    if (sparksSpeed[0][id] > 0)
      sparksSpeed[0][id] -= SpeedDecX;
    else
      sparksSpeed[0][id] += SpeedDecX;
    if (abs(sparksSpeed[0][id]) <= SpeedDecX)
      sparksSpeed[0][id] = 0;
  }
  if (SpeedDecY && sparksSpeed[1][id]) {
    if (sparksSpeed[1][id] > 0)
      sparksSpeed[1][id] -= SpeedDecY;
    else
      sparksSpeed[1][id] += SpeedDecY;
    if (abs(sparksSpeed[1][id]) <= SpeedDecY)
      sparksSpeed[1][id] = 0;
  }
  sparksPos[0][id] += sparksSpeed[0][id];
  sparksPos[1][id] += sparksSpeed[1][id];
}

void wu_pixel(uint32_t x, uint32_t y, CRGB * col) { //awesome wu_pixel procedure by reddit u/sutaburosu
  // extract the fractional parts and derive their inverses
  uint8_t xx = x & 0xff, yy = y & 0xff, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
#define WU_WEIGHT(a, b)((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {
    WU_WEIGHT(ix, iy),
    WU_WEIGHT(xx, iy),
    WU_WEIGHT(ix, yy),
    WU_WEIGHT(xx, yy)
  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    uint16_t xy = XY10((x >> 8) + (i & 1), (y >> 8) + ((i >> 1) & 1));
    leds[xy].r = qadd8(leds[xy].r, col -> r * wu[i] >> 8);
    leds[xy].g = qadd8(leds[xy].g, col -> g * wu[i] >> 8);
    leds[xy].b = qadd8(leds[xy].b, col -> b * wu[i] >> 8);
  }
}

void render(byte id, CRGB Col) {
  phisicsB1(id);
  if (sparksPos[1][id] < ((kMatrixHeight - 1) * 10) and sparksPos[1][id] >= 0)
    if (sparksPos[0][id] < ((kMatrixWidth - 1) * 10) and sparksPos[0][id] >= 0) {
      CRGB color = Col;
      wu_pixel(sparksPos[0][id] * 25.6, sparksPos[1][id] * 25.6, & color);
    }
}

void setGenPos(int x, int y) {
  genPos[0] = x;
  genPos[1] = y;
}

void setGravityPos(int x, int y) {
  gravityPos[0] = x;
  gravityPos[1] = y;
}

void setRegenRule(byte id, bool b) {
  if (b) regB1(id);
}

void Start() {
  for (byte i = 0; i < SPARKS_AM; i++) {
    regB1(i);
    for (byte a = 0; a < i; a++) {
      setRegenRule(a, (sparksPos[0][a] <= 0 || sparksPos[0][a] >= (kMatrixWidth - 1) * 10 || sparksPos[1][a] < 0 || sparksPos[1][a] >= (kMatrixHeight - 1) * 10 || sparksFade[a] < 20) ? 1 : 0);
      phisicsB1(a);
    }
  }
}

void drawFire() {
  setGenPos(kMatrixWidth * 5, kMatrixHeight * 1);
  byte noise = inoise8(millis() / 10);
  setGravityPos(map(noise, 0, 255, kMatrixWidth * 4, kMatrixWidth * 6), map(abs(128 - noise), 0, 127, kMatrixHeight * 6, kMatrixHeight * 9));
  if (loadingFlag) {
    Start();
    loadingFlag = false;
  }
  //FastLED.clear();
  //fadeToBlackBy(leds, NUM_LEDS, 20);
  for (byte i = 0; i < SPARKS_AM; i++) {
    setRegenRule(i, (sparksPos[0][i] <= 0 || sparksPos[0][i] >= (kMatrixWidth - 1) * 10 || sparksPos[1][i] < 0 || sparksPos[1][i] >= (kMatrixHeight - 1) * 10 || sparksFade[i] < 20) ? 1 : 0);
    render(i, ColorFromPalette(HeatColors_p, sparksFade[i]));
  }
  blur2d(leds, kMatrixWidth, kMatrixHeight, 32);
  delay(16);
}

void drawGravityDemo() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  setGenPos(beatsin16(10, 0, kMatrixWidth * 10), beatsin16(10, 0, kMatrixHeight * 10, 0, 16384));
  setGravityPos((kMatrixWidth / 2) * 10, (kMatrixHeight / 2) * 10);
  for (byte i = 0; i < SPARKS_AM; i++) {
    setRegenRule(i, (sparksPos[0][i] <= 0 || sparksPos[0][i] >= (kMatrixWidth - 1) * 10 || sparksPos[1][i] < 0 || sparksPos[1][i] >= (kMatrixHeight - 1) * 10 || sparksFade[i] <= 35) ? 1 : 0);
    render(i, CHSV(sparksColor[i], 255, constrain(sparksFade[i], 32, 255)));
  }
  delay(16);
}

void drawBengalFire() {
  fadeToBlackBy(leds, NUM_LEDS, beatsin8(5, 20, 100));
  setGenPos(kMatrixWidth * 5, kMatrixHeight * 5);
  setGravityPos(-1205, 0);
  for (byte i = 0; i < SPARKS_AM; i++) {
    setRegenRule(i, (sparksPos[0][i] <= 0 || sparksPos[0][i] >= (kMatrixWidth - 1) * 10 || sparksPos[1][i] < 0) ? 1 : 0);
    render(i, CHSV(sparksColor[i], constrain(sparksSat[i], 5, 255), constrain(sparksFade[i], 32, 255)));
    EVERY_N_SECONDS(period) {
      for (byte i = 0; i < SPARKS_AM; i++) regB1(i);
      period = random(10, 60);
    }
  }
  delay(16);
}

void drawWind() {
  setGenPos(0,-1205);
  byte noise = inoise8(millis() / 10);
  setGravityPos(kMatrixWidth * 11, map(abs(128 - noise), 0, 127, kMatrixHeight, kMatrixHeight * 9));
  if (loadingFlag) {
    Start();
    loadingFlag = false;
  }
  //FastLED.clear();
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (byte i = 0; i < SPARKS_AM; i++) {
    setRegenRule(i, (sparksPos[0][i] >= (kMatrixWidth - 1) * 10 || sparksPos[1][i] < 0 || sparksPos[1][i] >= (kMatrixHeight - 1) * 10 || sparksFade[i] < 20) ? 1 : 0);
    render(i, CHSV(0,0,sparksFade[i]));
  }
  //blur2d(leds, LED_COLS, LED_ROWS, 32);
  delay(16);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// кривая B3

//Idea from Ldir's F_lying
void drawPixelXYFB3(float x, float y, const CRGB & color) {
  if (x < 0 || y < 0 || x > ((float) kMatrixWidth - 1) || y > ((float) kMatrixHeight - 1)) return;

  // extract the fractional parts and derive their inverses
  uint8_t xx = (x - (int) x) * 255, yy = (y - (int) y) * 255, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
#define WU_WEIGHT(a, b)((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {
    WU_WEIGHT(ix, iy),
    WU_WEIGHT(xx, iy),
    WU_WEIGHT(ix, yy),
    WU_WEIGHT(xx, yy)
  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XYB3(xn, yn)];
    if (xn < (int) kMatrixWidth - 1 && yn < (int) kMatrixHeight - 1 && yn > 0 && xn > 0) {
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

void drawCurve(float x,float y, float x2,float y2,float x3,float y3,CRGB col) 
{ 
    float xu = 0.0 , yu = 0.0 , u = 0.0 ; 
    int i = 0 ; 
    for(u = 0.0 ; u <= 1.0 ; u += 0.01) 
    { 
        xu = pow(1-u,3)*x+3*u*pow(1-u,2)*x2+3*pow(u,2)*(1-u)*x3 
             +pow(u,3)*x3; 
        yu = pow(1-u,3)*y+3*u*pow(1-u,2)*y2+3*pow(u,2)*(1-u)*y3
            +pow(u,3)*y3; 
        drawPixelXYFB3(xu,yu,col); 
    } 
}
byte hue;
#define speed 100
void drawB3() {
  fadeToBlackBy(leds, NUM_LEDS, 30);
byte x1 = beatsin8 (18+speed, 1, (kMatrixWidth-2));
byte x2 = beatsin8 (23+speed, 1, (kMatrixWidth-2)); 
byte x3 = beatsin8 (27+speed, 1, (kMatrixWidth-2)); 

byte y1 = beatsin8 (20+speed, 1, (kMatrixHeight-2)); 
byte y2 = beatsin8 (26+speed, 1, (kMatrixHeight-2));
byte y3 = beatsin8 (15+speed, 1, (kMatrixHeight-2));

drawCurve(x1,y1,x2,y2,x3,y3,CHSV(hue,255,255));
hue++;}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// паутина 


#define MATRIX_TYPE 1
#define HEIGHT 16
#define WIDTH 16
#define NUM_LEDS HEIGHT * WIDTH 

#define Speed 250
#define Koef 9 //1,3,5,7,9(standart),10,12
#define lines 7
#define Color 1 //By Kostyamat https://editor.soulmatelights.com/gallery/550
#define Fader 30 //By Kostyamat https://editor.soulmatelights.com/gallery/550
#define sx ((256 / (WIDTH / 2)) - (WIDTH / 16))
#define cy ((256 / (HEIGHT / 2)) - (HEIGHT / 16))

uint16_t XY13 (uint8_t x, uint8_t y) {
 //if(x<0||x>= WIDTH || y<0 || y >= HEIGHT) { return;}
  if ((y % 2 == 0) || MATRIX_TYPE)                     // РµСЃР»Рё С‡С‘С‚РЅР°СЏ СЃС‚СЂРѕРєР°
  {
    return ((uint32_t)y *WIDTH + x) % (WIDTH  * HEIGHT);
  }
  else                                                      // РµСЃР»Рё РЅРµС‡С‘С‚РЅР°СЏ СЃС‚СЂРѕРєР°
  {
    return ((uint32_t)y  * HEIGHT + WIDTH - x - 1) % (WIDTH  * HEIGHT);
  }
}





void drawLine(int x1, int y1, int x2, int y2,  const CRGB & color) {
 
  int deltaX = abs(x2 - x1);
  int deltaY = abs(y2 - y1);
  int signX = x1 < x2 ? 1 : -1;
  int signY = y1 < y2 ? 1 : -1;
  int error = deltaX - deltaY;
  
  leds[XY13(x2, y2)] += color;
  while (x1 != x2 || y1 != y2) {
    leds[XY13(x1, y1)] += color;
    int error2 = error * 2;
    if (error2 > -deltaY) {
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX) {
      error += deltaX;
      y1 += signY;
    }
  }
}

void draw13() {
  fadeToBlackBy(leds, NUM_LEDS, Fader);
  double t = millis() / (256 - Speed);
  for (uint8_t c = 0; c < lines; c++) {
    byte xx = sin8(t + (100 * c) * Koef) / sx;
    byte yy = cos8(t + (150 * c) * Koef) / cy;
    if (Color)
      drawLine(xx, yy, WIDTH - xx - 1, HEIGHT - yy - 1, CHSV(c * (256 / lines), 200, 255));
    else
      drawLine(xx, yy, WIDTH - xx - 1, HEIGHT - yy - 1, CHSV(0, 0, 255));
  }
  blur2d(leds, WIDTH, HEIGHT, 32);
  delay(16);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// метель

#define HEIGHT 16
#define WIDTH 16
#define NUM_LEDS WIDTH * HEIGHT

byte FF16[WIDTH][HEIGHT];
byte SF16[WIDTH][HEIGHT];

bool loadingFlag16 = true;
uint8_t speed16 = 128; 

int shift16;

uint16_t XY16 (uint8_t x, uint8_t y) {
  return ((HEIGHT - 1 - y) * WIDTH + x);
}

void snow() {
  if(loadingFlag16){
    memset8(SF16, 0,NUM_LEDS);
    memset8(FF16, 0,NUM_LEDS);
    shift16 = 0;
    loadingFlag16 = 0;
  }
  for (byte y = 0; y < HEIGHT; y++) {
    for (byte x = 0; x < WIDTH; x++) {
      CRGB col = CHSV(0, 0, (FF16[y][x]) ? 255 : 0);
      leds[XY16(y, x)] = nblend(col, CHSV(0, 0, (SF16[y][x]) ? 255 : 0), shift16);
    }
  }
  shift16 += speed16;
  if (shift16 >= 255) {
    memcpy(FF16, SF16, WIDTH * HEIGHT);
    for (byte y = 0; y < HEIGHT; y++) { // (byte x = 0; x < WIDTH; x++)
      for (byte x = 0; x < WIDTH; x++) { // (byte y = 0; y < HEIGHT; y++)
        if (SF16[y][x] && y)
        {
          SF16[y][x - 1] = 255;
          SF16[y][x] = 0;
        }
        else if (SF16[y][x] && !y) {
          SF16[y][x] = 0;
        }
      }
    } if (random() % 2)SF16[random(0, WIDTH)][HEIGHT - 1] = 255;
    shift16 %= 255;
  }
}
