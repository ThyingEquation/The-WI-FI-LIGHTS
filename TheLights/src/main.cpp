#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <LittleFS.h>
#include <main.h>

#include "animations.h"
#include "canvas.h"
#include "colorfulSpots.h"
#include "colorfulWaves.h"
#include "colors.h"
#include "draw.h"
#include "drawImages.h"
#include "flashLights.h"
#include "games.h"
#include "jumpingLights.h"
#include "matrixMovie.h"
#include "rainbows.h"
#include "runningLights.h"
#include "runningLine.h"
#include "space.h"
#include "water.h"
#include "weather.h"

static void settingsProcessing(void);
static void mainModesProcessing(void);
static void paintingProcessing(void);
static void drawingImagesProcessing(void);
static bool loadSettings();
static bool saveSettings();
static void applyNewParameters(String paramData, uint8_t param);
static void allModesEffect();

CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    mWidth, mHeight, 4,
    NEO_MATRIX_BOTTOM + TEXT_POS + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);

enum effects {
  RUNNING_LINE,
  COLORFUL_SPOTS,
  RAINBOWS,
  DRAW,
  RUNNING_LIGHTS,
  SPACE,
  FLASH_LIGHTS,
  WATER,
  WEATHER,
  COLORFUL_WAVES,
  ANIMATIONS,
  GAMES,
  JUMPING_LIGHTS,
  MATRIX_MOVIE
};

void (*funcArray[14])(uint8_t) = {
    runningLine, colorfulSpots, rainbows,      draw,       runningLights,
    space,       flashLights,   water,         weather,    colorfulWaves,
    animations,  games,         jumpingLights, matrixMovie};

const std::array<std::pair<uint8_t, uint8_t>, 60> modes = {{
    {DRAW, 3},              // 0 полная заливка
    {DRAW, 1},              // 1 заливка змейкой 1
    {DRAW, 2},              // 2 заливка змейкой 2
    {DRAW, 4},              // 3 заливка змейкой 3
    {DRAW, 5},              // 4 цветное дыхание
    {COLORFUL_SPOTS, 255},  // 5 цветные пятна
    {RAINBOWS, 1},          // 6 радуга колесо
    {RAINBOWS, 2},          // 7 радуга волной 1
    {RAINBOWS, 3},          // 8 радуга волной 2
    {RAINBOWS, 4},          // 9 радуга змейкой
    {RUNNING_LIGHTS, 1},    // 10 бегущий огонек медленно
    {RUNNING_LIGHTS, 2},    // 11 бегущий огонек быстро
    {RUNNING_LIGHTS, 3},    // 12 бегущий огонек цветной
    {RUNNING_LIGHTS, 4},    // 13 бегущие огоньки 1
    {RUNNING_LIGHTS, 5},    // 14 бегущие огоньки 2
    {RUNNING_LIGHTS, 7},    // 15 бегущие огоньки 3
    {RUNNING_LIGHTS, 6},    // 16 цветная змейка
    {JUMPING_LIGHTS, 1},    // 17 прыгающие огоньки 1
    {JUMPING_LIGHTS, 2},    // 18 прыгающие огоньки 2
    {JUMPING_LIGHTS, 3},    // 19 прыгающие огоньки 3
    {JUMPING_LIGHTS, 4},    // 20 прыгающие огоньки 4
    {JUMPING_LIGHTS, 5},    // 21 Прыгающий квадрат
    {JUMPING_LIGHTS, 6},    // 22 Прыгающие точки
    {SPACE, 2},             // 23 космические корабли
    {SPACE, 1},             // 24 звездное небо
    {SPACE, 3},             // 25 пульсар
    {SPACE, 4},             // 26 метеорный поток
    {SPACE, 5},             // 27 спиральная галактика
    {FLASH_LIGHTS, 1},      // 28 мерцающие огни 1
    {FLASH_LIGHTS, 2},      // 29 мерцающие огни 2
    {FLASH_LIGHTS, 3},      // 30 мерцающие огни 3
    {WATER, 1},             // 31 лагуна
    {WATER, 2},             // 32 бассейн
    {WEATHER, 1},           // 33 снегопад
    {WEATHER, 2},           // 34 метель
    {WEATHER, 3},           // 35 дождь
    {WEATHER, 4},           // 36 ливень
    {COLORFUL_WAVES, 1},    // 37 цветные волны 1
    {COLORFUL_WAVES, 2},    // 38 цветные волны 2
    {COLORFUL_WAVES, 3},    // 39 цветные волны 3
    {COLORFUL_WAVES, 4},    // 40 цветные волны 4
    {COLORFUL_WAVES, 5},    // 41 цветные волны 5
    {ANIMATIONS, 1},        // 42 анимации: Сердце
    {ANIMATIONS, 2},        // 43 смайлик
    {ANIMATIONS, 3},        // 44 прыгающий человечек
    {ANIMATIONS, 4},        // 45 файербол
    {ANIMATIONS, 5},        // 46 взрыв
    {ANIMATIONS, 6},   // 47 "С НОВЫМ ГОДОМ" на японском
    {ANIMATIONS, 7},   // 48 приветствие на корейском
    {ANIMATIONS, 8},   // 49 цифровой сигнал
    {ANIMATIONS, 9},   // 50 синусоида
    {ANIMATIONS, 10},  // 51 цветные синусоиды
    {ANIMATIONS, 11},  // 52 цветные линии 1
    {ANIMATIONS, 12},  // 53 цветные линии 2
    {ANIMATIONS, 13},  // 54 цветные линии 3
    {ANIMATIONS, 14},  // 55 цветные линии 4
    {GAMES, 1},        // 56 игра змейка
    {GAMES, 2},        // 57 игра тетрис
    {GAMES, 3},        // 58 игра арканоид
    {MATRIX_MOVIE, 1}  // 59 эффект из к/ф матрица
}};

