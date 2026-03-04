/*
==BARANOV KIRILL==
ver 4.x.x - 202x
*/

#pragma once

#include <ESP8266TrueRandom.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Arduino.h>
#include <FastLED.h>

enum {
  MATRIX_WIDTH = 12,
  MATRIX_HEIGHT = 13,
  MATRIX_LEDS = MATRIX_WIDTH * MATRIX_HEIGHT,
  TEXT_POS = NEO_MATRIX_LEFT // NEO_MATRIX_LEFT - текст внутри === NEO_MATRIX_RIGHT - текст снаружи
};

extern Adafruit_NeoPixel strip;
extern Adafruit_NeoMatrix matrix;
extern CRGB* const leds;

int XY(int x, int y);
uint32_t Wheel(byte WheelPos);