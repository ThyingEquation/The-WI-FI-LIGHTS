#include <main.h>

#define AUTO_SELECT_BACKGROUND_COLOR 0
#define COOL_LIKE_INCANDESCENT 1
#define TWINKLE_SPEED 4
#define TWINKLE_DENSITY 5


CRGB gBackgroundColor = CRGB::Black;
CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;
CRGBArray<NUMPIXELS> Leds;

CRGBPalette16 currentPalette(CloudColors_p);

#if MATRIX_SIZE_TYPE == 11
   const uint8_t kMatrixWidth = 16;
   const uint8_t kMatrixHeight = 16;
   int startupColorLine = 0;
   int BRIGHTNESS = 50;

#elif MATRIX_SIZE_TYPE == 12
   const uint8_t kMatrixWidth = 16;
   const uint8_t kMatrixHeight = 16;
   int startupColorLine = 0;
   int BRIGHTNESS = 50;

#elif MATRIX_SIZE_TYPE == 21
   const uint8_t kMatrixWidth = 12;
   const uint8_t kMatrixHeight = 13;
   int startupColorLine = 0;
   int BRIGHTNESS = 50;

#elif MATRIX_SIZE_TYPE == 22
   const uint8_t kMatrixWidth = 12;
   const uint8_t kMatrixHeight = 13;
   int startupColorLine = 0;
    int BRIGHTNESS = 50;

#elif MATRIX_SIZE_TYPE == LINE18
   const uint8_t kMatrixWidth = 18;
   const uint8_t kMatrixHeight = 1;
   int BRIGHTNESS = 50;
   int startupColorLine = 1;

#endif

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define MAX_DIMENSION ((kMatrixWidth > kMatrixHeight) ? kMatrixWidth : kMatrixHeight)

uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];

uint16_t X;
uint16_t Y;
uint16_t Z;