static uint8_t mode = 255;
static uint8_t subMode = 255;
static uint8_t allModesEnable = 0;
static unsigned int currentIndex = 255;

static unsigned long prevTime = 0;
static unsigned long startingMillis = 0;

static uint8_t intBrightness;
static char brightness[4] = {'0', '0', '0', '\0'};
static unsigned long allModeDelay = 0;
static char allModeDelayString[5] = {'0', '0', '0', '0', '\0'};
static char allModeType[2] = {'0', '\0'};
static char startMode[3] = {'0', '0', '\0'};
static char wifiAutoOffType[2] = {'0', '\0'};
static char ssid[15];
const char *password = "11111111";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server(80);

void setup() {
  srand(time(NULL));

  if (!LittleFS.begin()) {
    return;
  }

  loadSettings();

  Serial.begin(9600);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);
  FastLED.addLeds<WS2812B, 4, GRB>(leds, NUM_LEDS)
      .setCorrection(TypicalSMD5050)
      .setDither(intBrightness <= 255);
  FastLED.setBrightness(intBrightness);

  strip.begin();
  strip.show();
  strip.setBrightness(intBrightness);
  strip.show();

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(intBrightness);
  matrix.setTextColor(pgm_read_dword(&(mainColors[rand() % 128])));

  delay(10);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/settings", settingsProcessing);
  server.on("/command", mainModesProcessing);
  server.on("/painting", paintingProcessing);
  server.on("/drawingImages", drawingImagesProcessing);
  server.begin();

  String modeS = startMode;
  uint8_t modeNum = modeS.toInt();

  if (modeNum != 99) {
    if (modeNum != 98) {
      mode = modes[modeNum].first;
      subMode = modes[modeNum].second;
    } else {
      currentIndex = 255;
      allModesEnable = 1;
      mode = 255;
      subMode = 255;
    }
  }

  delay(1500);

  startingMillis = millis();
}

void loop() {
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis >= 100) {
    lastMillis = currentMillis;
    server.handleClient();

    if (wifiAutoOffType[0] == '1') {
      if (currentMillis - startingMillis >= 300000) {
        WiFi.softAPdisconnect(true);
      }
    }
  }

  if (allModesEnable == 1) {
    allModesEffect();
  }

  if (mode < 14) {
    funcArray[mode](subMode);
  }
}

