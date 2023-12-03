#ifndef MODE7_H
#define MODE7_H

#include "main.h"

void mode7(String choosenMode);

///////////////////////////////////////////////////////// космические корабли

void drawPixel(byte x, byte y, CRGB color);
void MoveX(int8_t delta);
void MoveY(int8_t delta);
void spaceship();

////////////////////////////////////////////////////////// солнечная система

void SolarSystem();
#endif