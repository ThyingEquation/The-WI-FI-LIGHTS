#ifndef MODE10_H
#define MODE10_H

#include "main.h"

void mode10(String choosenMode);

/////////////////////////// автоматическое отображение

void regB1(byte id);
void phisicsB1(byte id);
void wu_pixel(uint32_t x, uint32_t y, CRGB * col);
void render(byte id, CRGB Col);
void setGenPos(int x, int y);
void setGravityPos(int x, int y);
void setRegenRule(byte id, bool b);
void Start();
void drawFire();
void drawGravityDemo();
void drawBengalFire();
void drawWind();

////////////////////////////////////////////////////// кривая

void drawPixelXYFB3(float x, float y, const CRGB & color);
void drawCurve(float x,float y, float x2,float y2,float x3,float y3,CRGB col);
void drawB3();

////////////////////////////////////////////////////// паутина

void drawLine(int x1, int y1, int x2, int y2,  const CRGB & color);
void draw13();

////////////////////////////////////////////////////// метель

void snow();

#endif