void settingsProcessing() {
  strip.clear();
  strip.show();
  if (server.hasArg("ssid")) {
    String newSsid = server.arg("ssid");
    applyNewParameters(newSsid, 1);
  } else if (server.hasArg("startMode")) {
    String newStartMode = server.arg("startMode");
    applyNewParameters(newStartMode, 2);
  } else if (server.hasArg("brightness")) {
    String newBrightness = server.arg("brightness");
    applyNewParameters(newBrightness, 3);
  } else if (server.hasArg("restart")) {
    ESP.restart();
  } else if (server.hasArg("allModesTime")) {
    String allModeDelayTime = server.arg("allModesTime");
    applyNewParameters(allModeDelayTime, 4);
  } else if (server.hasArg("allModes")) {
    String allMode = server.arg("allModes");
    applyNewParameters(allMode, 5);
  } else if (server.hasArg("wifiOff")) {
    WiFi.softAPdisconnect(true);
  } else if (server.hasArg("wifiAutoOff")) {
    String wifiAutoOff = server.arg("wifiAutoOff");
    applyNewParameters(wifiAutoOff, 6);
  }
}

void drawingImagesProcessing(void) { drawImages((server.arg("img")).toInt()); }

void paintingProcessing(void) {
  int ledNum = server.arg("led").toInt();
  uint8_t canvasColor = server.arg("color").toInt();
  canvas(server.arg("mode"), canvasColor, ledNum);
}

void mainModesProcessing() {
  allModesEnable = 0;
  FastLED.clear();
  strip.clear();
  strip.show();

  if (server.hasArg("ok")) {
  } else if (server.hasArg("stop")) {
    mode = 255;
  } else if (server.hasArg("allModes")) {
    currentIndex = 255;
    allModesEnable = 1;
    mode = 255;
    subMode = 255;
    allModesEffect();
  } else if (server.hasArg("runLine")) {
    mode = RUNNING_LINE;
    runningLine(99);
    subMode = (uint8_t)((server.arg("runLine")).toInt());
  } else if (server.hasArg("spots")) {
    mode = COLORFUL_SPOTS;
  } else if (server.hasArg("rainbow")) {
    mode = RAINBOWS;
    subMode = (server.arg("rainbow")).toInt();
  } else if (server.hasArg("draw")) {
    mode = DRAW;
    subMode = (uint8_t)((server.arg("draw")).toInt());
  } else if (server.hasArg("runLights")) {
    mode = RUNNING_LIGHTS;
    subMode = (uint8_t)((server.arg("runLights")).toInt());
  } else if (server.hasArg("space")) {
    mode = SPACE;
    subMode = (uint8_t)((server.arg("space")).toInt());
  } else if (server.hasArg("flashLights")) {
    mode = FLASH_LIGHTS;
    subMode = (uint8_t)((server.arg("flashLights")).toInt());
  } else if (server.hasArg("water")) {
    mode = WATER;
    subMode = (uint8_t)((server.arg("water")).toInt());
  } else if (server.hasArg("weather")) {
    mode = WEATHER;
    subMode = (uint8_t)((server.arg("weather")).toInt());
  } else if (server.hasArg("colorfulWaves")) {
    mode = COLORFUL_WAVES;
    subMode = (uint8_t)((server.arg("colorfulWaves")).toInt());
  } else if (server.hasArg("animations")) {
    mode = ANIMATIONS;
    animations(99);
    subMode = (uint8_t)((server.arg("animations")).toInt());
  } else if (server.hasArg("games")) {
    mode = GAMES;
    subMode = (uint8_t)((server.arg("games")).toInt());
  } else if (server.hasArg("jumpingLights")) {
    mode = JUMPING_LIGHTS;
    subMode = (uint8_t)((server.arg("jumpingLights")).toInt());
  } else if (server.hasArg("matrix")) {
    mode = MATRIX_MOVIE;
    subMode = (uint8_t)((server.arg("matrix")).toInt());
  }
}

