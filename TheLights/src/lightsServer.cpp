#include <deque>
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

enum EffectsGroupMode // Главные группы эффектов. Пародяк эффектов аналогичен андроид приложению
{
  CANVAS_EFFECTS,     // (0)  - Эффекты режима рисования
  RUNNING_LINE,       // (1)  - Бегущая строка (текст)
  COLORFUL_SPOTS,     // (2)  - Цветные пятна
  RAINBOWS,           // (3)  - Радуги
  RUNNING_LIGHTS,     // (4)  - Бегущие огни
  JUMPING_LIGHTS,     // (5) - Прыгающие огоньки
  SPACE_EFFECTS,      // (6)  - Космос
  FLASH_LIGHTS,       // (7)  - Мерцающие огни (вспышки)
  WATER_EFFECTS,      // (8)  - Водные эффекты
  WEATHER_EFFECTS,    // (9)  - Погодные эффекты
  COLORFUL_WAVES,     // (10)  - Цветные волны
  ANIMATIONS,         // (11) - Анимации
  GAMES_EFFECTS,      // (12) - Эффекты в виде игр
  MATRIX_MOVIE_EFFECT // (13) - Эффект из к/ф матрица
};

const std::array<ModeConfig, ALL_MODES_EFFECT_COUNT> mainModes = {{
    {CANVAS_EFFECTS, 3},   // [0.0]   - "Полная заливка"
    {CANVAS_EFFECTS, 1},   // [0.1]   - "Быстрая заливка змейкой"
    {CANVAS_EFFECTS, 2},   // [0.2]   - "Медленная заливка змейкой"
    {CANVAS_EFFECTS, 4},   // [0.3]   - "Цветная заливка змейкой"
    {CANVAS_EFFECTS, 5},   // [0.4]   - "Цветное дыхание"
    {CANVAS_EFFECTS, 255}, // [0.5]   - "Все картинки подряд"

    {COLORFUL_SPOTS, 255}, // [2.0]   - "Цветные пятна"

    {RAINBOWS, 1}, // [3.0]   - "Радуга колесо"
    {RAINBOWS, 2}, // [3.1]   - "Радуга волной 1"
    {RAINBOWS, 3}, // [3.2]   - "Радуга волной 2"
    {RAINBOWS, 4}, // [3.3]   - "Радуга змейкой"

    {RUNNING_LIGHTS, 1}, // [4.0]   - "Бегущий огонек медленно"
    {RUNNING_LIGHTS, 2}, // [4.1]   - "Бегущий огонек быстро"
    {RUNNING_LIGHTS, 3}, // [4.2]   - "Бегущий огонек цветной"
    {RUNNING_LIGHTS, 4}, // [4.3]   - "Бегущие огоньки 1"
    {RUNNING_LIGHTS, 5}, // [4.4]   - "Бегущие огоньки 2"
    {RUNNING_LIGHTS, 7}, // [4.5]   - "Бегущие огоньки 3"
    {RUNNING_LIGHTS, 6}, // [4.6]   - "Цветная змейка"

    {JUMPING_LIGHTS, 1}, // [5.0]  - "Прыгающие огоньки 1"
    {JUMPING_LIGHTS, 2}, // [5.1]  - "Прыгающие огоньки 2"
    {JUMPING_LIGHTS, 3}, // [5.2]  - "Прыгающие огоньки 3"
    {JUMPING_LIGHTS, 4}, // [5.3]  - "Прыгающие огоньки 4"
    {JUMPING_LIGHTS, 5}, // [5.4]  - "Прыгающий квадрат"
    {JUMPING_LIGHTS, 6}, // [5.5]  - "Прыгающие точки"

    {SPACE_EFFECTS, 2}, // [6.0]   - "Космические корабли"
    {SPACE_EFFECTS, 1}, // [6.1]   - "Звездное небо"
    {SPACE_EFFECTS, 3}, // [6.2]   - "Пульсирующая звезда"
    {SPACE_EFFECTS, 4}, // [6.3]   - "Метеорный поток"
    {SPACE_EFFECTS, 5}, // [6.4]   - "Спиральная туманность"

    {FLASH_LIGHTS, 1}, // [7.0]   - "Мерцающие огни 1"
    {FLASH_LIGHTS, 2}, // [7.1]   - "Мерцающие огни 2"
    {FLASH_LIGHTS, 3}, // [7.2]   - "Мерцающие огни 3"

    {WATER_EFFECTS, 1}, // [8.0]   - "Лагуна"
    {WATER_EFFECTS, 2}, // [8.1]   - "Бассейн"

    {WEATHER_EFFECTS, 1}, // [9.0]   - "Снегопад"
    {WEATHER_EFFECTS, 2}, // [9.1]   - "Метель"
    {WEATHER_EFFECTS, 3}, // [9.2]   - "Дождь"
    {WEATHER_EFFECTS, 4}, // [9.3]   - "Ливень"

    {COLORFUL_WAVES, 1}, // [10.0]   - "Цветные волны 1"
    {COLORFUL_WAVES, 2}, // [10.1]   - "Цветные волны 2"
    {COLORFUL_WAVES, 3}, // [10.2]   - "Цветные волны 3"
    {COLORFUL_WAVES, 4}, // [10.3]   - "Цветные волны 4"
    {COLORFUL_WAVES, 5}, // [10.4]   - "Цветные волны 5"

    {ANIMATIONS, 1},  // [11.0]  - "Анимации: Сердце"
    {ANIMATIONS, 2},  // [11.1]  - "Смайлик"
    {ANIMATIONS, 3},  // [11.2]  - "Прыгающий человечек"
    {ANIMATIONS, 4},  // [11.3]  - "Файербол"
    {ANIMATIONS, 5},  // [11.4]  - "Взрыв"
    {ANIMATIONS, 6},  // [11.5]  - "'С НОВЫМ ГОДОМ' на японском"
    {ANIMATIONS, 7},  // [11.6]  - "Приветствие на корейском"
    {ANIMATIONS, 8},  // [11.7]  - "Цифровой сигнал"
    {ANIMATIONS, 9},  // [11.8]  - "Синусоида"
    {ANIMATIONS, 10}, // [11.9]  - "Цветные синусоиды"
    {ANIMATIONS, 11}, // [11.10] - "Цветные линии 1"
    {ANIMATIONS, 12}, // [11.11] - "Цветные линии 2"
    {ANIMATIONS, 13}, // [11.12] - "Цветные линии 3"
    {ANIMATIONS, 14}, // [11.13] - "Цветные линии 4"

    {GAMES_EFFECTS, 1}, // [11.0]  - "Игра змейка"
    {GAMES_EFFECTS, 2}, // [11.1]  - "Игра тетрис"
    {GAMES_EFFECTS, 3}, // [11.2]  - "Игра арканоид"

    {MATRIX_MOVIE_EFFECT, 1} // [13.0]  - "Эффект из к/ф матрица"
}};

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
    char incomingPacket[255];
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

