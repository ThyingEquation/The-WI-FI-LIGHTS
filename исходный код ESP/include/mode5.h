#ifndef MODE5_H
#define MODE5_H

#include "main.h"

extern const uint32_t colorsPixel[];
void mode5(String choosenMode);

/////////////////////////////////////////////////////// слайда

void drawPixelXYF5(float x, float y, CRGB color);
void shift();
void clearing();
void SmokeBallsRoutine();

#endif