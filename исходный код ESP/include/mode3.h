#ifndef MODE3_H
#define MODE3_H

#include "main.h"

void mode3();
void fillnoise8();
void mapNoiseToLEDsUsingPalette();
void ChangePaletteAndSettingsPeriodically();
void SetupRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);

#endif