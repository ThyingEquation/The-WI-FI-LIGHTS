#pragma once

#include <ESP8266WiFi.h>

void initLightServer();
void checkLightServer();
void sendVirtualCommand();

struct DeviceEffectsState {
    bool isScreenClearEnable = true;
    bool isAllModesEnable = false;
    bool isWifiActive = false;

    uint8_t effectsGroup = 255;
    uint8_t effectSubmode = 255;
    uint8_t currentIndex = 255;
};

extern DeviceEffectsState deviceEffectsState;
