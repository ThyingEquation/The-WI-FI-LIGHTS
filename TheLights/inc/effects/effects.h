#pragma once

#include <string_view>
#include <ESP8266TrueRandom.h>

#include "main.h"

void stripShow();
bool checkCommandReceived();

uint16_t XY(uint16_t x, uint16_t y);
void drawPicture(const uint32_t* p2);
void drawPixelXYFB3(float x, float y, const CRGB &color);
CRGB customWheel(byte WheelPos);

void initRunningLine();

void drawColorfulEffects(uint8_t subMode);
void drawAnimations(uint8_t subMode);
void drawOnCanvas(std::string_view mode, uint8_t color, uint16_t ledNum);
void drawCanvasEffects(uint8_t subMode);
void drawFlickeringLights(uint8_t subMode);
void drawGamesEffects(uint8_t subMode);
void drawJumpingLights(uint8_t subMode);
void drawRunningLights(uint8_t subMode);
void drawRunningLine(uint8_t subMode);
void drawSpaceEffects(uint8_t subMode);
void drawWaterEffects(uint8_t subMode);
void drawWeatherEffects(uint8_t subMode);

extern const uint8_t mainMatrixScheme[];