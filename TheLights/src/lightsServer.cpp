#include <deque>

#include "lightsServer.h"
#include "lightsSettings.h"
#include "effects.h"

static void handleSettings();
static void handleConnectionState();
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

  delay(500);

  server.on("/settings", handleSettings);
  server.on("/connectionState", handleConnectionState);
  server.on("/command", handleMainCommand);
  server.on("/canvas", handleCanvas);

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

    FastLED.clear(true);
  }
}

static void handleSettings()
{
  FastLED.clear(true);

  if (server.hasArg("ssid"))
  {
    server.arg("ssid").toCharArray(settings.ssid, sizeof(settings.ssid));
    saveSettings();
  }
  else if (server.hasArg("startMode") && server.hasArg("startSubMode")) // ?startMode=5&startSubMode=2
  {
    settings.startingEffectsGroup = server.arg("startMode").toInt();
    settings.startingEffectSubMode = server.arg("startSubMode").toInt();
    saveSettings();
  }
  else if (server.hasArg("brightness"))
  {
    settings.globalBrightness = server.arg("brightness").toInt();
    saveSettings();
  }
  else if (server.hasArg("restart"))
  {
    ESP.restart();
  }
  else if (server.hasArg("allModesTime"))
  {
    settings.allModeDelay = server.arg("allModesTime").toInt();
    saveSettings();
  }
  else if (server.hasArg("allModes"))
  {
    settings.allModesWorkType = server.arg("allModes").toInt();
    saveSettings();
  }
  else if (server.hasArg("wifiOff"))
  {
    WiFi.softAPdisconnect(true);
  }
  else if (server.hasArg("wifiAutoOff"))
  {
    settings.allModesWorkType = server.arg("wifiAutoOff").toInt();
    saveSettings();
  }
}

static void handleConnectionState()
{
  if (server.hasArg("check"))
  {
    server.send(200, "text/plain", "imHere");
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

  if (server.hasArg("stop"))
  {
    deviceEffectsState.effectsGroup = 255;
  }
  else if (server.hasArg("canvasEffects"))
  {
    deviceEffectsState.effectsGroup = CANVAS_EFFECTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("canvasEffects")).toInt());
  }
  else if (server.hasArg("runningLine"))
  {
    deviceEffectsState.effectsGroup = RUNNING_LINE;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("runningLine")).toInt());
  }
  else if (server.hasArg("allModes"))
  {
    deviceEffectsState.currentIndex = 255;
    deviceEffectsState.isAllModesEnable = true;
    deviceEffectsState.effectsGroup = 255;
    deviceEffectsState.effectSubmode = 255;
    effectAllModes();
  }
  else if (server.hasArg("colorfulSpots"))
  {
    deviceEffectsState.effectsGroup = COLORFUL_SPOTS;
  }
  else if (server.hasArg("rainbows"))
  {
    deviceEffectsState.effectsGroup = RAINBOWS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("rainbows")).toInt());
  }
  else if (server.hasArg("runningLights"))
  {
    deviceEffectsState.effectsGroup = RUNNING_LIGHTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("runningLights")).toInt());
  }
  else if (server.hasArg("jumpingLights"))
  {
    deviceEffectsState.effectsGroup = JUMPING_LIGHTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("jumpingLights")).toInt());
  }
  else if (server.hasArg("spaceEffects"))
  {
    deviceEffectsState.effectsGroup = SPACE_EFFECTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("spaceEffects")).toInt());
  }
  else if (server.hasArg("flashLights"))
  {
    deviceEffectsState.effectsGroup = FLASH_LIGHTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("flashLights")).toInt());
  }
  else if (server.hasArg("waterEffects"))
  {
    deviceEffectsState.effectsGroup = WATER_EFFECTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("waterEffects")).toInt());
  }
  else if (server.hasArg("weatherEffects"))
  {
    deviceEffectsState.effectsGroup = WEATHER_EFFECTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("weatherEffects")).toInt());
  }
  else if (server.hasArg("colorfulWaves"))
  {
    deviceEffectsState.effectsGroup = COLORFUL_WAVES;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("colorfulWaves")).toInt());
  }
  else if (server.hasArg("animations"))
  {
    deviceEffectsState.effectsGroup = ANIMATIONS;
    drawAnimations(99);
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("animations")).toInt());
  }
  else if (server.hasArg("gamesEffects"))
  {
    deviceEffectsState.effectsGroup = GAMES_EFFECTS;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("gamesEffects")).toInt());
  }
  else if (server.hasArg("matrixMovieEffect"))
  {
    deviceEffectsState.effectsGroup = MATRIX_MOVIE_EFFECT;
    deviceEffectsState.effectSubmode = (uint8_t)((server.arg("matrixMovieEffect")).toInt());
  }
}