bool loadSettings() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    return false;
  }

  size_t size = configFile.size();
  if (size > 256) {
    configFile.close();
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  size_t bytesRead = configFile.readBytes(buf.get(), size);
  configFile.close();

  if (bytesRead != size) {
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    return false;
  }

  const char *jsonStartmode = doc["json_startmode"];
  if (jsonStartmode) {
    strncpy(startMode, jsonStartmode, 2);
  }

  const char *jsonSsid = doc["json_ssid"];
  if (jsonSsid) {
    strncpy(ssid, jsonSsid, 15);
  }

  const char *jsonBrightness = doc["json_brightness"];
  if (jsonBrightness) {
    strncpy(brightness, jsonBrightness, 3);
    String localStr = brightness;
    intBrightness = (uint8_t)(localStr.toInt());
  }

  const char *jsonAllModeDelay = doc["json_allModeDelay"];
  if (jsonAllModeDelay) {
    strncpy(allModeDelayString, jsonAllModeDelay, 3);
    String localStr = allModeDelayString;
    allModeDelay = (unsigned long)(localStr.toInt()) * 1000;
  }

  const char *jsonAllModeType = doc["json_allMode"];
  if (jsonSsid) {
    strncpy(allModeType, jsonAllModeType, 1);
  }

  const char *jsonWifiAutoOff = doc["json_wifiAutoOff"];
  if (jsonSsid) {
    strncpy(wifiAutoOffType, jsonWifiAutoOff, 1);
  }

  return true;
}

bool saveSettings() {
  JsonDocument json;

  json["json_startmode"] = startMode;
  json["json_ssid"] = ssid;
  json["json_brightness"] = brightness;
  json["json_allModeDelay"] = allModeDelayString;
  json["json_allMode"] = allModeType;
  json["json_wifiAutoOff"] = wifiAutoOffType;

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    return false;
  }

  serializeJson(json, configFile);
  configFile.close();

  return true;
}

void successSave() {
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

void applyNewParameters(String paramData, uint8_t param) {
  // имя сети - 1; стартовый режим - 2; яркость - 3
  // длительность эффекта для "Все режимы" - 4
  // "Все режимы" - эффекты подрял или в разброс - 5
  // Автоотключение wifi через 5 минут после включения

  int paramDataLen = paramData.length() + 1;
  char webParamData[paramDataLen];
  paramData.toCharArray(webParamData, paramDataLen);

  switch (param) {
    case 1:
      strncpy(ssid, webParamData, sizeof(ssid));
      break;

    case 2:
      strncpy(startMode, webParamData, sizeof(startMode));
      break;

    case 3:
      strncpy(brightness, webParamData, sizeof(brightness));
      break;

    case 4: {
      strncpy(allModeDelayString, webParamData, sizeof(allModeDelayString));
      String localStr = allModeDelayString;
      allModeDelay = (unsigned long)(localStr.toInt());
    } break;

    case 5:
      strncpy(allModeType, webParamData, sizeof(allModeType));
      break;

    case 6:
      strncpy(wifiAutoOffType, webParamData, sizeof(wifiAutoOffType));
      break;
  }

  saveSettings();

  if (param == 4) {
    allModeDelay *= 1000;
  }

  successSave();
}

void allModesEffect() {
  unsigned long currentTime = millis();

  if (currentIndex == 255 || (currentTime - prevTime >= allModeDelay)) {
    if (currentIndex == 255) {
      currentIndex = 0;
    }

    prevTime = currentTime;

    if (allModeType[0] == '1') {
      if (currentIndex > sizeof(modes) / sizeof(modes[0])) {
        currentIndex = 0;
      }

      mode = modes[currentIndex].first;
      subMode = modes[currentIndex].second;

      currentIndex += 1;

    } else {
      currentIndex = rand() % 60;

      mode = modes[currentIndex].first;
      subMode = modes[currentIndex].second;
    }

    FastLED.clear();
    FastLED.show();
    FastLED.show();
    strip.clear();
    strip.show();
    strip.show();
  }
}

int XY(int x, int y) {
  if (x % 2 == 0) {
    return x * mHeight + y;
  } else {
    return x * mHeight + (mHeight - 1 - y);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}