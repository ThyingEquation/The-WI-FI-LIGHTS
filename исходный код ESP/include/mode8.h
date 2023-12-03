#ifndef MODE8_H
#define MODE8_H

#include "main.h"

CRGB computeOneTwinkle( uint32_t ms, uint8_t salt);
uint8_t attackDecayWave8( uint8_t i);

/////////////////////////////////////////////////////// мерцающие огни
void mode8(String choosenMode);
void drawTwinkles( CRGBSet& L);
void coolLikeIncandescent( CRGB& c, uint8_t phase);
void chooseNextColorPalette( CRGBPalette16& pal);

/////////////////////////////////////////////////////// мерцающие огни 2

void theaterChase(uint32_t c);

/////////////////////////////////////////////////////// мерцающие огни 3

void theaterChaseRainbow();

/////////////////////////////////////////////////////// сверкание

void sparkles();

#endif