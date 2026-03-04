#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include <../inc/main.h>
#include <../inc/settings.h>

#include "../modes/modes.h"
#include "../inc/colors.h"

static void settingsProcessing(void);
static void mainModesProcessing(void);
static void paintingProcessing(void);
static void drawingImagesProcessing(void);
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
static uint8_t currentIndex = 255;

static unsigned long startingMillis = 0;

appSettings_s settings;

ESP8266WebServer server(80);

void setup() {

  if (!LittleFS.begin()) {
    return;
  }

  if (loadSettings()) {return;}

  Serial.begin(9600);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);
  FastLED.addLeds<WS2812B, 4, GRB>(leds, NUM_LEDS)
      .setCorrection(TypicalSMD5050)
      .setDither(settings.intBrightness <= 255);
  FastLED.setBrightness(settings.intBrightness);

  strip.begin();
  strip.show();
  strip.setBrightness(settings.intBrightness);
  strip.show();

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(settings.intBrightness);
  matrix.setTextColor(pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(128)])));

  delay(10);

  IPAddress localIp(settings.localIpVal[0], settings.localIpVal[1], settings.localIpVal[2], settings.localIpVal[3]);
  IPAddress gateway(settings.gatewayVal[0], settings.gatewayVal[1], settings.gatewayVal[2], settings.gatewayVal[3]);
  IPAddress subnet(settings.subnetVal[0], settings.subnetVal[1], settings.subnetVal[2], settings.subnetVal[3]);

  WiFi.softAP(settings.ssid, settings.password);
  WiFi.softAPConfig(localIp, gateway, subnet);
  delay(100);

  server.on("/settings", settingsProcessing);
  server.on("/command", mainModesProcessing);
  server.on("/painting", paintingProcessing);
  server.on("/drawingImages", drawingImagesProcessing);
  server.begin();

  //String modeS = startMode;
  //uint8_t modeNum = modeS.toInt();

  if (settings.modeNum != 99) {
    if (settings.modeNum != 98) {
      mode = modes[settings.modeNum].first;
      subMode = modes[settings.modeNum].second;
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

    if (settings.isWifiAutoOffEnable) {
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
    if (applyNewParameters(newSsid, 1)) {return;}
  } else if (server.hasArg("startMode")) {
    String newStartMode = server.arg("startMode");
    if (applyNewParameters(newStartMode, 2)) {return;}
  } else if (server.hasArg("brightness")) {
    String newBrightness = server.arg("brightness");
    if (applyNewParameters(newBrightness, 3)) {return;}
  } else if (server.hasArg("restart")) {
    ESP.restart();
  } else if (server.hasArg("allModesTime")) {
    String allModeDelayTime = server.arg("allModesTime");
    if (applyNewParameters(allModeDelayTime, 4)) {return;}
  } else if (server.hasArg("allModes")) {
    String allMode = server.arg("allModes");
    if (applyNewParameters(allMode, 5)) {return;}
  } else if (server.hasArg("wifiOff")) {
    WiFi.softAPdisconnect(true);
  } else if (server.hasArg("wifiAutoOff")) {
    String wifiAutoOff = server.arg("wifiAutoOff");
    if (applyNewParameters(wifiAutoOff, 6)) {return;}
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

void allModesEffect() {
  static unsigned long prevTime = 0;
  unsigned long currentTime = millis();

  if (currentIndex == 255 || (currentTime - prevTime >= settings.allModeDelay)) {
    if (currentIndex == 255) {
      currentIndex = 0;
    }

    prevTime = currentTime;

    if (settings.allModesWorkType == 1) {
      if (currentIndex > sizeof(modes) / sizeof(modes[0])) {
        currentIndex = 0;
      }

      mode = modes[currentIndex].first;
      subMode = modes[currentIndex].second;

      currentIndex += 1;

    } else {
      currentIndex = ESP8266TrueRandom.random(60);

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