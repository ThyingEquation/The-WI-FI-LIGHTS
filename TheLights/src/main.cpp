#include "lightsServer.h"
#include "lightsSettings.h"
#include "effects.h"
#include "colors.h"

appSettings_s settings;
deviceEffectsState_s deviceEffectsState;

CRGB leds[MATRIX_LEDS];

// void (*modeFunctions[14])(uint8_t) = {
//     drawCanvasEffects, drawRunningLine,
//     drawColorfulWaves, drawAnimations,};

void (*modeFunctions[14])(uint8_t) = { // порядок группы эффекта в этом массиве = коду группы эффекта
    drawColorfulEffects, drawRunningLights, drawJumpingLights, drawFlickeringLights,
  drawWaterEffects, drawWeatherEffects, drawGamesEffects, drawSpaceEffects};

static uint32_t startingMillis = 0;

void setup()
{
  Serial.begin(9600);

  if (!loadSettings())
  {
    return;
  }

  delay(100);

  FastLED.addLeds<WS2812B, PIN, GRB>(leds, MATRIX_LEDS)
      .setCorrection(TypicalSMD5050)
      .setDither(settings.globalBrightness <= 255);
  FastLED.setBrightness(settings.globalBrightness);

  delay(10);

  initRunningLine();

  initLightServer();

  if (settings.startingEffectsGroup != 255)
  {
    if (settings.startingEffectsGroup != 250) // все режимы подряд
    {
      deviceEffectsState.effectsGroup = settings.startingEffectsGroup;
      deviceEffectsState.effectSubmode = settings.startingEffectSubmode;
    }
    else
    {
      deviceEffectsState.currentIndex = 255;
      deviceEffectsState.isAllModesEnable = true;
      deviceEffectsState.effectsGroup = 255;
      deviceEffectsState.effectSubmode = 255;
    }
  }

  delay(2000);

  Serial.println(deviceEffectsState.effectsGroup);
  Serial.println(deviceEffectsState.effectSubmode);

  fill_solid(leds, MATRIX_LEDS, CRGB::Black);
  FastLED.show();

  startingMillis = millis();
}

void loop()
{
  static uint32_t lastMillis = 0;
  uint32_t currentMillis = millis();

  checkLightServer();

  if (deviceEffectsState.isWifiActive && currentMillis - lastMillis >= 1000)
  {
    lastMillis = currentMillis;

    if (settings.isWifiAutoOffEnable)
    {
      if (currentMillis - startingMillis >= 180000)
      {
        WiFi.softAPdisconnect(true);
        deviceEffectsState.isWifiActive = false;
      }
    }
  }

  if (deviceEffectsState.isAllModesEnable)
  {
    effectAllModes();
  }

  if (deviceEffectsState.effectsGroup < 14)
  {
    modeFunctions[deviceEffectsState.effectsGroup](deviceEffectsState.effectSubmode);
  }

  if (deviceEffectsState.isScreenClearEnable)
  {
    deviceEffectsState.isScreenClearEnable = false;
    FastLED.clear();
    FastLED.show();
  }
}