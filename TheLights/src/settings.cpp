#include "settings.h"
#include "effects.h"

namespace {
    void successSave();
}

namespace Settings {
    const char *SETTINGS_FILE = "/appSettings.bin";

    bool load() {
        if (!LittleFS.begin()) {
            return false;
        }

        File settingsFile = LittleFS.open(SETTINGS_FILE, "r");
        if (!settingsFile) {
            Settings::save();
            return true;
        }

        if (const size_t sz = settingsFile.size(); sz != sizeof(Settings::SettingsParameters)) {
            settingsFile.close();
            Settings::parameters = Settings::SettingsParameters{};
            Settings::save();
            return true;
        }

        const size_t read =
                settingsFile.read(reinterpret_cast<uint8_t *>(&Settings::parameters), sizeof(Settings::SettingsParameters));
        settingsFile.close();

        if (read != sizeof(Settings::SettingsParameters)) {
            Settings::save();
            return false;
        }

        return true;
    }

    void save() {
        File settingsFile = LittleFS.open(SETTINGS_FILE, "w");
        if (!settingsFile) {
            return;
        }

        const size_t written = settingsFile.write(reinterpret_cast<const uint8_t *>(&Settings::parameters),
                                                  sizeof(Settings::SettingsParameters));
        settingsFile.close();

        if (written == sizeof(Settings::SettingsParameters)) {
            successSave();
        }
    }

    void reset() { (void) LittleFS.remove(SETTINGS_FILE); }
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
