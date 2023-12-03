#include <mode6.h>

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define MATRIX_TYPE 1

#define n ((256 > 128) ? 256 / 128 : 1 ) ////////////////////////////////////////////////////
#define restart (10) // sec
bool setUp = true; 
int8_t pos[2][n];
byte dir[n];
byte hue6;
bool fade = false;



uint16_t XY6 (uint8_t x, uint8_t y) { //X5 и Х3
  return (y * kMatrixWidth + x);
}



void mode6(String choosenMode) {
  if (choosenMode.indexOf("/X1") != -1) {
    if (l <= 256) {
      strip.setPixelColor(l, colorsPixel[col]);
      strip.show();
      strip.setPixelColor(l - 1, strip.Color(0, 0, 0));
      strip.show();
      delay(100);
      l++;
    } else {
     // if (++col >= 128) { pass = 0; }
     col = random(127);
      l = 0;
    }
  }

    if (choosenMode.indexOf("/X2") != -1) {
    if (l <= 256) {
      strip.setPixelColor(l, colorsPixel[col]);
      strip.show();
      strip.setPixelColor(l - 1, strip.Color(0, 0, 0));
      strip.show();
      delay(10);
      l++;
    } else {
     // if (++col >= 128) { pass = 0; }
     col = random(127);
      l = 0;
    }
  }

      if (choosenMode.indexOf("/X3") != -1) {
        
  draw3();
  FastLED.show();
  }

      if (choosenMode.indexOf("/X4") != -1) {
        
  draw4();
  FastLED.show();
  }

        if (choosenMode.indexOf("/X5") != -1) {
          
  jumplight();
  }

        if (choosenMode.indexOf("/X6") != -1) {
          FastLED.clear();
  runningColorLED ();
  }




  

}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// лэнгтон /Х3

void move(byte id) {
  switch (dir[id]) {
    case 0: pos[1][id]++; break; //up
    case 1: pos[0][id]++; break; //right
    case 2: pos[1][id]--; break; //down
    case 3: pos[0][id]--; break; //left
  }
}
void check2(byte id) {
  if (leds[XY6(pos[0][id], pos[1][id])] == CRGB(0, 0, 0))dir[id]++; else dir[id]--;
  if (dir[id] > 3) dir[id] = 0; else if (dir[id] < 0) dir[id] = 3;
}
void check3(byte id) {
 if(pos[0][id]> kMatrixHeight-1) pos[0][id]=0;
 if(pos[1][id]> kMatrixWidth-1) pos[1][id]=0;
 if(pos[0][id]< 0) pos[0][id]=kMatrixHeight-1;
 if(pos[1][id]< 0) pos[1][id]=kMatrixWidth-1;
}
void check1(byte id) {
  if (leds[XY6(pos[0][id], pos[1][id])] == CRGB(0, 0, 0))leds[XY(pos[0][id], pos[1][id])] = CHSV(hue6, 255, 255);else leds[XY(pos[0][id], pos[1][id])] = 0;}

void draw3() {
  if(setUp){
    setUp = false;
    FastLED.clear();
    for(byte i=0; i<n; i++){
      pos[0][i] = random(0,kMatrixHeight);
      pos[1][i] = random(0,kMatrixWidth);
      dir[i] = random(0, 3);
    }
  }

  for(byte i=0; i<n; i++) {
  check1(i);
  move(i);
  check3(i);
  check2(i);}
  hue6++;
  if(fade)fadeToBlackBy(leds, NUM_LEDS, 5);
  EVERY_N_SECONDS (restart){
   setUp = true; 
  }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// блуждающие огоньки /Х4

#define Speed 255
#define Scale 8
#define Run 3 // 0-AllIn1Direction/1-beatsin/2-linear/3-circular
#define col 1 //0-Whited/1-Colored/2-fromPallete
#define trace 2 //0-None/1-DimAll/2-Blur2D/3-wings
#define reseting 10 //reset time in seconds, 0-off
#define subPixel 1 //
#define broad 1 //
//---------------
//#define LIGHTERS_AM kMatrixWidth + kMatrixHeight

#define LIGHTERS_AM 16 + 16
int lightersPosX6[LIGHTERS_AM];
int lightersPosY6[LIGHTERS_AM];
uint16_t lightersSpeedX6[LIGHTERS_AM];
uint16_t lightersSpeedY6[LIGHTERS_AM];
byte lightersSpeedZ[LIGHTERS_AM];
byte lcolor6[LIGHTERS_AM];
byte mass6[LIGHTERS_AM];
bool loadingFlag6 = true;


uint16_t XY3 (uint8_t x, uint8_t y) {

  if ((y % 2 == 0) || MATRIX_TYPE)                    
  {
    return ((uint32_t)y * kMatrixWidth + x) % (kMatrixWidth * kMatrixHeight);
  }
  else                                                      
  {
    return ((uint32_t)y  * kMatrixWidth + kMatrixWidth - x - 1) % (kMatrixWidth * kMatrixHeight);
  }
}


void drawPixelXYF(float x, float y, CRGB color) {
  // if (x < 0 || y < 0 || x > ((float)kMatrixWidth - 1) || y > ((float)kMatrixHeight - 1)) return;
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
    CRGB clr = leds[XY(xn, yn)];
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    leds[XY(xn, yn)] = clr;
  }
}

