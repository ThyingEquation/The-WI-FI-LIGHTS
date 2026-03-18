#include <deque>

#include "lightsServer.h"
#include "lightsSettings.h"
#include "effects.h"

static void handleSettings();
static void handleCanvas();
static void handleMainCommand();

ESP8266WebServer server(80);

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
    {CANVAS_EFFECTS, 3, 1},   // [0.0]   - "Полная заливка"
    {CANVAS_EFFECTS, 1, 1},   // [0.1]   - "Быстрая заливка змейкой"
    {CANVAS_EFFECTS, 2, 1},   // [0.2]   - "Медленная заливка змейкой"
    {CANVAS_EFFECTS, 4, 1},   // [0.3]   - "Цветная заливка змейкой"
    {CANVAS_EFFECTS, 5, 1},   // [0.4]   - "Цветное дыхание"
    {CANVAS_EFFECTS, 255, 1}, // [0.5]   - "Все картинки подряд"

    {COLORFUL_SPOTS, 255, 1}, // [2.0]   - "Цветные пятна"

    {RAINBOWS, 1, 1}, // [3.0]   - "Радуга колесо"
    {RAINBOWS, 2, 1}, // [3.1]   - "Радуга волной 1"
    {RAINBOWS, 3, 1}, // [3.2]   - "Радуга волной 2"
    {RAINBOWS, 4, 1}, // [3.3]   - "Радуга змейкой"

    {RUNNING_LIGHTS, 1, 1}, // [4.0]   - "Бегущий огонек медленно"
    {RUNNING_LIGHTS, 2, 1}, // [4.1]   - "Бегущий огонек быстро"
    {RUNNING_LIGHTS, 3, 1}, // [4.2]   - "Бегущий огонек цветной"
    {RUNNING_LIGHTS, 4, 1}, // [4.3]   - "Бегущие огоньки 1"
    {RUNNING_LIGHTS, 5, 1}, // [4.4]   - "Бегущие огоньки 2"
    {RUNNING_LIGHTS, 7, 1}, // [4.5]   - "Бегущие огоньки 3"
    {RUNNING_LIGHTS, 6, 1}, // [4.6]   - "Цветная змейка"

    {JUMPING_LIGHTS, 1, 1}, // [5.0]  - "Прыгающие огоньки 1"
    {JUMPING_LIGHTS, 2, 1}, // [5.1]  - "Прыгающие огоньки 2"
    {JUMPING_LIGHTS, 3, 1}, // [5.2]  - "Прыгающие огоньки 3"
    {JUMPING_LIGHTS, 4, 1}, // [5.3]  - "Прыгающие огоньки 4"
    {JUMPING_LIGHTS, 5, 1}, // [5.4]  - "Прыгающий квадрат"
    {JUMPING_LIGHTS, 6, 1}, // [5.5]  - "Прыгающие точки"

    {SPACE_EFFECTS, 2, 1}, // [6.0]   - "Космические корабли"
    {SPACE_EFFECTS, 1, 1}, // [6.1]   - "Звездное небо"
    {SPACE_EFFECTS, 3, 1}, // [6.2]   - "Пульсирующая звезда"
    {SPACE_EFFECTS, 4, 1}, // [6.3]   - "Метеорный поток"
    {SPACE_EFFECTS, 5, 1}, // [6.4]   - "Спиральная туманность"

    {FLASH_LIGHTS, 1, 1}, // [7.0]   - "Мерцающие огни 1"
    {FLASH_LIGHTS, 2, 1}, // [7.1]   - "Мерцающие огни 2"
    {FLASH_LIGHTS, 3, 1}, // [7.2]   - "Мерцающие огни 3"

    {WATER_EFFECTS, 1, 1}, // [8.0]   - "Лагуна"
    {WATER_EFFECTS, 2, 1}, // [8.1]   - "Бассейн"

    {WEATHER_EFFECTS, 1, 1}, // [9.0]   - "Снегопад"
    {WEATHER_EFFECTS, 2, 1}, // [9.1]   - "Метель"
    {WEATHER_EFFECTS, 3, 1}, // [9.2]   - "Дождь"
    {WEATHER_EFFECTS, 4, 1}, // [9.3]   - "Ливень"

    {COLORFUL_WAVES, 1, 1}, // [10.0]   - "Цветные волны 1"
    {COLORFUL_WAVES, 2, 1}, // [10.1]   - "Цветные волны 2"
    {COLORFUL_WAVES, 3, 1}, // [10.2]   - "Цветные волны 3"
    {COLORFUL_WAVES, 4, 1}, // [10.3]   - "Цветные волны 4"
    {COLORFUL_WAVES, 5, 1}, // [10.4]   - "Цветные волны 5"

    {ANIMATIONS, 1, 1},  // [11.0]  - "Анимации: Сердце"
    {ANIMATIONS, 2, 1},  // [11.1]  - "Смайлик"
    {ANIMATIONS, 3, 1},  // [11.2]  - "Прыгающий человечек"
    {ANIMATIONS, 4, 1},  // [11.3]  - "Файербол"
    {ANIMATIONS, 5, 1},  // [11.4]  - "Взрыв"
    {ANIMATIONS, 6, 1},  // [11.5]  - "'С НОВЫМ ГОДОМ' на японском"
    {ANIMATIONS, 7, 1},  // [11.6]  - "Приветствие на корейском"
    {ANIMATIONS, 8, 1},  // [11.7]  - "Цифровой сигнал"
    {ANIMATIONS, 9, 1},  // [11.8]  - "Синусоида"
    {ANIMATIONS, 10, 1}, // [11.9]  - "Цветные синусоиды"
    {ANIMATIONS, 11, 1}, // [11.10] - "Цветные линии 1"
    {ANIMATIONS, 12, 1}, // [11.11] - "Цветные линии 2"
    {ANIMATIONS, 13, 1}, // [11.12] - "Цветные линии 3"
    {ANIMATIONS, 14, 1}, // [11.13] - "Цветные линии 4"

    {GAMES_EFFECTS, 1, 1}, // [11.0]  - "Игра змейка"
    {GAMES_EFFECTS, 2, 1}, // [11.1]  - "Игра тетрис"
    {GAMES_EFFECTS, 3, 1}, // [11.2]  - "Игра арканоид"

    {MATRIX_MOVIE_EFFECT, 1, 1} // [13.0]  - "Эффект из к/ф матрица"
}};

