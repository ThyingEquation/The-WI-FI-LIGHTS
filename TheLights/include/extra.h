#ifndef EXTRA_H
#define EXTRA_H

#include "main.h"

void extra();
void extra1();
void extra2();
void extra3();
void extra4();
void extra5();
void extra6();
void extra7();

void updateSquareColor();
void updateSquarePosition();
void drawSquare(int x, int y, CRGB color);
extern int XY(int x, int y);
void drawPoint(int x, int y, CRGB color);
void initPoints();
void drawSpiral();
void updateSnake();
void drawSnake();
void drawFood();
void spawnFood();
void drawFigure(int figureIndex, int x, int y, CRGB color);
bool checkCollision(int figureIndex, int x, int y);
void addNewFigure();
void drawTiles();
void movePlatform();
void moveBall();
void clearAll();

uint32_t HSVtoRGB(uint8_t hE, uint8_t sE, uint8_t vE);

extern const int ledMatrix[13][12];

#endif