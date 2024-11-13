#ifndef RUNNINGLIGHTS_H
#define RUNNINGLIGHTS_H

#include "draw.h"
#include "main.h"

extern XYMap xyMap;

uint16_t XY6(uint8_t x, uint8_t y);

void runningLights();

void move(byte id);
void check2(byte id);
void check3(byte id);
void check1(byte id);
void draw3R();
void drawPixelXYF(float x, float y, CRGB color);
void draw4R();
void jumplight();
void runningColorLED();
void theaterChase(uint32_t c);
void theaterChaseRainbow();
void drawB3();
void draw5R();
void draw6R();
//void snake(uint32_t color, uint8_t wait);
void snake();

extern int col;

#endif