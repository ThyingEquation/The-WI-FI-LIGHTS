#include <main.h>

CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);

Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    kMatrixWidth, kMatrixHeight, PIN,
    NEO_MATRIX_BOTTOM + TEXT_POS + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);


uint8_t choosenModeD1 = 255;
uint8_t choosenModeD2 = 255;

char ssid[15];
const char *password = "134599996";
char start_mode[3];

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
  matrix.setTextColor(getMatrixColorByIndex(0));

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

  //choosenModeD1 = 1;
  //choosenModeD2 = 7;
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

  strncpy(start_mode, startMode, 3);
  saveConfig();
  ESP.restart();
}

void (*funcArray[12])() = {runningLine,    // 0
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
                           extra};         // 11

void loop() {
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis >= 100) {
    lastMillis = currentMillis;
    server.handleClient();
  }

  if (choosenModeD1 <= 11) {
    funcArray[choosenModeD1]();
  }
}

void settingsProcessing() {
  clearStrip();
  if (server.hasArg("ssid")) {
    String newSsid = server.arg("ssid");
    changeSSID(newSsid);
  }
  // else if (command.indexOf("/CSM") != -1) {
  //   FastLED.clear();
  //   String nSM = command;
  //   nSM.remove(0, 8);
  //   nSM.remove(((nSM.length() - 9) + 1), 9);
  //   changeSM(nSM);
  // }
}

void paintingProcessing(void) {
  if (server.hasArg("canvas")) {
    canvas(server.arg("canvas"));
  }
}

void mainProcessing() {
  clearStrip();
  if (server.hasArg("ok")) {
  } else if (server.hasArg("stop")) {
    choosenModeD1 = 255;
    choosenModeD2 = 255;
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

void clearStrip() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
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
    strncpy(start_mode, json_startmode, 3);
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

uint32_t getMatrixColor(const RGBColorX &colorX) {
  return matrix.Color(colorX.r, colorX.g, colorX.b);
}

uint32_t getStripColor(const RGBColorX &colorX) {
  return strip.Color(colorX.r, colorX.g, colorX.b);
}

uint32_t getMatrixColorByIndex(int index) {
  if (index < 0 || index >= sizeof(colorsX) / sizeof(colorsX[0])) {
    return matrix.Color(0, 0, 0);
  }
  return getMatrixColor(colorsX[index]);
}

uint32_t getStripColorByIndex(int index) {
  if (index < 0 || index >= sizeof(colorsX) / sizeof(colorsX[0])) {
    return strip.Color(0, 0, 0);
  }
  return getStripColor(colorsX[index]);
}