uint16_t XY(uint8_t X, uint8_t Y) {
  uint16_t i;
  if (kMatrixSerpentineLayout == false) {
    i = (Y * kMatrixWidth) + X;
  }
  if (kMatrixSerpentineLayout == true) {
    if (Y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - X;
      i = (Y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (Y * kMatrixWidth) + X;
    }
  }
  return i;
}

uint16_t XYsafe(uint8_t X, uint8_t Y) {
  if (X >= kMatrixWidth) return -1;
  if (Y >= kMatrixHeight) return -1;
  return XY(X, Y);
}

 const bool kMatrixSerpentineLayout = true;  

 CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
 CRGB* const leds(leds_plus_safety_pixel + 1);


#if MATRIX_SIZE_TYPE == 11
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN, NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
#elif MATRIX_SIZE_TYPE == 12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
 Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN, NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
#elif MATRIX_SIZE_TYPE == 21
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(12, 13, PIN, NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
#elif MATRIX_SIZE_TYPE == 22
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(12, 13, PIN, NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
#elif MATRIX_SIZE_TYPE == LINE18
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(18, 1, PIN, NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
#endif

String start = "";
String request;
String choosenMode;

int pass = 0;
int col = 0;
unsigned int l = 0;
unsigned int b = 0;
int g = matrix.width();

char wifi_ssid[15]; //.................................... CHANGE SSID {COMMAND}
//char wifi_password[15]; //........................
const char* wifi_password = "11111111";
char start_mode[3]; //........................

IPAddress local_ip(192, 168, 1, 1); 
IPAddress gateway(192, 168, 1, 1);  
IPAddress subnet(255, 255, 255, 0); 
WiFiServer server(80);


//.................................... CHANGE SSID {COMMAND} ..................................................................
bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if(!configFile) {
 //   Serial.println("Failed to open config file:");
    return false;
  }
  size_t size = configFile.size();
 // Serial.println("size ");
//  Serial.println(size);
  if(size > 256){
//    Serial.println("Config file size is to large");
    return false;
  }
  //Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  //Serial.println(buf.get());
  
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  if(!json.success()){
 //   Serial.println("Failed to parse config file");
    return false; 
  }
    

  const char* json_startmode = json["json_startmode"];
  strncpy(start_mode, json_startmode, 3);
  Serial.print("start mode Loaded: ");
  Serial.println(start_mode);

  const char* json_ssid = json["json_ssid"];
  strncpy(wifi_ssid, json_ssid, 15);
  Serial.print("ssid Loaded: ");
  Serial.println(wifi_ssid);
 
  //const char* json_paswd = json["json_paswd"];
  //strncpy(wifi_password, json_paswd, 15);
  //Serial.print("paswd Loaded: ");
  //Serial.println(wifi_password);
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  json["json_startmode"] = start_mode;
  json["json_ssid"] = wifi_ssid;
  //json["json_paswd"] = wifi_password;
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
 //   Serial.println("Failed to open config file for writing");
    return false;
  }
  json.printTo(configFile);
  return true;
}
//.................................... CHANGE SSID {COMMAND} ..................................................................



void setup() {

  //BRIGHTNESS = 50;  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.begin(9600); 
  delay(500);

  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050).setDither(BRIGHTNESS <= 255); //////////
  FastLED.setBrightness(BRIGHTNESS);
  chooseNextColorPalette(gTargetPalette);

  X = random16();
  Y = random16();
  Z = random16();

  strip.begin();
  strip.show();
   if (startupColorLine == 1) {
  strip.setBrightness(255);
 }
  else strip.setBrightness(51);
  strip.show();

  matrix.begin();
  matrix.setTextWrap(false);
     if (startupColorLine == 1) {
  matrix.setBrightness(255);
 }
  else matrix.setBrightness(51);
  matrix.setTextColor(colorsMatrix[0]);


  delay(500);

  if(!SPIFFS.begin()){
 //   Serial.println("Failed to mount file system");
    return;
  }  

  loadConfig();
  saveConfig();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(wifi_ssid, wifi_password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(200);
  server.begin();


  choosenMode = start_mode;

  if (choosenMode.indexOf("100") != -1) {  // стартовый режим, заливка цветом

   choosenMode = "/S/257/A/C"; //white
 
  }

  else if (choosenMode.indexOf("101") != -1) {  

   choosenMode = "/S/257/A/D"; //blue
 
  }

    else if (choosenMode.indexOf("102") != -1) {  

   choosenMode = "/S/257/A/E"; //lime
 
  }

    else if (choosenMode.indexOf("103") != -1) {  

   choosenMode = "/S/257/A/F"; //yellow
 
  }

    else if (choosenMode.indexOf("104") != -1) {  

   choosenMode = "/S/257/A/G"; //orange
 
  }

    else if (choosenMode.indexOf("105") != -1) {  

   choosenMode = "/S/257/A/H"; //red
 
  }

    else if (choosenMode.indexOf("106") != -1) {  

   choosenMode = "/S/257/A/I"; //crimson color (raspberry)
 
  }

    else if (choosenMode.indexOf("107") != -1) {  

   choosenMode = "/S/257/A/J"; //violet
 
  }

    else if (choosenMode.indexOf("108") != -1) {  

   choosenMode = "/S/257/A/K"; //spring green
 
  }

    else if (choosenMode.indexOf("109") != -1) {  

   choosenMode = "/S/257/A/L"; //gold
 
  }

    else if (choosenMode.indexOf("110") != -1) {  

   choosenMode = "/S/257/A/N"; //light blue
 
  }

    else if (choosenMode.indexOf("111") != -1) {  

   choosenMode = "/S/257/A/O"; //aqua color
 
  }

    else if (choosenMode.indexOf("112") != -1) {  

   choosenMode = "/S/257/A/P"; //indigo
 
  }

    else if (choosenMode.indexOf("113") != -1) {  

   choosenMode = "/S/257/A/Q"; //light pink
 
  }

    else if (choosenMode.indexOf("114") != -1) {  

   choosenMode = "/S/257/A/R"; //green
 
  }
}



//  PORTS (the lights)
// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/



void changeSSID(String ssid) {

   // String ssid;// = command;//server.arg("value"); //////////////////////////////////////////
   // String qpass = server.arg("pass"); == if this enable, will be possible CHANGE PASSWORD ==
    int sid_str_len = ssid.length() + 1; 
    char webssid[sid_str_len];
    ssid.toCharArray(webssid, sid_str_len);
   // int pass_str_len = qpass.length() + 1; == if this enable, will be possible CHANGE PASSWORD ==
   // char webpass[pass_str_len]; == if this enable, will be possible CHANGE PASSWORD ==
   // qpass.toCharArray(webpass, pass_str_len); == if this enable, will be possible CHANGE PASSWORD ==
      
        strncpy(wifi_ssid, webssid, 15);
 
 //       strncpy(wifi_password, webpass, 15); == if this enable, will be possible CHANGE PASSWORD == 
 //       Serial.print("writing eeprom pass:"); == if this enable, will be possible CHANGE PASSWORD ==
  //      Serial.println(wifi_password); == if this enable, will be possible CHANGE PASSWORD ==
  //      Serial.print("\n"); == if this enable, will be possible CHANGE PASSWORD ==
           
        saveConfig();   
    //    statusCode = 200; /////////////////////////////////////////////////////////////////////////////////////////////////////////
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



void loop() {

   String* pCommand = &choosenMode;

  WiFiClient client = server.accept();
  if (!client) {
      
  if (choosenMode.indexOf("/T") != -1) {  // режим 2. ожидающий цикл

    mode2(choosenMode);
 
  }

    else if (choosenMode.indexOf("/S") != -1) {  // режим 1. рисование попиксельное
    mode1(pCommand);
  }

  else if (choosenMode.indexOf("/U") != -1) {  // режим 3. ожидающий цикл
    mode3();
  }

  else if (choosenMode.indexOf("/V") != -1) {  // режим 4. ожидающий цикл
    mode4(choosenMode);
  }

   else if (choosenMode.indexOf("/W") != -1) {  // режим 5. ожидающий цикл
    mode5(choosenMode);
  }

   else if (choosenMode.indexOf("/X") != -1) {  // режим 6. ожидающий цикл
    mode6(choosenMode);
  }

  else if (choosenMode.indexOf("/Y") != -1) {  // режим 7. ожидающий цикл
    mode7(choosenMode);
  }

   else if (choosenMode.indexOf("/Z") != -1) {  // режим 8. ожидающий цикл

   FastLED.addLeds<LED_TYPE, PIN, COLOR_ORDER>(Leds, NUM_LEDS).setCorrection(TypicalSMD5050).setDither(BRIGHTNESS <= 255); /////////////////////////////////////////////////////////////////


   if (choosenMode.indexOf("/Z4") != -1) {
    FastLED.addLeds<LED_TYPE, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050).setDither(BRIGHTNESS <= 255); ////////// 
    mode8(choosenMode);
   }

    mode8(choosenMode);
  }

   else if (choosenMode.indexOf("/A") != -1) {  // режим 9. ожидающий цикл
    mode9(choosenMode);
  }

   else if (choosenMode.indexOf("/B") != -1) {  // режим 10. ожидающий цикл
    mode10(choosenMode);
  }

   else if (choosenMode.indexOf("/C") != -1) {  // режим 11. ожидающий цикл
    mode11(choosenMode);
  }
  }

  if (!client) {
    return;
  }
  
  while (!client.available()) {
    delay(5);
  }

  
 // command = client.readStringUntil('\r');

  String command = client.readStringUntil('\r');

  modes(command);

  command = "";

  client.flush();
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += request;
  client.print(s);
  delay(1);
 
}


void modes(String command) {


  String* pCommand = &command;

  if (command.indexOf("/ok") != -1) {
    request = "ok";
    start = "ok";

  }

    else if (command.indexOf("/CSM") != -1) {  // смена стартового режима
     
     String nSM = command;
     nSM.remove(0,8);
     nSM.remove(((nSM.length()-9)+1),9);
     changeSM(nSM);

  }

  else if (command.indexOf("/CSD") != -1) {  // смена имени сети
     
     String nSSID = command;
     nSSID.remove(0,8);
     nSSID.remove(((nSSID.length()-9)+1),9);
     changeSSID(nSSID);

  }

  else if (command.indexOf("/M") != -1) {  // Настройка яркости
    brightness(pCommand);
  }

  else if (command.indexOf("/S") != -1) {  // режим 1. рисование попиксельное
    mode1(pCommand);
  }

  else if (command.indexOf("/T") != -1) {  // режим 2. вывод текста
    choosenMode = command;
    pass = random(127);
    b = 0; 
    mode2(choosenMode);
  }

  else if (command.indexOf("/t") != -1) {  // режим 2. выход
    choosenMode = " ";
    g = matrix.width();
    pass = 0;
    b = 0;
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  else if (command.indexOf("/U") != -1) {  // режим 3. тление (пятна)
    choosenMode = command;
    mode3();
  }

  else if (command.indexOf("/u") != -1) {  // режим 3. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  else if (command.indexOf("/V") != -1) {  // режим 4. радуга
    choosenMode = command;
    mode4(choosenMode);
  }

  else if (command.indexOf("/v") != -1) {  // режим 4. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  else if (command.indexOf("/W") != -1) {  // режим 5. заливка
    choosenMode = command;
    col = 0;
    l =0;
    mode5(choosenMode);
  }

  else if (command.indexOf("/w") != -1) {  // режим 5. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  else if (command.indexOf("/X") != -1) {  // режим 6. бегущий огонек
    choosenMode = command;
    col = random(128);
    l =0;
    mode6(choosenMode);
  }

  else if (command.indexOf("/x") != -1) {  // режим 6. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  else if (command.indexOf("/Y") != -1) {  // режим 7. звездное небо
    choosenMode = command;
    mode7(choosenMode);
  }

  else if (command.indexOf("/y") != -1) {  // режим 7. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }


  else if (command.indexOf("/Z") != -1) {  // режим 8. мерцающие огни
   FastLED.addLeds<LED_TYPE, PIN, COLOR_ORDER>(Leds, NUM_LEDS).setCorrection(TypicalSMD5050).setDither(BRIGHTNESS < 255); ///////////////////////////////////////////////////////////////////
    delay(10);
    choosenMode = command;
    col = 0;
    mode8(choosenMode);
  }

  else if (command.indexOf("/z") != -1) {  // режим 8. выход
  FastLED.addLeds<LED_TYPE, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050).setDither(BRIGHTNESS < 255); //////////
   delay(10);
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  else if (command.indexOf("/A") != -1) {  // режим 9. морская волна
    choosenMode = command;
    mode9(choosenMode);
  }

  else if (command.indexOf("/a") != -1) {  // режим 9. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }


  else if (command.indexOf("/B") != -1) {  // режим 10. различные эффекты (изменить название)
    choosenMode = command;
    mode10(choosenMode);
  }

  else if (command.indexOf("/b") != -1) {  // режим 10. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  else if (command.indexOf("/C") != -1) {  // режим 11. цветные волны
    choosenMode = command;
    mode11(choosenMode);
  }

  else if (command.indexOf("/c") != -1) {  // режим 11. выход
    choosenMode = " ";
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }
}

const uint16_t colorsMatrix[] = {  matrix.Color(93, 138, 168),   // Военно-воздушный синий    // Английский алфавит. Буква А
                            matrix.Color(227, 38, 54),    // Ализариновый красный
                            matrix.Color(255, 191, 0),    // Янтарный
                            matrix.Color(153, 102, 204),  // Аметистовый
                            matrix.Color(242, 243, 244),  // Матовый белый                           
                            matrix.Color(141, 182, 0),    // Яблочно-зелёный                         
                            matrix.Color(0, 255, 255),    // Цвет морской волны
                            matrix.Color(75, 83, 32),     // Армейский зелёный
                            matrix.Color(254, 111, 94),   // Горький розовый  
                            matrix.Color(0, 0, 255),      // Синий
                            matrix.Color(204, 0, 0),      // Красный Университета Бостона
                            matrix.Color(181, 166, 66),   // Латунный
                            matrix.Color(102, 255, 0),    // Ярко-зелёный
                            matrix.Color(8, 232, 222),    // Ярко-бирюзовый
                            matrix.Color(205, 0, 205),    // Ярко-фиолетовый      
                            matrix.Color(144, 0, 32),     // Бургундский   
                            matrix.Color(233, 116, 81),   // Сиена жжёная
                            matrix.Color(119, 41, 83),    // Баклажановый
                            matrix.Color(196, 30, 58),    // Тёмно-красный, кардинал
                            matrix.Color(172, 225, 175),  // Селадоновый
                            matrix.Color(222, 49, 99),    // Светлая вишня
                            matrix.Color(42, 82, 190),    // Лазурно-синий
                            matrix.Color(127, 255, 0),    // Салатовый цвет, шартрез
                            matrix.Color(251, 236, 93),   // 	Кукурузный
                            matrix.Color(220, 20, 60),    // Тёмно-красный, кровавый, Малиновый
                            matrix.Color(0, 255, 255),    // Циан, васильковый
                            matrix.Color(189, 183, 107),  // Тёмный хаки, хаки
                            matrix.Color(3, 192, 60),     // Тёмный пастельно-зелёный
                            matrix.Color(255, 218, 185),  // Тёмно-персиковый
                            matrix.Color(231, 84, 128),   // Тёмно-розовый
                            matrix.Color(0, 51, 153),     // Тёмно-Пороховая синь
                            matrix.Color(233, 150, 122),  // Тёмная лососина
                            matrix.Color(86, 3, 25),      // Тёмно-алый
                            matrix.Color(23, 114, 69),    // Тёмный весенне-зелёный
                            matrix.Color(255, 168, 18),   // Тёмно-мандариновый
                            matrix.Color(66, 49, 137),    // Глубокий фиолетовый цвет
                            matrix.Color(255, 20, 147),   // Глубокий розовый цвет
                            matrix.Color(80, 200, 120),   // Изумрудный                                                               // Буква E
                            matrix.Color(153, 0, 102),    // Баклажановый
                            matrix.Color(178, 34, 34),    // Кирпичный
                            matrix.Color(255, 0, 255),    // Фуксия
                            matrix.Color(255, 215, 0),    // Золотой
                            matrix.Color(223, 115, 255),  // Гелиотроп  
                            matrix.Color(0, 255, 0),      // Зелёный
                            matrix.Color(0, 125, 255),    // Синий Градуса
                            matrix.Color(173, 255, 47),   // Жёлто-зелёный                                                           // Буква H
                            matrix.Color(252, 15, 192),   // Ярко-розовый цвет
                            matrix.Color(75, 0, 130),     // Индиго                                                                   // Буква I
                            matrix.Color(255, 79, 0),     // Международный (сигнальный) оранжевый
                            matrix.Color(205, 92, 92),    // Индийский красный, каштановый
                            matrix.Color(0, 168, 107),    // Нефритовый                                                               // Буква J                                                                // Буква K
                            matrix.Color(58, 117, 196),   // Синий Кляйна
                            matrix.Color(230, 230, 250),  // Лаванда, лавандово-синий                                                // Буква L
                            matrix.Color(204, 255, 0),    // Лайм
                            matrix.Color(253, 233, 16),   // Лимонный
                            matrix.Color(200, 162, 200),  // Сиреневый
                            matrix.Color(250, 240, 230),  // Зелёная лужайка
                            matrix.Color(255, 0, 255),    // Маджента, фуксия                                                          // Буква M
                            matrix.Color(11, 218, 81),    // Малахитовая зелень (малахитовый)
                            matrix.Color(128, 0, 0),      // Коричнево-малиновый
                            matrix.Color(152, 255, 152),  // Зелёная мята (мятный)
                            matrix.Color(153, 51, 102),   // Розовато-лиловый
                            matrix.Color(0, 51, 102),     // Полуночно-синий
                            matrix.Color(173, 223, 173),  // Зелёный мох
                            matrix.Color(153, 122, 141),  // Розовый Маунтбэттена
                            matrix.Color(255, 219, 88),   // Горчичный                                                           // Буква N
                            matrix.Color(0, 0, 128),      // Тёмно-синий
                            matrix.Color(204, 119, 34),   // Охра                                                                     // Буква O
                            matrix.Color(107, 142, 35),   // Нежно-оливковый
                            matrix.Color(255, 165, 0),    // Оранжевый
                            matrix.Color(218, 112, 214),  // Орхидея
                            matrix.Color(253, 245, 230),  // Старое кружево
                            matrix.Color(175, 238, 238),  // Бледно-синий                                                           // Буква P
                            matrix.Color(152, 118, 84),   // 	Бледно-коричневыйй
                            matrix.Color(175, 64, 53),    // Бледно-карминный, ср. Кармин
                            matrix.Color(221, 173, 175),  // Бледно-каштановый
                            matrix.Color(249, 132, 229),  // Бледно-пурпурный
                            matrix.Color(250, 218, 221),  // Бледно-розовый
                            matrix.Color(219, 112, 147),  // Лиловый
                            matrix.Color(218, 189, 171),  // Бледно-песочный цвет
                            matrix.Color(240, 220, 130),  // Палевый
                            matrix.Color(199, 252, 236),  // Панг
                            matrix.Color(255, 239, 213),  // Побег папайи
                            matrix.Color(119, 221, 119),  // Пастельно-зелёный
                            matrix.Color(255, 209, 220),  // Пастельно-розовый
                            matrix.Color(209, 226, 49),   // Грушевый
                            matrix.Color(204, 20, 255),   // Барвинок
                            matrix.Color(102, 0, 255),    // Персидский синий
                            matrix.Color(1, 121, 111),    // 	Зелёная сосна
                            matrix.Color(255, 192, 203),  // Розовый
                            matrix.Color(255, 153, 102),  // Оранжево-розовый
                            matrix.Color(102, 0, 102),    // Сливовый
                            matrix.Color(204, 136, 153),  // Красновато-коричневый, пюсовый (устар.), тёмно-коричневый, блошиный
                            matrix.Color(255, 117, 24),   // Тыква
                            matrix.Color(128, 0, 128),    // Фиолетовый (оттенок)                                                                 // Буква R
                            matrix.Color(255, 0, 0),      // Красный
                            matrix.Color(199, 21, 133),   // Фиолетово-красный
                            matrix.Color(65, 105, 225),   // Ярко-синий («королевский синий»)
                            matrix.Color(255, 153, 0),    // Сигнальный оранжевый                                                    // Буква S 
                            matrix.Color(8, 37, 103),     // Сапфировый
                            matrix.Color(210, 134, 200),  // Салутовый
                            matrix.Color(255, 55, 105),   // Лососёвый
                            matrix.Color(146, 0, 10),     // Сангрия
                            matrix.Color(255, 36, 0),     // Ярко-красный, алый
                            matrix.Color(255, 216, 0),    // Цвет жёлтого школьного автобуса
                            matrix.Color(46, 139, 87),    // Зелёное море
                            matrix.Color(255, 245, 238),  //  Цвет морской ракушки
                            matrix.Color(0, 255, 127),    // Зелёная весна
                            matrix.Color(70, 130, 180),   // Синяя сталь
                            matrix.Color(172, 183, 142),  // Болотный
                            matrix.Color(210, 180, 140),  // Цвет загара                                                             // Буква T
                            matrix.Color(205, 87, 0),     // Краснобуро-оранжевый
                            matrix.Color(255, 204, 0),    // Мандариновый
                            matrix.Color(208, 240, 192),  // Зелёный чай
                            matrix.Color(0, 128, 128),    // Сине-зелёный
                            matrix.Color(216, 191, 216),  // Чертополох
                            matrix.Color(48, 213, 200),   // Бирюзовый, ср. бирюза
                            matrix.Color(255, 99, 71),    // Томатный                                                        // Буква U
                            matrix.Color(91, 146, 229),   // Цвет флага ООН
                            matrix.Color(221, 72, 20),    //Оранжевый Ubuntu
                            matrix.Color(243, 229, 171),  // Ванильный                                                               // Буква V
                            matrix.Color(139, 0, 255),    // Фиолетовый
                            matrix.Color(64, 130, 109),   // Зелень Гинье
                            matrix.Color(255, 255, 255),  // Белый
                            matrix.Color(201, 160, 220),  // Глициния
                            matrix.Color(114, 47, 55),    // Вино                                    
                            matrix.Color(255, 255, 0),    // Жёлтый                                                                  // Буква Y
                            matrix.Color(235, 194, 175),  // Циннвальдит                                                             // Буква Z
                            matrix.Color(0, 20, 168) };   // Кобальтовая синь