static bool isCommandReceived = false;

void initLightServer()
{
  IPAddress localIp(settings.localIpVal[0], settings.localIpVal[1], settings.localIpVal[2], settings.localIpVal[3]);
  IPAddress gateway(settings.gatewayVal[0], settings.gatewayVal[1], settings.gatewayVal[2], settings.gatewayVal[3]);
  IPAddress subnet(settings.subnetVal[0], settings.subnetVal[1], settings.subnetVal[2], settings.subnetVal[3]);

  WiFi.softAP(settings.ssid, settings.password);
  WiFi.softAPConfig(localIp, gateway, subnet);

  delay(100);

  server.on("/settings", handleSettings);
  server.on("/command", handleMainCommand);
  server.on("/painting", handleCanvas);
  server.begin();
}

void checkLightServer()
{
  server.handleClient();
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
  uint32_t currentTime = millis();

  if (deviceEffectsState.currentIndex == 255 || (currentTime - prevTime >= settings.allModeDelay))
  {
    if (deviceEffectsState.currentIndex == 255)
    {
      deviceEffectsState.currentIndex = 0;
    }

    prevTime = currentTime;

    if (settings.allModesWorkType == 1)
    {
      if (deviceEffectsState.currentIndex > sizeof(mainModes) / sizeof(mainModes[0]))
      {
        deviceEffectsState.currentIndex = 0;
      }

      deviceEffectsState.effectsGroup = mainModes[deviceEffectsState.currentIndex].effectsGroup;
      deviceEffectsState.submode = mainModes[deviceEffectsState.currentIndex].subMode;

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
      deviceEffectsState.submode = mainModes[deviceEffectsState.currentIndex].subMode;
    }

    FastLED.clear(true);
  }
}

