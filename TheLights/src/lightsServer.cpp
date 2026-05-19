#include <string>

#include <WiFiUdp.h>

#include "lightsServer.h"
#include "lightsSettings.h"
#include "effects.h"

static void handleConnectionState(std::string_view command);
static void handleSettings(std::string_view command);
static void handleCanvas(std::string_view command);
static void handleMainCommand(std::string_view command);

WiFiUDP Udp;

static bool isCommandReceived = false;

void initLightServer()
{
  WiFi.mode(WIFI_AP);
  delay(50);

  IPAddress localIp(settings.localIpVal[0], settings.localIpVal[1], settings.localIpVal[2], settings.localIpVal[3]);
  IPAddress gateway(settings.gatewayVal[0], settings.gatewayVal[1], settings.gatewayVal[2], settings.gatewayVal[3]);
  IPAddress subnet(settings.subnetVal[0], settings.subnetVal[1], settings.subnetVal[2], settings.subnetVal[3]);
  delay(50);

  WiFi.softAPConfig(localIp, gateway, subnet);
  delay(50);
  WiFi.softAP(settings.ssid, settings.password);
  delay(50);

  Udp.begin(1653);
}

void checkLightServer()
{

  constexpr std::array<std::string_view, 4> commands = {"connectionState?", "settings?", "canvas?", "command?"};

  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    char incomingPacket[256];
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }

    std::string_view command = std::string_view(incomingPacket);

    if (command.rfind(commands[0]) == 0)
    {
      handleConnectionState(command.substr(commands[0].length()));
    }
    else if (command.find(commands[1]) == 0)
    {
      handleSettings(command.substr(commands[1].length()));
    }
    else if (command.find(commands[2]) == 0)
    {
      handleCanvas(command.substr(commands[2].length()));
    }
    else if (command.find(commands[3]) == 0)
    {
      handleMainCommand(command.substr(commands[3].length()));
    }
    else
    {
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write("Error main");
      Udp.endPacket();
    }
  }
}

bool checkCommandReceived()
{
  if (isCommandReceived)
  {
    isCommandReceived = false;
    return true;
  }
  else
  {
    return false;
  }
}

void sendVirtualCommand()
{
  isCommandReceived = true;
}

static void handleConnectionState(std::string_view command)
{
  if (command == "check")
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("imHere");
    Udp.endPacket();
  }
}

static void handleSettings(std::string_view command)
{

  constexpr std::array<std::string_view, 8> commands = {"ssid=", "startingEffect=", "brightness=", "restart",
                                                        "allModesTime=", "allModes=", "wifiOff", "wifiAutoOff="};

  bool isFailed = false;

  if (command.rfind(commands[0]) == 0)
  {
    command.remove_prefix(commands[0].length());
    size_t len = std::min(command.size(), size_t(32));
    command.copy(settings.ssid, len);
    settings.ssid[len] = '\0';
  }
  else if (command.rfind(commands[1]) == 0)
  {
    command.remove_prefix(commands[1].length());
    size_t separatorPos = command.find('-');
    if (separatorPos != std::string_view::npos)
    {
      uint32_t groupTemp = 0, submodeTemp = 0;
      if (sscanf(std::string(command.substr(0, separatorPos)).c_str(), "%u", &groupTemp) != 1 ||
          sscanf(std::string(command.substr(separatorPos + 1)).c_str(), "%u", &submodeTemp) != 1)
      {
        isFailed = true;
      }
      else if (groupTemp > 255 || submodeTemp > 255)
      {
        isFailed = true;
      }
      else
      {
        settings.startingEffectsGroup = static_cast<uint8_t>(groupTemp);
        settings.startingEffectSubmode = static_cast<uint8_t>(submodeTemp);
      }
    }
  }
  else if (command.rfind(commands[2]) == 0)
  {
    command.remove_prefix(commands[2].length());
    uint32_t val = 0;
    if (sscanf(std::string(command).c_str(), "%u", &val) != 1)
    {
      isFailed = true;
    }
    else if (val > 255)
    {
      isFailed = true;
    }
    else
    {
      settings.globalBrightness = static_cast<uint8_t>(val);
    }
  }
  else if (command.rfind(commands[3]) == 0)
  {
    saveSettings();
    ESP.restart();
  }
  else if (command.rfind(commands[4]) == 0)
  {
    command.remove_prefix(commands[4].length());
    uint32_t allModeDelayTemp = 0;
    if (sscanf(std::string(command).c_str(), "%u", &allModeDelayTemp) != 1)
    {
      isFailed = true;
    }
    else if (allModeDelayTemp > 3600)
    {
      isFailed = true;
    }
    else
    {
      settings.allModeDelay = allModeDelayTemp * 1000;
    }
  }
  else if (command.rfind(commands[5]) == 0)
  {
    command.remove_prefix(commands[5].length());
    uint32_t allModesWorkTypeTemp = 0;
    if (sscanf(std::string(command).c_str(), "%u", &allModesWorkTypeTemp) != 1)
    {
      isFailed = true;
    }
    else if (allModesWorkTypeTemp > 1)
    {
      isFailed = true;
    }
    else
    {
      settings.allModesWorkType = static_cast<uint8_t>(allModesWorkTypeTemp);
    }
  }
  else if (command.rfind(commands[6]) == 0)
  {
    deviceEffectsState.isWifiActive = false;
    WiFi.softAPdisconnect(true);
  }
  else if (command.rfind(commands[7]) == 0)
  {
    command.remove_prefix(commands[7].length());
    uint32_t isWifiAutoOffEnableTemp = 0;
    if (sscanf(std::string(command).c_str(), "%u", &isWifiAutoOffEnableTemp) != 1)
    {
      isFailed = true;
    }
    else if (isWifiAutoOffEnableTemp > 1)
    {
      isFailed = true;
    }
    else
    {
      settings.isWifiAutoOffEnable = static_cast<bool>(isWifiAutoOffEnableTemp);
    }
  }
  else
  {
    isFailed = true;
  }

  if (isFailed)
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Error settings");
    Udp.endPacket();
  }
  else
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Setting ok");
    Udp.endPacket();
  }
}

