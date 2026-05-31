#include "settings.h"
#include "effects.h"

namespace {
    void successSave();
}

namespace Settings {
    const char *SETTINGS_FILE = "/appSettings.bin";

    bool loadSettings() {
        if (!LittleFS.begin()) {
            return false;
        }

        File settingsFile = LittleFS.open(SETTINGS_FILE, "r");
        if (!settingsFile) {
            Settings::saveSettings();
            return true;
        }

        if (const size_t sz = settingsFile.size(); sz != sizeof(Settings::AppSettings)) {
            settingsFile.close();
            Settings::settings = Settings::AppSettings{};
            Settings::saveSettings();
            return true;
        }

        const size_t read =
                settingsFile.read(reinterpret_cast<uint8_t *>(&Settings::settings), sizeof(Settings::AppSettings));
        settingsFile.close();

        if (read != sizeof(Settings::AppSettings)) {
            Settings::saveSettings();
            return false;
        }

        return true;
    }

    void saveSettings() {
        File settingsFile = LittleFS.open(SETTINGS_FILE, "w");
        if (!settingsFile) {
            return;
        }

        const size_t written = settingsFile.write(reinterpret_cast<const uint8_t *>(&Settings::settings),
                                                  sizeof(Settings::AppSettings));
        settingsFile.close();

        if (written == sizeof(Settings::AppSettings)) {
            successSave();
        }
    }

    void resetSettings() { (void) LittleFS.remove(SETTINGS_FILE); }
} // namespace Settings

namespace {
    void successSave() {
        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CHSV(96U, 255U, 80U));
        StripControl::show();
        delay(2000U);
        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
        StripControl::show();
    }
} // namespace
