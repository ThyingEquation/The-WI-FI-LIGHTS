#include <deque>

#include "lightsServer.h"
#include "lightsSettings.h"
#include "modes.h"

ESP8266WebServer server(80);

enum EffectsGroupMode // Главные группы эффектов
{
  RUNNING_LINE,   // (0)  - Бегущая строка (текст)
  COLORFUL_SPOTS, // (1)  - Цветные пятна
  RAINBOWS,       // (2)  - Радуги
  DRAW,           // (3)  - Эффекты режима рисования
  RUNNING_LIGHTS, // (4)  - Бегущие огни
  SPACE,          // (5)  - Космос
  FLASH_LIGHTS,   // (6)  - Мерцающие огни (вспышки)
  WATER,          // (7)  - Водные эффекты
  WEATHER,        // (8)  - Погодные эффекты
  COLORFUL_WAVES, // (9)  - Цветные волны
  ANIMATIONS,     // (10) - Анимации
  GAMES,          // (11) - Эффекты в виде игр
  JUMPING_LIGHTS, // (12) - Прыгающие огоньки
  MATRIX_MOVIE    // (13) - Эффект из к/ф матрица
};

const std::array<ModeConfig, ALL_MODES_EFFECT_COUNT> mainModes = {{
    {DRAW, 3, 1},             // [3.0]   - "Полная заливка"
    {DRAW, 1, 1},             // [3.1]   - "Быстрая заливка змейкой"
    {DRAW, 2, 1},             // [3.2]   - "Медленная заливка змейкой"
    {DRAW, 4, 1},             // [3.3]   - "Цветная заливка змейкой"
    {DRAW, 5, 1},             // [3.4]   - "Цветное дыхание"
    {DRAW, 255, 1},            // [3.5]   - "Все картинки подряд"

    {COLORFUL_SPOTS, 255, 1}, // [1.0]   - "Цветные пятна"

    {RAINBOWS, 1, 1},         // [2.0]   - "Радуга колесо"
    {RAINBOWS, 2, 1},         // [2.1]   - "Радуга волной 1"
    {RAINBOWS, 3, 1},         // [2.2]   - "Радуга волной 2"
    {RAINBOWS, 4, 1},         // [2.3]   - "Радуга змейкой"

    {RUNNING_LIGHTS, 1, 1},   // [4.0]   - "Бегущий огонек медленно"
    {RUNNING_LIGHTS, 2, 1},   // [4.1]   - "Бегущий огонек быстро"
    {RUNNING_LIGHTS, 3, 1},   // [4.2]   - "Бегущий огонек цветной"
    {RUNNING_LIGHTS, 4, 1},   // [4.3]   - "Бегущие огоньки 1"
    {RUNNING_LIGHTS, 5, 1},   // [4.4]   - "Бегущие огоньки 2"
    {RUNNING_LIGHTS, 7, 1},   // [4.5]   - "Бегущие огоньки 3"
    {RUNNING_LIGHTS, 6, 1},   // [4.6]   - "Цветная змейка"

    {JUMPING_LIGHTS, 1, 1},   // [12.0]  - "Прыгающие огоньки 1"
    {JUMPING_LIGHTS, 2, 1},   // [12.1]  - "Прыгающие огоньки 2"
    {JUMPING_LIGHTS, 3, 1},   // [12.2]  - "Прыгающие огоньки 3"
    {JUMPING_LIGHTS, 4, 1},   // [12.3]  - "Прыгающие огоньки 4"
    {JUMPING_LIGHTS, 5, 1},   // [12.4]  - "Прыгающий квадрат"
    {JUMPING_LIGHTS, 6, 1},   // [12.5]  - "Прыгающие точки"

    {SPACE, 2, 1},            // [5.0]   - "Космические корабли"
    {SPACE, 1, 1},            // [5.1]   - "Звездное небо"
    {SPACE, 3, 1},            // [5.2]   - "Пульсирующая звезда"
    {SPACE, 4, 1},            // [5.3]   - "Метеорный поток"
    {SPACE, 5, 1},            // [5.4]   - "Спиральная туманность"

    {FLASH_LIGHTS, 1, 1},     // [6.0]   - "Мерцающие огни 1"
    {FLASH_LIGHTS, 2, 1},     // [6.1]   - "Мерцающие огни 2"
    {FLASH_LIGHTS, 3, 1},     // [6.2]   - "Мерцающие огни 3"

    {WATER, 1, 1},            // [7.0]   - "Лагуна"
    {WATER, 2, 1},            // [7.1]   - "Бассейн"

    {WEATHER, 1, 1},          // [8.0]   - "Снегопад"
    {WEATHER, 2, 1},          // [8.1]   - "Метель"
    {WEATHER, 3, 1},          // [8.2]   - "Дождь"
    {WEATHER, 4, 1},          // [8.3]   - "Ливень"

    {COLORFUL_WAVES, 1, 1},   // [9.0]   - "Цветные волны 1"
    {COLORFUL_WAVES, 2, 1},   // [9.1]   - "Цветные волны 2"
    {COLORFUL_WAVES, 3, 1},   // [9.2]   - "Цветные волны 3"
    {COLORFUL_WAVES, 4, 1},   // [9.3]   - "Цветные волны 4"
    {COLORFUL_WAVES, 5, 1},   // [9.4]   - "Цветные волны 5"

    {ANIMATIONS, 1, 1},       // [10.0]  - "Анимации: Сердце"
    {ANIMATIONS, 2, 1},       // [10.1]  - "Смайлик"
    {ANIMATIONS, 3, 1},       // [10.2]  - "Прыгающий человечек"
    {ANIMATIONS, 4, 1},       // [10.3]  - "Файербол"
    {ANIMATIONS, 5, 1},       // [10.4]  - "Взрыв"
    {ANIMATIONS, 6, 1},       // [10.5]  - "'С НОВЫМ ГОДОМ' на японском"
    {ANIMATIONS, 7, 1},       // [10.6]  - "Приветствие на корейском"
    {ANIMATIONS, 8, 1},       // [10.7]  - "Цифровой сигнал"
    {ANIMATIONS, 9, 1},       // [10.8]  - "Синусоида"
    {ANIMATIONS, 10, 1},      // [10.9]  - "Цветные синусоиды"
    {ANIMATIONS, 11, 1},      // [10.10] - "Цветные линии 1"
    {ANIMATIONS, 12, 1},      // [10.11] - "Цветные линии 2"
    {ANIMATIONS, 13, 1},      // [10.12] - "Цветные линии 3"
    {ANIMATIONS, 14, 1},      // [10.13] - "Цветные линии 4"

    {GAMES, 1, 1},            // [11.0]  - "Игра змейка"
    {GAMES, 2, 1},            // [11.1]  - "Игра тетрис"
    {GAMES, 3, 1},            // [11.2]  - "Игра арканоид"

    {MATRIX_MOVIE, 1, 1}      // [13.0]  - "Эффект из к/ф матрица"
}};

