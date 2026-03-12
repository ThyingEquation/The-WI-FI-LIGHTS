#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

extern ESP8266WebServer server;

void handleSettings();
void handleDrawImage();
void handlePainting();
void handleMainCommand();
void effectAllModes();

extern uint8_t allModesEnable;
extern uint8_t effectsGroup;
extern uint8_t submode;
extern uint8_t currentIndex;