static void handleSettings()
{
  FastLED.clear(true);

  if (server.hasArg("ssid"))
  {
    String newSsid = server.arg("ssid");
    if (applyNewParameters(newSsid, 1))
    {
      return;
    }
  }
  else if (server.hasArg("startMode"))
  {
    String newStartMode = server.arg("startMode");
    if (applyNewParameters(newStartMode, 2))
    {
      return;
    }
  }
  else if (server.hasArg("brightness"))
  {
    String newBrightness = server.arg("brightness");
    if (applyNewParameters(newBrightness, 3))
    {
      return;
    }
  }
  else if (server.hasArg("restart"))
  {
    ESP.restart();
  }
  else if (server.hasArg("allModesTime"))
  {
    String allModeDelayTime = server.arg("allModesTime");
    if (applyNewParameters(allModeDelayTime, 4))
    {
      return;
    }
  }
  else if (server.hasArg("allModes"))
  {
    String allMode = server.arg("allModes");
    if (applyNewParameters(allMode, 5))
    {
      return;
    }
  }
  else if (server.hasArg("wifiOff"))
  {
    WiFi.softAPdisconnect(true);
  }
  else if (server.hasArg("wifiAutoOff"))
  {
    String wifiAutoOff = server.arg("wifiAutoOff");
    if (applyNewParameters(wifiAutoOff, 6))
    {
      return;
    }
  }
}

static void handleCanvas()
{
  isCommandReceived = true;

  uint16_t ledNum = (uint16_t)(server.arg("led").toInt());
  uint8_t canvasColor = (uint8_t)(server.arg("color").toInt());
  drawOnCanvas(server.arg("mode"), canvasColor, ledNum);
}

static void handleMainCommand()
{
  isCommandReceived = true;
  deviceEffectsState.isScreenClearEnable = true;
  deviceEffectsState.isAllModesEnable = false;

  if (server.hasArg("ok"))
  {
  }
  else if (server.hasArg("stop"))
  {
    deviceEffectsState.effectsGroup = 255;
  }
  else if (server.hasArg("allModes"))
  {
    deviceEffectsState.currentIndex = 255;
    deviceEffectsState.isAllModesEnable = true;
    deviceEffectsState.effectsGroup = 255;
    deviceEffectsState.submode = 255;
    effectAllModes();
  }
  else if (server.hasArg("runLine"))
  {
    deviceEffectsState.effectsGroup = RUNNING_LINE;
    drawRunningLine(99);
    deviceEffectsState.submode = (uint8_t)((server.arg("runLine")).toInt());
  }
  else if (server.hasArg("spots"))
  {
    deviceEffectsState.effectsGroup = COLORFUL_SPOTS;
  }
  else if (server.hasArg("rainbow"))
  {
    deviceEffectsState.effectsGroup = RAINBOWS;
    deviceEffectsState.submode = (uint8_t)((server.arg("rainbow")).toInt());
  }
  else if (server.hasArg("draw"))
  {
    deviceEffectsState.effectsGroup = CANVAS_EFFECTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("draw")).toInt());
  }
  else if (server.hasArg("runLights"))
  {
    deviceEffectsState.effectsGroup = RUNNING_LIGHTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("runLights")).toInt());
  }
  else if (server.hasArg("space"))
  {
    deviceEffectsState.effectsGroup = SPACE_EFFECTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("space")).toInt());
  }
  else if (server.hasArg("flashLights"))
  {
    deviceEffectsState.effectsGroup = FLASH_LIGHTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("flashLights")).toInt());
  }
  else if (server.hasArg("water"))
  {
    deviceEffectsState.effectsGroup = WATER_EFFECTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("water")).toInt());
  }
  else if (server.hasArg("weather"))
  {
    deviceEffectsState.effectsGroup = WEATHER_EFFECTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("weather")).toInt());
  }
  else if (server.hasArg("colorfulWaves"))
  {
    deviceEffectsState.effectsGroup = COLORFUL_WAVES;
    deviceEffectsState.submode = (uint8_t)((server.arg("colorfulWaves")).toInt());
  }
  else if (server.hasArg("animations"))
  {
    deviceEffectsState.effectsGroup = ANIMATIONS;
    drawAnimations(99);
    deviceEffectsState.submode = (uint8_t)((server.arg("animations")).toInt());
  }
  else if (server.hasArg("games"))
  {
    deviceEffectsState.effectsGroup = GAMES_EFFECTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("games")).toInt());
  }
  else if (server.hasArg("jumpingLights"))
  {
    deviceEffectsState.effectsGroup = JUMPING_LIGHTS;
    deviceEffectsState.submode = (uint8_t)((server.arg("jumpingLights")).toInt());
  }
  else if (server.hasArg("matrix"))
  {
    deviceEffectsState.effectsGroup = MATRIX_MOVIE_EFFECT;
    deviceEffectsState.submode = (uint8_t)((server.arg("matrix")).toInt());
  }
}