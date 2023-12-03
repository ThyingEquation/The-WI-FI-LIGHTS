#ifndef MODE6_H
#define MODE6_H

#include "main.h"
#include "mode5.h"


void mode6(String choosenMode);

//////////////////////////// лэнгтон

void move(byte id);
void check2(byte id);
void check3(byte id);
void check1(byte id);
void draw3();

//////////////////////////// блуждающие огоньки 

void drawPixelXYF(float x, float y, CRGB color);
void draw4() ;

/////////////////////////// прыгающие огоньки

void jumplight();

/////////////////////////// бегущий цветной огонеК

void runningColorLED ();

///////////////////////////

#endif