uint8_t allModesEnable = 0;
uint8_t effectsGroup = 255;
uint8_t submode = 255;
uint8_t currentIndex = 255;

void handleSettings()
{
  strip.clear();
  strip.show();

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

void handlePainting()
{
  uint16_t ledNum = (uint16_t)(server.arg("led").toInt());
  uint8_t canvasColor = (uint8_t)(server.arg("color").toInt());
  drawOnCanvas(server.arg("mode"), canvasColor, ledNum);
}

void handleMainCommand()
{
  allModesEnable = 0;
  FastLED.clear(true);
  //FastLED.show();
  //strip.clear();
  //strip.show();

  if (server.hasArg("ok"))
  {
  }
  else if (server.hasArg("stop"))
  {
    effectsGroup = 255;
  }
  else if (server.hasArg("allModes"))
  {
    currentIndex = 255;
    allModesEnable = 1;
    effectsGroup = 255;
    submode = 255;
    effectAllModes();
  }
  else if (server.hasArg("runLine"))
  {
    effectsGroup = RUNNING_LINE;
    drawRunningLines(99);
    submode = (uint8_t)((server.arg("runLine")).toInt());
  }
  else if (server.hasArg("spots"))
  {
    effectsGroup = COLORFUL_SPOTS;
  }
  else if (server.hasArg("rainbow"))
  {
    effectsGroup = RAINBOWS;
    submode = (uint8_t)((server.arg("rainbow")).toInt());
  }
  else if (server.hasArg("draw"))
  {
    effectsGroup = DRAW;
    submode = (uint8_t)((server.arg("draw")).toInt());
  }
  else if (server.hasArg("runLights"))
  {
    effectsGroup = RUNNING_LIGHTS;
    submode = (uint8_t)((server.arg("runLights")).toInt());
  }
  else if (server.hasArg("space"))
  {
    effectsGroup = SPACE;
    submode = (uint8_t)((server.arg("space")).toInt());
  }
  else if (server.hasArg("flashLights"))
  {
    effectsGroup = FLASH_LIGHTS;
    submode = (uint8_t)((server.arg("flashLights")).toInt());
  }
  else if (server.hasArg("water"))
  {
    effectsGroup = WATER;
    submode = (uint8_t)((server.arg("water")).toInt());
  }
  else if (server.hasArg("weather"))
  {
    effectsGroup = WEATHER;
    submode = (uint8_t)((server.arg("weather")).toInt());
  }
  else if (server.hasArg("colorfulWaves"))
  {
    effectsGroup = COLORFUL_WAVES;
    submode = (uint8_t)((server.arg("colorfulWaves")).toInt());
  }
  else if (server.hasArg("animations"))
  {
    effectsGroup = ANIMATIONS;
    drawAnimations(99);
    submode = (uint8_t)((server.arg("animations")).toInt());
  }
  else if (server.hasArg("games"))
  {
    effectsGroup = GAMES;
    submode = (uint8_t)((server.arg("games")).toInt());
  }
  else if (server.hasArg("jumpingLights"))
  {
    effectsGroup = JUMPING_LIGHTS;
    submode = (uint8_t)((server.arg("jumpingLights")).toInt());
  }
  else if (server.hasArg("matrix"))
  {
    effectsGroup = MATRIX_MOVIE;
    submode = (uint8_t)((server.arg("matrix")).toInt());
  }
}

void effectAllModes()
{
  std::deque<uint8_t> usedEffects;

  static uint32_t prevTime = 0;
  static uint8_t randomCounter = 0;
  uint32_t currentTime = millis();

  if (currentIndex == 255 || (currentTime - prevTime >= settings.allModeDelay))
  {
    if (currentIndex == 255)
    {
      currentIndex = 0;
    }

    prevTime = currentTime;

    if (settings.allModesWorkType == 1)
    {
      if (currentIndex > sizeof(mainModes) / sizeof(mainModes[0]))
      {
        currentIndex = 0;
      }

      effectsGroup = mainModes[currentIndex].effectsGroup;
      submode = mainModes[currentIndex].subMode;

      currentIndex += 1;
    }
    else
    {
      do
      {
        currentIndex = ESP8266TrueRandom.random(0, ALL_MODES_EFFECT_COUNT);
        randomCounter++;
        if (randomCounter > 30)
          break;
      } while (std::find(usedEffects.begin(), usedEffects.end(), currentIndex) != usedEffects.end());

      usedEffects.push_back(currentIndex);
      if (usedEffects.size() > 20) // Размер истории используемых эффектов
      {
        usedEffects.pop_front();
      }

      effectsGroup = mainModes[currentIndex].effectsGroup;
      submode = mainModes[currentIndex].subMode;
    }

    FastLED.clear();
    FastLED.show();
    FastLED.show();
    strip.clear();
    strip.show();
    strip.show();
  }
}