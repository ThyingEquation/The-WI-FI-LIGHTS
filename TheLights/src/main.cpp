#include "lightsServer.h"
#include "lightsSettings.h"
#include "modes.h"
#include "colors.h"

appSettings_s settings;

CRGB leds_plus_safety_pixel[MATRIX_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MATRIX_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    MATRIX_WIDTH, MATRIX_HEIGHT, PIN,
    NEO_MATRIX_BOTTOM + TEXT_POS + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);

void (*modeFunctions[14])(uint8_t) = {
    drawRunningLines, drawColorfulSpots, drawRainbows, drawCanvasEffects, drawRunningLights,
    drawSpaceEffects, drawFlashLights, drawWaterEffects, drawWeatherEffects, drawColorfulWaves,
    drawAnimations, drawGamesEffects, drawJumpingLights, drawMatrixMovieEffect};


static uint32_t startingMillis = 0;

void setup()
{
  if (!LittleFS.begin())
  {
    return;
  }

  if (loadSettings())
  {
    return;
  }

  Serial.begin(9600);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, MATRIX_LEDS)
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
  matrix.setTextColor(pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(0, 128)])));

  delay(10);

  IPAddress localIp(settings.localIpVal[0], settings.localIpVal[1], settings.localIpVal[2], settings.localIpVal[3]);
  IPAddress gateway(settings.gatewayVal[0], settings.gatewayVal[1], settings.gatewayVal[2], settings.gatewayVal[3]);
  IPAddress subnet(settings.subnetVal[0], settings.subnetVal[1], settings.subnetVal[2], settings.subnetVal[3]);

  WiFi.softAP(settings.ssid, settings.password);
  WiFi.softAPConfig(localIp, gateway, subnet);

  delay(100);

  server.on("/settings", handleSettings);
  server.on("/command", handleMainCommand);
  server.on("/painting", handlePainting);
  server.on("/drawingImages", handleDrawImage);
  server.begin();

  if (settings.modeNum != 99)
  {
    if (settings.modeNum != 98)
    {
      effectsGroup = mainModes[settings.modeNum].effectsGroup;
      submode = mainModes[settings.modeNum].subMode;
    }
    else
    {
      currentIndex = 255;
      allModesEnable = 1;
      effectsGroup = 255;
      submode = 255;
    }
  }

  delay(1500);

  startingMillis = millis();
}

void loop()
{
  static uint32_t lastMillis = 0;
  uint32_t currentMillis = millis();

  if (currentMillis - lastMillis >= 100)
  {
    lastMillis = currentMillis;
    server.handleClient();

    if (settings.isWifiAutoOffEnable)
    {
      if (currentMillis - startingMillis >= 300000)
      {
        WiFi.softAPdisconnect(true);
      }
    }
  }

  if (allModesEnable == 1)
  {
    effectAllModes();
  }

  if (effectsGroup < 14)
  {
    modeFunctions[effectsGroup](submode);
  }
}