void effectAllModes()
{
  std::deque<uint8_t> usedEffects;

  static uint32_t prevTime = 0;
  static uint8_t randomCounter = 0;
  static uint32_t allModeDelayLocal = settings.allModeDelay;
  static uint8_t allModesWorkTypeLocal = settings.allModesWorkType;

  uint32_t currentTime = millis();

  if (deviceEffectsState.currentIndex == 255 || (currentTime - prevTime >= allModeDelayLocal))
  {
    if (deviceEffectsState.currentIndex == 255)
    {
      deviceEffectsState.currentIndex = 0;
    }

    prevTime = currentTime;

    if (allModesWorkTypeLocal == 1)
    {
      if (deviceEffectsState.currentIndex > sizeof(mainModes) / sizeof(mainModes[0]))
      {
        deviceEffectsState.currentIndex = 0;
      }

      deviceEffectsState.effectsGroup = mainModes[deviceEffectsState.currentIndex].effectsGroup;
      deviceEffectsState.effectSubmode = mainModes[deviceEffectsState.currentIndex].subMode;

      deviceEffectsState.currentIndex += 1;
    }
    else
    {
      do
      {
        deviceEffectsState.currentIndex = ESP8266TrueRandom.random(0, ALL_MODES_EFFECT_COUNT);
        randomCounter++;
        if (randomCounter > 30)
          break;
      } while (std::find(usedEffects.begin(), usedEffects.end(), deviceEffectsState.currentIndex) != usedEffects.end());

      usedEffects.push_back(deviceEffectsState.currentIndex);
      if (usedEffects.size() > 20) // Размер истории используемых эффектов
      {
        usedEffects.pop_front();
      }

      deviceEffectsState.effectsGroup = mainModes[deviceEffectsState.currentIndex].effectsGroup;
      deviceEffectsState.effectSubmode = mainModes[deviceEffectsState.currentIndex].subMode;
    }

    FastLED.clear();
  }
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
      uint32_t groupTemp = std::stoi(std::string(command.substr(0, separatorPos)));
      uint32_t submodeTemp = std::stoi(std::string(command.substr(separatorPos + 1)));
      if (groupTemp <= 255 && submodeTemp <= 255)
      {
        settings.startingEffectsGroup = static_cast<uint8_t>(groupTemp);
        settings.startingEffectSubmode = static_cast<uint8_t>(submodeTemp);
      }
      else
      {
        isFailed = true;
      }
    }
  }
  else if (command.rfind(commands[2]) == 0)
  {
    command.remove_prefix(commands[2].length());
    uint32_t brightnessTemp = std::stoi(std::string(command));
    if (brightnessTemp <= 255)
    {
      settings.globalBrightness = static_cast<uint8_t>(brightnessTemp);
    }
    else
    {
      isFailed = true;
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
    uint32_t allModeDelayTemp = std::stoi(std::string(command));
    if (allModeDelayTemp <= 3600)
    {
      settings.allModeDelay = static_cast<uint32_t>(allModeDelayTemp) * 1000;
    }
    else
    {
      isFailed = true;
    }
  }
  else if (command.rfind(commands[5]) == 0)
  {
    command.remove_prefix(commands[5].length());
    uint32_t allModesWorkTypeTemp = std::stoi(std::string(command));
    if (allModesWorkTypeTemp <= 1)
    {
      settings.allModesWorkType = static_cast<uint8_t>(allModesWorkTypeTemp);
    }
    else
    {
      isFailed = true;
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
    uint32_t isWifiAutoOffEnableTemp = std::stoi(std::string(command));
    if (isWifiAutoOffEnableTemp <= 1)
    {
      settings.isWifiAutoOffEnable = static_cast<bool>(isWifiAutoOffEnableTemp);
    }
    else
    {
      isFailed = true;
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
  isCommandReceived = true;

  bool isFailed = false;

  size_t modeStart = command.find("mode=");
  size_t modeEnd = command.find('&', modeStart);
  std::string_view modeString = command.substr(modeStart + 5, modeEnd - (modeStart + 5));

  size_t colorStart = command.find("color=");
  size_t colorEnd = command.find('&', colorStart);
  std::string_view colorString = command.substr(colorStart + 6, colorEnd - (colorStart + 6));

  uint32_t colorTemp = std::stoi(std::string(colorString));
  uint8_t color = 0;

  if (colorTemp <= 20)
  {
    color = static_cast<uint8_t>(colorTemp);
  }
  else
  {
    isFailed = true;
  }

  size_t ledStart = command.find("led=");
  std::string_view ledString = command.substr(ledStart + 4);

  uint32_t ledNumTemp = std::stoi(std::string(ledString));
  uint16_t ledNum = 0;

  if (ledNumTemp <= 257)
  {
    ledNum = static_cast<uint16_t>(ledNumTemp);
  }
  else
  {
    isFailed = true;
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
  else if (command.rfind("allModes") == 0)
  {
    deviceEffectsState.currentIndex = 255;
    deviceEffectsState.isAllModesEnable = true;
    deviceEffectsState.effectsGroup = 255;
    deviceEffectsState.effectSubmode = 255;
    effectAllModes();
  }
  else if (command.find("-") != std::string_view::npos)
  {
    size_t separatorPos = command.find('-');
    if (separatorPos != std::string_view::npos)
    {
      uint32_t groupTemp = std::stoi(std::string(command.substr(0, separatorPos)));
      uint32_t submodeTemp = std::stoi(std::string(command.substr(separatorPos + 1)));
      if (groupTemp <= 255 && submodeTemp <= 255)
      {
        deviceEffectsState.effectsGroup = static_cast<uint8_t>(groupTemp);
        deviceEffectsState.effectSubmode = static_cast<uint8_t>(submodeTemp);
        FastLED.clear();
        if (deviceEffectsState.effectsGroup == 10)
        {
          drawAnimations(255);
        }
      }
      else
      {
        isFailed = true;
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