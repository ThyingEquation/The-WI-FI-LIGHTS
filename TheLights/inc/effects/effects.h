#pragma once

#include <string_view>
#include <ESP8266TrueRandom.h>

#include "main.h"

void drawAnimations(uint8_t subMode);
void drawOnCanvas(std::string_view mode, uint8_t color, uint16_t ledNum);
void drawColorfulSpots(uint8_t empty);
void drawColorfulWaves(uint8_t subMode);
void drawCanvasEffects(uint8_t subMode);
void drawFlashLights(uint8_t subMode);
void drawGamesEffects(uint8_t subMode);
void drawJumpingLights(uint8_t subMode);
void drawMatrixMovieEffect(uint8_t subMode);
void drawRainbows(uint8_t subMode);
void drawRunningLights(uint8_t subMode);
void drawRunningLine(uint8_t subMode);
void drawSpaceEffects(uint8_t subMode);
void drawWaterEffects(uint8_t subMode);
void drawWeatherEffects(uint8_t subMode);

void initRunningLine();
bool checkCommandReceived();

uint16_t XY(uint16_t x, uint16_t y);
CRGB Wheel(byte WheelPos);
void drawPicture(uint8_t p1[], const uint32_t p2[]);

extern uint8_t mainMatrixScheme[];

struct ModeConfig {
    uint8_t effectsGroup;
    uint8_t subMode; 
};

extern const std::array<ModeConfig, ALL_MODES_EFFECT_COUNT> mainModes;