void draw4() {
  if (loadingFlag6) {
    loadingFlag6 = false;
    randomSeed(millis());
    for (byte i = 0; i < LIGHTERS_AM; i++) {
      if (Run == 2) {
        lightersSpeedX6[i] = random(-10, 10);
        lightersSpeedY6[i] = random(-10, 10);
      } else if (Run == 3) {
        lightersSpeedX6[i] = random(-10, 10); //angle speed
        lightersSpeedY6[i] = random(0, 360); //angle
        mass6[i] = random(5, 10); //vspeed
      } else {
        lightersSpeedX6[i] = random(3, 25);
        lightersSpeedY6[i] = random(3, 25);
        mass6[i] = random(15, 100);
      }
      lightersSpeedZ[i] = random(3, 25);
      lightersPosX6[i] = random(0, kMatrixWidth * 10);
      lightersPosY6[i] = random(0, kMatrixHeight * 10);
      lcolor6[i] = random(0, 9) * 28;
    }
  }
  switch (trace) {
    case 0:
      FastLED.clear();
      break;
    case 1:
      fadeToBlackBy(leds, NUM_LEDS, 50);
      break;
    case 2:
      blur2d(leds, kMatrixWidth, kMatrixHeight, 30);
      fadeToBlackBy(leds, NUM_LEDS, 5);
      break;
    case 3:
      fadeToBlackBy(leds, NUM_LEDS, 200);
      break;
  }

  for (byte i = 0; i < map(Scale, 1, 16, 2, LIGHTERS_AM); i++) {
    lcolor6[i]++;
    switch (Run) {
      case 0:
        lightersPosX6[i] += beatsin88(lightersSpeedX6[0] * Speed, 0, mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 8)) - mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 16);
        lightersPosY6[i] += beatsin88(lightersSpeedY6[0] * Speed, 0, mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 8)) - mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 16);
        break;
      case 1:
        if (broad) {
          lightersPosX6[i] = beatsin16(lightersSpeedX6[i] / map(Speed, 1, 255, 10, 1), 0, (kMatrixWidth - 1) * 10);
          lightersPosY6[i] = beatsin16(lightersSpeedY6[i] / map(Speed, 1, 255, 10, 1), 0, (kMatrixHeight - 1) * 10);
        } else {
          lightersPosX6[i] += beatsin16(lightersSpeedX6[i] / map(Speed, 1, 255, 10, 1), 0, mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 8)) - mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 16);
          lightersPosY6[i] += beatsin16(lightersSpeedY6[i] / map(Speed, 1, 255, 10, 1), 0, mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 8)) - mass6[i] / 10 * ((kMatrixHeight + kMatrixWidth) / 16);
        }
        break;
      case 2:
        lightersPosX6[i] += lightersSpeedX6[i] / map(Speed, 1, 255, 10, 1);
        lightersPosY6[i] += lightersSpeedY6[i] / map(Speed, 1, 255, 10, 1);
        break;
      case 3:
        lightersPosX6[i] += mass6[i] * cos(radians(lightersSpeedY6[i])) / map(Speed, 1, 255, 10, 1);
        lightersPosY6[i] += mass6[i] * sin(radians(lightersSpeedY6[i])) / map(Speed, 1, 255, 10, 1);
        lightersSpeedY6[i] += lightersSpeedX6[i] / map(Speed, 1, 255, 20, 2);
        break;
    }
    if (broad) {
      if (Run == 3) {
        if (lightersPosY6[i] < 0) {
          lightersPosY6[i] = 1;
          lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
        }
        if (lightersPosX6[i] < 0) {
          lightersPosX6[i] = 1;
          lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
        }
        if (lightersPosY6[i] >= (kMatrixHeight - 1) * 10) {
          lightersPosY6[i] = ((kMatrixHeight - 1) * 10) - 1;
          lightersSpeedY6[i] = 360 - lightersSpeedY6[i];
        }
        if (lightersPosX6[i] >= (kMatrixWidth - 1) * 10) {
          lightersPosX6[i] = ((kMatrixWidth - 1) * 10) - 1;
          lightersSpeedY6[i] = 180 - lightersSpeedY6[i];
        }
      } else if (Run == 1) {} else {
        if ((lightersPosX6[i] <= 0) || (lightersPosX6[i] >= (kMatrixWidth - 1) * 10)) lightersSpeedX6[i] = -lightersSpeedX6[i];
        if ((lightersPosY6[i] <= 0) || (lightersPosY6[i] >= (kMatrixHeight - 1) * 10)) lightersSpeedY6[i] = -lightersSpeedY6[i];
      }
    } else {
      if (lightersPosX6[i] < 0) lightersPosX6[i] = (kMatrixWidth - 1) * 10;
      if (lightersPosX6[i] > (kMatrixWidth - 1) * 10) lightersPosX6[i] = 0;
      if (lightersPosY6[i] < 0) lightersPosY6[i] = (kMatrixHeight - 1) * 10;
      if (lightersPosY6[i] > (kMatrixHeight - 1) * 10) lightersPosY6[i] = 0;
    }
    CRGB color = 0;
    switch (col) {
      case 0:
        color = CHSV(lcolor6[i], 40, (trace == 3) ? 128 + random8(2) * 111 : beatsin8(lightersSpeedZ[i] / map(Speed, 1, 255, 10, 1), 128, 255));
        break;
      case 1:
        color = CHSV(lcolor6[i], 255, (trace == 3) ? 128 + random8(2) * 111 : beatsin8(lightersSpeedZ[i] / map(Speed, 1, 255, 10, 1), 128, 255));
        break;
      case 2:
        color = ColorFromPalette(PartyColors_p, lcolor6[i], (trace == 3) ? 128 + random8(2) * 111 : beatsin8(lightersSpeedZ[i] / map(Speed, 1, 255, 10, 1), 128, 255));
        break;
    }
    if (subPixel)
      drawPixelXYF((float) lightersPosX6[i] / 10, (float) lightersPosY6[i] / 10, color);
    else
      leds[XY(lightersPosX6[i] / 10, lightersPosY6[i] / 10)] = color;
  }
  if (reseting > 0) {
    EVERY_N_SECONDS(reseting) {
      randomSeed(millis());
      for (byte i = 0; i < map(Scale, 1, 16, 2, LIGHTERS_AM); i++) {
        if (Run == 2) {
          lightersSpeedX6[i] = random(-10, 10);
          lightersSpeedY6[i] = random(-10, 10);
        } else if (Run == 3) {
          lightersSpeedX6[i] = random(-10, 10);
          lightersSpeedY6[i] = random(0, 360);
          mass6[i] = random(5, 10);
        } else {
          lightersSpeedX6[i] = random(3, 25);
          lightersSpeedY6[i] = random(3, 25);
        }
        lightersSpeedZ[i] = random(3, 25);
        mass6[i] + random(-25, 25);
      }
    }
  }
  delay(16);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// прыгающие огоньки /Х5

void jumplight() {
  for(byte i=8;i--;) {
  leds[XY6(beatsin8(12+i,0,kMatrixWidth-1), beatsin8(15-i,0,kMatrixHeight-1))]=CHSV(beatsin8(12,0,255),255,255);
  blur2d(leds,kMatrixWidth,kMatrixHeight,16);
  }
  FastLED.show();
} //loop

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// цветной бегущий огонек /Х6

void runningColorLED () {
  Serial.print("я тут");
 for (int i = 0; i < NUM_LEDS; i++) {
   leds[i] = CHSV(i, 255, 255);
   FastLED.show();
   delay(20);
   leds[i] = CRGB::Black;
 }
 
 for (int i = 0; i < NUM_LEDS; i++) {
   leds[i] = CHSV(rand() % 256, 255, 255);
   FastLED.show();
   delay(20);
   leds[i] = CRGB::Black;
 }
}
