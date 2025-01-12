/*
==BARANOV KIRILL==
ver 3.2.0 - 2025
*/

#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Arduino.h>
#include <FastLED.h>

#define TEXT_POS \
  NEO_MATRIX_LEFT  // NEO_MATRIX_LEFT - текст внутри === NEO_MATRIX_RIGHT -
                   // текст снаружи
#define mWidth 12
#define mHeight 13
#define NUM_LEDS mWidth* mHeight

extern Adafruit_NeoPixel strip;
extern Adafruit_NeoMatrix matrix;
extern CRGB* const leds;

int XY(int x, int y);
uint32_t Wheel(byte WheelPos);