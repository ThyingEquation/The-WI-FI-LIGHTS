#include <main.h>

CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);

Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    mWidth, mHeight, PIN,
    NEO_MATRIX_BOTTOM + TEXT_POS + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);

uint8_t choosenModeD1 = 255;
uint8_t choosenModeD2 = 255;
uint8_t allModsEnable = 0;

const int modeArray[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, /*42,*/ /*43,*/ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56};
const int arraySize = sizeof(modeArray) / sizeof(modeArray[0]);

unsigned long prevTime = 0;
const unsigned long modeDelay = 15 * 60 * 1000;  // 15 минут в миллисекундах
int currentIndex = 255;

char ssid[15];
const char *password = "134599996";
char start_mode[2];

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server(80);

void setup() {
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS)
      .setCorrection(TypicalSMD5050)
      .setDither(BRIGHTNESS <= 255);
  FastLED.setBrightness(BRIGHTNESS);

  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);
  strip.show();

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(BRIGHTNESS);
  matrix.setTextColor(pgm_read_dword(&(mainColors[0])));

  randomSeed(analogRead(0));

  delay(500);

  if (!LittleFS.begin()) {
    return;
  }

  loadConfig();
  saveConfig();

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  server.on("/settings", settingsProcessing);
  server.on("/command", mainProcessing);
  server.on("/painting", paintingProcessing);

  server.begin();

  String mode = start_mode;
  uint8_t modeNum = mode.toInt();
  checkMode(modeNum);

  delay(1000);
}

void changeSSID(String ssidW) {
  int sid_str_len = ssidW.length() + 1;
  char webssid[sid_str_len];
  ssidW.toCharArray(webssid, sid_str_len);
  strncpy(ssid, webssid, 15);
  saveConfig();
  ESP.restart();
}

void changeSM(String startM) {
  int sm_str_len = startM.length() + 1;
  char startMode[sm_str_len];
  startM.toCharArray(startMode, sm_str_len);

  strncpy(start_mode, startMode, 2);
  saveConfig();
  ESP.restart();
}

void (*funcArray[13])() = {runningLine,    // 0
                           colorfulSpots,  // 1
                           rainbows,       // 2
                           draw,           // 3
                           runningLights,  // 4
                           space,          // 5
                           flashLights,    // 6
                           water,          // 7
                           snow,           // 8
                           colorfulWaves,  // 9
                           anime,          // 10
                           extra,          // 11
                           allMods};       // 12

void loop() {
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis >= 100) {
    lastMillis = currentMillis;
    server.handleClient();
  }

  if (allModsEnable == 1) {
    allMods();
  }

  if (choosenModeD1 <= 12) {
    funcArray[choosenModeD1]();
  }
}

void settingsProcessing() {
  strip.clear();
  strip.show();
  if (server.hasArg("ssid")) {
    String newSsid = server.arg("ssid");
    changeSSID(newSsid);
  } else if (server.hasArg("startMode")) {
    String newStartMode = server.arg("startMode");
    changeSM(newStartMode);
  }
}

void paintingProcessing(void) {
  if (server.hasArg("canvas")) {
    canvas(server.arg("canvas"));
  }
}

void mainProcessing() {
  allModsEnable = 0;
  strip.clear();
  strip.show();
  if (server.hasArg("ok")) {
  } else if (server.hasArg("stop")) {
    choosenModeD1 = 255;
    choosenModeD2 = 255;
  } else if (server.hasArg("allModes")) {
    currentIndex = 255;
    allModsEnable = 1;
    choosenModeD1 = 255;
    choosenModeD2 = 255;
    allMods();
  } else if (server.hasArg("runLine")) {
    g = 13;
    choosenModeD1 = 0;
    argIndexCheck("runLine");
    runningLine();
  } else if (server.hasArg("spots")) {
    choosenModeD1 = 1;
    colorfulSpots();
  } else if (server.hasArg("rainbow")) {
    choosenModeD1 = 2;
    argIndexCheck("rainbow");
    rainbows();
  } else if (server.hasArg("draw")) {
    col = 0;
    ledsCount = 0;
    choosenModeD1 = 3;
    argIndexCheck("draw");
    draw();
  } else if (server.hasArg("runLights")) {
    col = random(128);
    ledsCount = 0;
    choosenModeD1 = 4;
    argIndexCheck("runLights");
    runningLights();
  } else if (server.hasArg("space")) {
    choosenModeD1 = 5;
    argIndexCheck("space");
    space();
  } else if (server.hasArg("flashLights")) {
    col = 0;
    choosenModeD1 = 6;
    argIndexCheck("flashLights");
    flashLights();
  } else if (server.hasArg("water")) {
    choosenModeD1 = 7;
    argIndexCheck("water");
    water();
  } else if (server.hasArg("snow")) {
    choosenModeD1 = 8;
    snow();
  } else if (server.hasArg("colorfulWaves")) {
    choosenModeD1 = 9;
    argIndexCheck("colorfulWaves");
    colorfulWaves();
  } else if (server.hasArg("anime")) {
    currentStepJP = 0;
    currentPhaseJP = 0;
    currentLetterJP = 0;
    currentStepKR = 0;
    currentPhaseKR = 0;
    currentLetterKR = 0;
    initArrLetters();
    choosenModeD1 = 10;
    argIndexCheck("anime");
    anime();
  } else if (server.hasArg("extra")) {
    initPoints();
    choosenModeD1 = 11;
    argIndexCheck("extra");
    extra();
  }
}