static void handleCanvas(std::string_view command)
{
  std::string_view modeString = "";
  isCommandReceived = true;
  bool isFailed = false;

  size_t modeStart = command.find("mode=");
  size_t modeEnd = command.find('&', modeStart);

  if (modeStart == std::string_view::npos)
  {
    isFailed = true; /* return */
  }
  else
  {
    modeString = command.substr(modeStart + 5, modeEnd - (modeStart + 5));
  }

  size_t colorStart = command.find("color=");
  uint8_t color = 0;
  if (colorStart == std::string_view::npos)
  {
    isFailed = true;
  }
  else
  {
    size_t colorEnd = command.find('&', colorStart);
    std::string_view colorString = command.substr(colorStart + 6, colorEnd - (colorStart + 6));
    uint32_t colorTemp = 0;
    if (sscanf(std::string(colorString).c_str(), "%u", &colorTemp) != 1)
    {
      isFailed = true;
    }
    else if (colorTemp > 20)
    {
      isFailed = true;
    }
    else
    {
      color = static_cast<uint8_t>(colorTemp);
    }
  }

  size_t ledStart = command.find("led=");
  uint16_t ledNum = 0;
  if (ledStart == std::string_view::npos)
  {
    isFailed = true;
  }
  else
  {
    std::string_view ledString = command.substr(ledStart + 4);
    uint32_t ledNumTemp = 0;
    if (sscanf(std::string(ledString).c_str(), "%u", &ledNumTemp) != 1)
    {
      isFailed = true;
    }
    else if (ledNumTemp > 257)
    {
      isFailed = true;
    }
    else
    {
      ledNum = static_cast<uint16_t>(ledNumTemp);
    }
  }

  if (isFailed)
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Error canvas");
    Udp.endPacket();
  }
  else
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Ok");
    Udp.endPacket();

    drawOnCanvas(modeString, color, ledNum);
  }
}

static void handleMainCommand(std::string_view command)
{
  isCommandReceived = true;

  bool isFailed = false;

  deviceEffectsState.isScreenClearEnable = true;
  deviceEffectsState.isAllModesEnable = false;

  if (command.rfind("stop") == 0)
  {
    deviceEffectsState.isScreenClearEnable = true;
    deviceEffectsState.effectsGroup = 255;
  }
  else if (command.find("-") != std::string_view::npos)
  {
    size_t separatorPos = command.find('-');
    if (separatorPos != std::string_view::npos)
    {
      uint32_t groupTemp = 0, submodeTemp = 0;
      if (sscanf(std::string(command.substr(0, separatorPos)).c_str(), "%u", &groupTemp) != 1 ||
          sscanf(std::string(command.substr(separatorPos + 1)).c_str(), "%u", &submodeTemp) != 1)
      {
        isFailed = true;
      }
      else if (groupTemp > 255 || submodeTemp > 255)
      {
        isFailed = true;
      }
      else
      {
        deviceEffectsState.effectsGroup = static_cast<uint8_t>(groupTemp);
        deviceEffectsState.effectSubmode = static_cast<uint8_t>(submodeTemp);
        if (deviceEffectsState.effectsGroup == 10)
        {
          drawAnimations(255);
        }
      }
    }
  }
  else
  {
    isFailed = true;
  }

  if (isFailed)
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Error commands");
    Udp.endPacket();
  }
  else
  {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Ok");
    Udp.endPacket();
  }
}