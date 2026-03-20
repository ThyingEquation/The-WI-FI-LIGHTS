#include "lightsServer.h"
#include "lightsSettings.h"
#include "effects.h"
#include "colors.h"

appSettings_s settings;
deviceEffectsState_s deviceEffectsState;

CRGB leds_plus_safety_pixel[MATRIX_LEDS + 1];
CRGB *const leds(leds_plus_safety_pixel + 1);

void (*modeFunctions[14])(uint8_t) = {
    drawCanvasEffects, drawRunningLine, drawColorfulSpots, drawRainbows, drawRunningLights,
    drawJumpingLights, drawSpaceEffects, drawFlashLights, drawWaterEffects, drawWeatherEffects,
    drawColorfulWaves, drawAnimations, drawGamesEffects, drawMatrixMovieEffect};

static uint32_t startingMillis = 0;

void setup()
{
  Serial.begin(9600);

  if (!loadSettings())
  {
    return;
  }

  initRunningLine();

  delay(100);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, MATRIX_LEDS)
      .setCorrection(TypicalSMD5050)
      .setDither(settings.globalBrightness <= 255);
  FastLED.setBrightness(settings.globalBrightness);

  delay(10);

  FastLED.clear(true);
  
  delay(100);

  initLightServer();

  if (settings.startingEffectsGroup != 99)
  {
    if (settings.startingEffectsGroup != 98)
    {
      deviceEffectsState.effectsGroup = mainModes[settings.startingEffectsGroup].effectsGroup;
      deviceEffectsState.effectSubmode = mainModes[settings.startingEffectSubMode].subMode;
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

  startingMillis = millis();
}

void loop()
{
  static uint32_t lastMillis = 0;
  uint32_t currentMillis = millis();

  if (currentMillis - lastMillis >= 100)
  {
    lastMillis = currentMillis;
    checkLightServer();

    if (settings.isWifiAutoOffEnable)
    {
      if (currentMillis - startingMillis >= 180000)
      {
        WiFi.softAPdisconnect(true);
      }
    }
  }

  if (deviceEffectsState.isScreenClearEnable)
  {
    deviceEffectsState.isScreenClearEnable = false;
    FastLED.clear(true);
  }

  if (deviceEffectsState.isAllModesEnable)
  {
    effectAllModes();
    yield();
  }

  if (deviceEffectsState.effectsGroup < 14)
  {
    modeFunctions[deviceEffectsState.effectsGroup](deviceEffectsState.effectSubmode);
    yield();
  }
}