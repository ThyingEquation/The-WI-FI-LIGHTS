#ifndef SPACE_H
#define SPACE_H

#include "main.h"

void space();
void drawPixel(byte x, byte y, CRGB color);
void MoveX(int8_t delta);
void MoveY(int8_t delta);
void starSky ();
void spaceship();
void starFall();

void pulsar();
void drawVortex(int radius);
int xyToIndex(int x, int y);
uint32_t WheelS(byte WheelPos);

#endif