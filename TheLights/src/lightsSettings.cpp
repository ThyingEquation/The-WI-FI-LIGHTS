#include "lightsSettings.h"
#include "main.h"

static void successSave();

const char *SETTINGS_FILE = "/appSettings.bin";

bool loadSettings()
{

    if (!LittleFS.begin())
    {
        return false;
    }

    File settingsFile = LittleFS.open(SETTINGS_FILE, "r");
    if (!settingsFile)
    {
        saveSettings();
        return true;
    }

    size_t sz = settingsFile.size();
    if (sz != sizeof(appSettings_s))
    {
        settingsFile.close();
        saveSettings();
        return true;
    }

    size_t read = settingsFile.read((uint8_t *)&settings, sizeof(appSettings_s));
    settingsFile.close();

    if (read != sizeof(appSettings_s))
    {
        saveSettings();
        return false;
    }

    return true;
}

void saveSettings()
{
    File settingsFile = LittleFS.open(SETTINGS_FILE, "w");
    if (!settingsFile)
    {
        Serial.println("Ошибка. Файл настроек невозможно открыть. Возможно не загружена файловая система в ESP");
        return;
    }

    settingsFile.write((const uint8_t *)&settings, sizeof(appSettings_s));
    settingsFile.close();

    successSave();
}

void resetSettings()
{

    File settingsFile = LittleFS.open(SETTINGS_FILE, "r");
    if (settingsFile)
    {
        settingsFile.close();
    }

    LittleFS.remove(SETTINGS_FILE);
}

static void successSave()
{
    FastLED.clear(true);

    CRGB saveColor = CHSV(96, 255, 80);

    fill_solid(leds, MATRIX_LEDS, saveColor);
    FastLED.show();

    delay(500);

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    FastLED.show();
}