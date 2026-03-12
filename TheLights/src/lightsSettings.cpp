#include <ArduinoJson.h>

#include "lightsSettings.h"
#include "main.h"

bool loadSettings()
{

  const char *appSettingsData = NULL;
  String localStr = "";

  File configFile = LittleFS.open("/appSettings.json", "r");
  if (!configFile)
  {
    return 1;
  }

  size_t size = configFile.size();
  if (size > 256)
  {
    configFile.close();
    return 1;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  size_t bytesRead = configFile.readBytes(buf.get(), size);
  configFile.close();

  if (bytesRead != size)
  {
    return 1;
  }

  JsonDocument json;
  DeserializationError errJson = deserializeJson(json, buf.get());
  if (errJson)
  {
    return 1;
  }

  appSettingsData = json["ssid"];
  if (appSettingsData)
  {
    strncpy(settings.ssid, appSettingsData, 15);
  }

  appSettingsData = json["startMode"];
  if (appSettingsData)
  {
    settings.modeNum = atoi(appSettingsData);
  }

  appSettingsData = json["brightness"];
  if (appSettingsData)
  {
    settings.intBrightness = atoi(appSettingsData);
  }

  appSettingsData = json["allModeDelay"];
  if (appSettingsData)
  {
    char allModeDelayString[5] = {'0', '0', '0', '0', '\0'};
    strncpy(allModeDelayString, appSettingsData, 4);
    localStr = allModeDelayString;
    settings.allModeDelay = (uint32_t)((localStr.toInt()) * 1000);
  }

  appSettingsData = json["allModeType"];
  if (appSettingsData)
  {
    settings.allModesWorkType = atoi(appSettingsData);
  }

  appSettingsData = json["isWifiAutoOffEnable"];
  if (appSettingsData)
  {
    settings.isWifiAutoOffEnable = atoi(appSettingsData);
  }

  return 0;
}

static void successSave()
{
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    strip.setPixelColor(i, 0xffffff);
  }
  strip.show();
  delay(250);
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    strip.setPixelColor(i, 0x000000);
  }
  strip.show();
}

bool applyNewParameters(String paramData, uint8_t param)
{
  StaticJsonDocument<512> json;

  File configFile = LittleFS.open("/appSettings.json", "r");
  if (configFile)
  {
    DeserializationError errJson = deserializeJson(json, configFile);
    if (errJson)
    {
      return 1;
    }
    configFile.close();
  }

  switch (param)
  {
  case 1:
    json["ssid"] = paramData;
    break;
  case 2:
    json["startMode"] = paramData;
    break;
  case 3:
    json["brightness"] = paramData;
    break;
  case 4:
    json["allModeDelay"] = paramData;
    break;
  case 5:
    json["allModeType"] = paramData;
    break;
  case 6:
    json["isWifiAutoOffEnable"] = paramData;
    break;
  }

  configFile = LittleFS.open("/appSettings.json", "w");
  if (!configFile)
  {
    return false;
  }

  serializeJson(json, configFile);
  configFile.close();

  if (param == 4)
  {
    settings.allModeDelay = ((uint32_t)json["allModeDelay"]) * 1000UL;
  }

  successSave();

  return true;
}