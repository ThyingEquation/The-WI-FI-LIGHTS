#include <mode4.h>

#define BRIGHTNESS 50
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)



// режим V1 вращающаяся радуга, V2 радуга с волнами, полосами и тп, V3 вертикальное переливание 
// радуги (1 проход слева на право), V4 змейка радуга (цикл 4 прохода)

void mode4(String choosenMode) {
  if (choosenMode.indexOf("/V1") != -1) {
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
    DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if( ms < 5000 ) {
      FastLED.setBrightness( scale8(BRIGHTNESS, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
  }
  if (choosenMode.indexOf("/V2") != -1) {
      rainbow2();
      FastLED.show();
  }
    if (choosenMode.indexOf("/V3") != -1) {
      rainbow();
  }

      if (choosenMode.indexOf("/V4") != -1) {
      	static uint8_t hue = 0;

	
	for(int i = 0; i < NUM_LEDS; i++) {
		
		leds[i] = CHSV(hue++, 255, 255);
		FastLED.show(); 
		fadeall();
		delay(10);
	}
	
	for(int i = (NUM_LEDS)-1; i >= 0; i--) {
		
		leds[i] = CHSV(hue++, 255, 255);
		FastLED.show();
		fadeall();
		delay(10);
	}
  }
}

// радуга2

void rainbow2() {

  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);


  }
}

// радуга3

void rainbow() {
  uint16_t i, j;
     
     for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(10);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//rainbow4
void fadeall() {

     for(int i = 0; i < NUM_LEDS; i++) {
         leds[i].nscale8(250);
          } 
          }

