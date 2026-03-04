#pragma once

#include <../inc/main.h>

typedef struct {
  char ssid[15] = "Init name";
  const char *password = "11111111";

  uint8_t localIpVal[4] = {192, 168, 1, 1};
  uint8_t gatewayVal[4] = {192, 168, 1, 1};
  uint8_t subnetVal[4] = {255, 255, 255, 0};

  uint8_t intBrightness = 0;
  uint8_t allModesWorkType = 0;
  uint8_t modeNum = 0;
  bool isWifiAutoOffEnable = 0;
  unsigned long allModeDelay= 0;
} appSettings_s;

extern appSettings_s settings;

bool loadSettings();
bool applyNewParameters(String paramData, uint8_t param);