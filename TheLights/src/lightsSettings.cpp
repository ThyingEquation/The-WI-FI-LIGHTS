#include "lightsSettings.h"
#include "effects.h"

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
        settings = appSettings_s{};
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
        return;
    }

    size_t written = settingsFile.write((const uint8_t *)&settings, sizeof(appSettings_s));
    settingsFile.close();
    
    if (written == sizeof(appSettings_s))
    {
        successSave();
    }
}

void resetSettings()
{
    LittleFS.remove(SETTINGS_FILE);
}

static void successSave()
{
    fill_solid(leds, MATRIX_LEDS, CHSV(96, 255, 80));
    stripShow();
    delay(2000);
    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    stripShow();
}