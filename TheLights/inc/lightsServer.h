#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

void initLightServer();
void checkLightServer();
void effectAllModes();

typedef struct
{
    bool isScreenClearEnable = true;
    bool isAllModesEnable = false;
    bool isWifiActive = 0;
    uint8_t effectsGroup = 255;
    uint8_t effectSubmode = 255;
    uint8_t currentIndex = 255;
} deviceEffectsState_s;

extern deviceEffectsState_s deviceEffectsState;