void argIndexCheck(String argu) {
  if (server.arg(argu) == "1") {
    choosenModeD2 = 1;
  } else if (server.arg(argu) == "2") {
    choosenModeD2 = 2;
  } else if (server.arg(argu) == "3") {
    choosenModeD2 = 3;
  } else if (server.arg(argu) == "4") {
    choosenModeD2 = 4;
  } else if (server.arg(argu) == "5") {
    choosenModeD2 = 5;
  } else if (server.arg(argu) == "6") {
    choosenModeD2 = 6;
  } else if (server.arg(argu) == "7") {
    choosenModeD2 = 7;
  } else if (server.arg(argu) == "8") {
    choosenModeD2 = 8;
  } else if (server.arg(argu) == "9") {
    choosenModeD2 = 9;
  } else if (server.arg(argu) == "10") {
    choosenModeD2 = 10;
  } else if (server.arg(argu) == "11") {
    choosenModeD2 = 11;
  } else if (server.arg(argu) == "12") {
    choosenModeD2 = 12;
  } else if (server.arg(argu) == "13") {
    choosenModeD2 = 13;
  } else if (server.arg(argu) == "14") {
    choosenModeD2 = 14;
  } else if (server.arg(argu) == "15") {
    choosenModeD2 = 15;
  }
}

bool loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 256) {
    Serial.println("Config file size is too large");
    configFile.close();
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  size_t bytesRead = configFile.readBytes(buf.get(), size);
  configFile.close();

  if (bytesRead != size) {
    Serial.println("Failed to read config file");
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to deserialize JSON");
    return false;
  }

  const char *json_startmode = doc["json_startmode"];
  if (json_startmode) {
    strncpy(start_mode, json_startmode, 2);
  }

  const char *json_ssid = doc["json_ssid"];
  if (json_ssid) {
    strncpy(ssid, json_ssid, 15);
  }

  return true;
}

bool saveConfig() {
  JsonDocument json;

  json["json_startmode"] = start_mode;
  json["json_ssid"] = ssid;

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(json, configFile);
  configFile.close();

  return true;
}

struct Mode {
  uint8_t choosenModeD1;
  uint8_t choosenModeD2;
};

const struct Mode modes[] = {
    {255, 255},  //[0] нет стартового режима
    {3, 3},      //[1] Полная заливка
    {3, 1},      //[2] Заливка змейкой 1
    {3, 2},      //[3] Заливка змейкой 2
    {3, 4},      //[4] Заливка змейкой 3
    {3, 5},      //[5] Цветное дыхание
    {1, 255},    //[6] цветные пятна
    {2, 1},      //[7] радуга колесо
    {2, 2},      //[8] радуга волной 1
    {2, 3},      //[9] радуга волной 2
    {2, 4},      //[10] радуга змейкой
    {4, 1},      //[11] бегущий огонек медленно
    {4, 2},      //[12] бегущий огонек быстро
    {4, 6},      //[13] бегущий огонек цветной
    {4, 7},      //[14] бегущие огоньки 1
    {4, 8},      //[15] бегущие огоньки 2
    {4, 10},     //[16] цветная змейка
    {4, 3},      //[17] прыгающие огоньки 1
    {4, 4},      //[18] прыгающие огоньки 2
    {4, 5},      //[19] прыгающие огоньки 3
    {4, 9},      //[20] прыгающие огоньки 4
    {5, 2},      //[21] космические корабли
    {5, 1},      //[22] звездное небо
    {5, 3},      //[23] пульсар
    {5, 4},      //[24] метеорный поток
    {6, 1},      //[25] мерцающие огни 1
    {6, 2},      //[26] мерцающие огни 2
    {6, 3},      //[27] мерцающие огни 3
    {7, 1},      //[28] лагуна
    {7, 2},      //[29] бассейн
    {8, 255},    //[30] метель
    {9, 1},      //[31] цветные волны 1
    {9, 2},      //[32] цветные волны 2
    {9, 3},      //[33] цветные волны 3
    {9, 4},      //[34] цветные волны 4
    {9, 5},      //[35] цветные волны 5
    {10, 1},     //[36] Анимации: Сердце
    {10, 2},     //[37] Смайлик
    {10, 3},     //[38] Прыгающий человечек
    {10, 4},     //[39] Файербол
    {10, 5},     //[40] Взрыв
    {10, 6},     //[41] Пакман
    {10, 7},     //[42] "С НОВЫМ ГОДОМ" на японском
    {10, 8},     //[43] Приветствие на корейском
    {10, 9},     //[44] Различные картинки
    {10, 10},    //[45] Цифровой сигнал
    {10, 11},    //[46] Синусоида
    {10, 12},    //[47] Цветные синусоиды
    {10, 13},    //[48] Цветные линии 1
    {10, 14},    //[49] Цветные линии 2
    {11, 1},     //[50] Прыгающий квадрат
    {11, 2},     //[51] Прыгающие точки
    {11, 3},     //[52] Цветная спираль
    {11, 4},     //[53] Игра змейка
    {11, 5},     //[54] Игра тетрис
    {11, 6},     //[55] Игра арканоид
    {11, 7}      //[56] Эффект из к/ф матрица
};

void checkMode(uint8_t num) {
  if (num < sizeof(modes) / sizeof(modes[0])) {
    choosenModeD1 = modes[num].choosenModeD1;
    choosenModeD2 = modes[num].choosenModeD2;
  }
}

void allMods() {
  unsigned long currentTime = millis();

  if (currentIndex == 255 || (currentTime - prevTime >= modeDelay)) {
    if (currentIndex == 255) {currentIndex = 0;} 
    prevTime = currentTime;
    checkMode(modeArray[currentIndex]);
    currentIndex = (currentIndex + 1) % arraySize;
    strip.clear();
    strip.show();
  }
}