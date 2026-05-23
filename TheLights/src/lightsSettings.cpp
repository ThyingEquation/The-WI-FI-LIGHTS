#include "lightsSettings.h"
#include "effects.h"

static void successSave();

const char *SETTINGS_FILE = "/appSettings.bin";

bool loadSettings() {
    if (!LittleFS.begin()) {
        return false;
    }

    File settingsFile = LittleFS.open(SETTINGS_FILE, "r");
    if (!settingsFile) {
        saveSettings();
        return true;
    }

    if (const size_t sz = settingsFile.size(); sz != sizeof(AppSettings)) {
        settingsFile.close();
        settings = AppSettings{};
        saveSettings();
        return true;
    }

    const size_t read = settingsFile.read(reinterpret_cast<uint8_t *>(&settings), sizeof(AppSettings));
    settingsFile.close();

    if (read != sizeof(AppSettings)) {
        saveSettings();
        return false;
    }

    return true;
}

void saveSettings() {
    File settingsFile = LittleFS.open(SETTINGS_FILE, "w");
    if (!settingsFile) {
        return;
    }

    const size_t written = settingsFile.write(reinterpret_cast<const uint8_t *>(&settings), sizeof(AppSettings));
    settingsFile.close();

    if (written == sizeof(AppSettings)) {
        successSave();
    }
}

void resetSettings() { LittleFS.remove(SETTINGS_FILE); }

static void successSave() {
    fill_solid(leds, MATRIX_LEDS, CHSV(96, 255, 80));
    stripShow();
    delay(2000);
    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    stripShow();
}
