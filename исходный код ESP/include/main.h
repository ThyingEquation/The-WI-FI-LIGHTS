/*
* ==BARANOV KIRILL==*
REMOTE CODES: 
* A, B - ON/OFF
* C ... L - COLORS
* M - BRIGHTNESS [M50 - brightness 50 percent]
* N ... R - COLORS
* S - MODE1
* T ... Z - MODE2-MODE8 

======================================================
======================================================
=== C - WHITE [CODE FOR START MODE: 100]           ===
=== D - BLUE [CODE FOR START MODE: 101]            ===
=== E - LIME GREEN [CODE FOR START MODE: 102]      ===
=== F - YELLOW [CODE FOR START MODE: 103]          ===
=== G - ORANGE [CODE FOR START MODE: 104]          === 
=== H - RED [CODE FOR START MODE: 105]             ===
=== I - RASPBERRY COLOR [CODE FOR START MODE: 106] ===
=== J - VIOLET [CODE FOR START MODE: 107]          ===
=== K - SPRING-GREEN [CODE FOR START MODE: 108]    ===
=== L - GOLD [CODE FOR START MODE: 109]            ===
=== N - BRIGHT BLUE [CODE FOR START MODE: 110]     ===
=== O - AQUAMARINE [CODE FOR START MODE: 111]      ===
=== P - INDIGO [CODE FOR START MODE: 112]          ===
=== Q - BRIGHT PINK  [CODE FOR START MODE: 113]    ===
=== R - GREEN [CODE FOR START MODE: 114]           ===
=======================================================
=======================================================

TYPICAL (EMPTY) INPUT COMMAND: "GET / HTTP/1.1"
TYPICAL INPUT COMMAND: "GET /T HTTP/1.1"

*/

#ifndef MAIN_H
#define MAIN_H

 #include <Arduino.h>

 #include <FastLED.h>
 #define FASTLED_ALLOW_INTERRUPTS 0
 FASTLED_USING_NAMESPACE
 #include <ESP8266WiFi.h>
 #include <Adafruit_GFX.h>
 #include <Adafruit_NeoMatrix.h>
 #include <Adafruit_NeoPixel.h>  

 #include <ArduinoJson.h> //.................................... CHANGE SSID {COMMAND}
#include "FS.h" //.................................... CHANGE SSID {COMMAND}

 #include "mode1.h"
 #include "mode2.h"
 #include "mode3.h"
 #include "mode4.h"
 #include "mode5.h"
 #include "mode6.h"
 #include "mode7.h"
 #include "mode8.h"
 #include "mode9.h"
 #include "mode10.h"
 #include "mode11.h"


// 11 matrix 16x16 text inside
// 12 matrix 16x16 text outside

// 21 matrix 12x13 text inside
// 22 matrix 12x13 text outside

// LINE18 линия из 18 светодиодов

 #define MATRIX_SIZE_TYPE 22 


 extern int BRIGHTNESS;

 #if MATRIX_SIZE_TYPE == 11
  #define NUMPIXELS 256 
 extern uint8_t noise[][16];

#elif MATRIX_SIZE_TYPE == 12
  #define NUMPIXELS 256 
 extern uint8_t noise[][16];

#elif MATRIX_SIZE_TYPE == 21
  #define NUMPIXELS 156 
    extern uint8_t noise[][13];

#elif MATRIX_SIZE_TYPE == 22
  #define NUMPIXELS 156 
   extern uint8_t noise[][13];

#elif MATRIX_SIZE_TYPE == LINE18
  #define NUMPIXELS 18 
   extern uint8_t noise[][18];
   #endif

 #define PIN 4                             
 #define LED_TYPE WS2812B
 #define COLOR_ORDER GRB
 #define VOLTS          5
 #define MAX_MA       5000

 extern const uint8_t kMatrixWidth;
 extern const uint8_t kMatrixHeight;

 extern const bool kMatrixSerpentineLayout;  

 extern uint16_t X;
 extern  uint16_t Y;
 extern  uint16_t Z;

 extern CRGB gBackgroundColor;
 extern CRGBPalette16 gCurrentPalette;  
 extern CRGBPalette16 gTargetPalette;  
 extern CRGBPalette16 currentPalette;

 extern Adafruit_NeoPixel strip;
 extern Adafruit_NeoMatrix matrix;

 extern const uint16_t colorsMatrix[];

 extern uint16_t XY(uint8_t X, uint8_t Y);

 extern CRGB* const leds;
 extern String req;
 extern int pass;
 extern unsigned int b;
 extern int g;
 extern int col;
 extern unsigned int l;

 extern CRGBArray <NUMPIXELS> Leds;


 void modes(String req);

#endif