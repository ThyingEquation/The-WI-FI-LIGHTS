#ifndef COLORFULSPOTS_H
#define COLORFULSPOTS_H

#include "main.h"

void colorfulSpots();
void fillnoise8();
void mapNoiseToLEDsUsingPalette();
void ChangePaletteAndSettingsPeriodically();
void SetupRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);

#endif