#ifndef MODE9_H
#define MODE9_H

#include "main.h"

void mode9(String choosenMode);

/////////////////////////////////// лагуна 

void pacifica_loop();
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff);
void pacifica_add_whitecaps();
void pacifica_deepen_colors();

/////////////////////////////////// штиль + дождь

void fillAll(CRGB color);
void drawPixelXY(uint8_t x, uint8_t y, CRGB color);
void N();
void PoolNoise();
void drawCircle(int x0, int y0, int radius, const CRGB &color);
void drop();

#endif