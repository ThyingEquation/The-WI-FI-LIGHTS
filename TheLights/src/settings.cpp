#include <ArduinoJson.h>

#include "settings.h"

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

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error)
  {
    return 1;
  }

  appSettingsData = doc["ssid"];
  if (appSettingsData)
  {
    strncpy(settings.ssid, appSettingsData, 15);
  }

  appSettingsData = doc["startMode"];
  if (appSettingsData)
  {
    settings.modeNum = atoi(appSettingsData);
  }

  appSettingsData = doc["brightness"];
  if (appSettingsData)
  {
    settings.intBrightness = atoi(appSettingsData);
  }

  appSettingsData = doc["allModeDelay"];
  if (appSettingsData)
  {
    char allModeDelayString[5] = {'0', '0', '0', '0', '\0'};
    strncpy(allModeDelayString, appSettingsData, 4);
    localStr = allModeDelayString;
    settings.allModeDelay = (unsigned long)(localStr.toInt()) * 1000;
  }

  appSettingsData = doc["allModeType"];
  if (appSettingsData)
  {
    settings.allModesWorkType = atoi(appSettingsData);
  }

  appSettingsData = doc["isWifiAutoOffEnable"];
  if (appSettingsData)
  {
    settings.isWifiAutoOffEnable = atoi(appSettingsData);
  }

  return 0;
}

static void successSave() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0xffffff);
  }
  strip.show();
  delay(950);
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0x000000);
  }
  strip.show();
}

bool applyNewParameters(String paramData, uint8_t param) {

  int paramDataLen = paramData.length() + 1;
  char webParamData[paramDataLen];
  paramData.toCharArray(webParamData, paramDataLen);

  JsonDocument json;

  switch (param) {
    case 1:
      json["ssid"] = paramData;
      break;

    case 2:
      json["startMode"] = paramData;
      break;

    case 3:
      json["brightness"] = paramData;
      break;

    case 4: {
      json["allModeDelay"] = paramData;
      settings.allModeDelay = ((unsigned long)(paramData.toInt()))*1000;
    } break;

    case 5:
      json["allModeType"] = paramData;
      break;

    case 6:
      json["isWifiAutoOffEnable"] = paramData;
      break;
  }

  File configFile = LittleFS.open("/appSettings.json", "w");
  if (!configFile) {
    return 1;
  }

  serializeJson(json, configFile);
  configFile.close();

  successSave();

  return 0;
}