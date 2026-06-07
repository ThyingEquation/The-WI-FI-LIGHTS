#include <deque>

#include "commandsHandler.h"
#include "effects.h"
#include "localWifiServer.h"
#include "settings.h"

namespace Settings {
    SettingsParameters parameters;
}

namespace StripControl {
    CRGB leds[MATRIX_LEDS];
    NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip(StripControl::MATRIX_LEDS); // GPIO2
} // namespace StripControl

namespace {
    void initEffectAllModes(uint32_t empty);
    void checkWifiAutoOff();
    void effectAllModes();

    void (*modeFunctions[])(uint32_t) = { // порядок группы эффекта в этом массиве = коду группы эффекта
            &Effects::drawColorfulEffects,  &Effects::drawRunningLights, &Effects::drawJumpingLights,
            &Effects::drawFlickeringLights, &Effects::drawWaterEffects,  &Effects::drawWeatherEffects,
            &Effects::drawGamesEffects,     &Effects::drawSpaceEffects,  &Effects::drawCanvasEffects,
            &Effects::drawRunningLine,      &Effects::drawAnimations,    &initEffectAllModes};
    constexpr uint8_t GROUP_COUNT = std::size(modeFunctions);

    enum class EffectsGroups : uint8_t { // Главные группы эффектов. Порядок эффектов аналогичен андроид приложению
        COLORFUL_EFFECTS, // (0) - Цветные эффекты
        RUNNING_LIGHTS, // (1) - Бегущие огни
        JUMPING_LIGHTS, // (2) - Прыгающие огоньки
        FLICKERING_LIGHTS, // (3) - Мерцающие огни
        WATER_EFFECTS, // (4) - Водные эффекты
        WEATHER_EFFECTS, // (5) - Погодные эффекты
        GAMES_EFFECTS, // (6) - Эффекты в виде игр
        SPACE_EFFECTS, // (7) - Космос
        CANVAS_EFFECTS, // (8) - Эффекты режима рисования
        RUNNING_LINE, // (9) - Бегущая строка (текст)
        ANIMATIONS, // (10) - Анимации
    };

    struct ModeConfig {
        uint8_t effectsGroup;
        uint8_t subMode;
    };

    constexpr ModeConfig mainModesData[] = {
            {static_cast<uint8_t>(EffectsGroups::COLORFUL_EFFECTS), 0U}, // "Цветные пятна"
            {static_cast<uint8_t>(EffectsGroups::COLORFUL_EFFECTS), 1U}, // "Световой шум"
            {static_cast<uint8_t>(EffectsGroups::COLORFUL_EFFECTS), 2U}, // "Диагональные волны"
            {static_cast<uint8_t>(EffectsGroups::COLORFUL_EFFECTS), 3U}, // "Крутящаяся радуга"
            {static_cast<uint8_t>(EffectsGroups::COLORFUL_EFFECTS), 4U}, // "Радужная рябь"
            {static_cast<uint8_t>(EffectsGroups::COLORFUL_EFFECTS), 5U}, // "Радуга волной"
            {static_cast<uint8_t>(EffectsGroups::COLORFUL_EFFECTS), 6U}, // "Радуга змейкой"

            {static_cast<uint8_t>(EffectsGroups::RUNNING_LIGHTS), 0U}, // "Медленный огонек"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LIGHTS), 1U}, // "Быстрый огонек"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LIGHTS), 2U}, // "Цветной огонек"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LIGHTS), 3U}, // "Цветная змейка"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LIGHTS), 4U}, // "Бегущие огоньки #1"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LIGHTS), 5U}, // "Бегущие огоньки #2"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LIGHTS), 6U}, // "Бегущие огоньки #3"

            {static_cast<uint8_t>(EffectsGroups::JUMPING_LIGHTS), 0U}, // "Хаос"
            {static_cast<uint8_t>(EffectsGroups::JUMPING_LIGHTS), 1U}, // "Дрейфующая линия"
            {static_cast<uint8_t>(EffectsGroups::JUMPING_LIGHTS), 2U}, // "Прыгающие круги"
            {static_cast<uint8_t>(EffectsGroups::JUMPING_LIGHTS), 3U}, // "Прыгающий квадрат"
            {static_cast<uint8_t>(EffectsGroups::JUMPING_LIGHTS), 4U}, // "Прыгающие точки"

            {static_cast<uint8_t>(EffectsGroups::FLICKERING_LIGHTS), 0U}, // "Летящие огни"
            {static_cast<uint8_t>(EffectsGroups::FLICKERING_LIGHTS), 1U}, // "Конфетти"
            {static_cast<uint8_t>(EffectsGroups::FLICKERING_LIGHTS), 2U}, // "Мерцающие огни"

            {static_cast<uint8_t>(EffectsGroups::WATER_EFFECTS), 0U}, // "Лагуна"
            {static_cast<uint8_t>(EffectsGroups::WATER_EFFECTS), 1U}, // "Бассейн"

            {static_cast<uint8_t>(EffectsGroups::WEATHER_EFFECTS), 0U}, // "Снегопад"
            {static_cast<uint8_t>(EffectsGroups::WEATHER_EFFECTS), 1U}, // "Метель"
            {static_cast<uint8_t>(EffectsGroups::WEATHER_EFFECTS), 2U}, // "Дождь"
            {static_cast<uint8_t>(EffectsGroups::WEATHER_EFFECTS), 3U}, // "Ливень"

            {static_cast<uint8_t>(EffectsGroups::GAMES_EFFECTS), 0U}, // "Игра змейка"
            {static_cast<uint8_t>(EffectsGroups::GAMES_EFFECTS), 1U}, // "Тетрис"
            {static_cast<uint8_t>(EffectsGroups::GAMES_EFFECTS), 2U}, // "Арканоид"
            {static_cast<uint8_t>(EffectsGroups::GAMES_EFFECTS), 3U}, // "Космические корабли"
            {static_cast<uint8_t>(EffectsGroups::GAMES_EFFECTS), 4U}, // "Эффект из к/ф матрица"

            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 0U}, // "Звездное небо"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 1U}, // "Созвездия"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 2U}, // "Пульсирующая звезда"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 3U}, // "Затменные звезды"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 4U}, // "Метеоритный дождь"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 5U}, // "Спиральная туманность"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 6U}, // "Спиральная галактика"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 7U}, // "Фазы луны"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 8U}, // "Юпитер"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 9U}, // "Черная дыра"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 10U}, // "Северное сияние"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 11U}, // "Магнитные волны"
            {static_cast<uint8_t>(EffectsGroups::SPACE_EFFECTS), 12U}, // "Интерференция лучей"

            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 0U}, // "Полная заливка"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 1U}, // "Зеркальная заливка"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 2U}, // "Заливка линиями"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 3U}, // "Попиксельная линиями"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 4U}, // "Быстрая заливка змейкой"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 5U}, // "Медленная заливка змейкой"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 6U}, // "Цветная заливка змейкой"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 7U}, // "Цветное дыхание"

            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 254U}, // "Все картинки подряд"
            {static_cast<uint8_t>(EffectsGroups::CANVAS_EFFECTS), 255U}, // "Все картинки беспорядочно"

            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 0U}, // "С наступающим Новым годом"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 1U}, // "С Новым годом"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 2U}, // "Happy New Year"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 3U}, // "С Рождеством"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 4U}, // "Merry Christmas"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 5U}, // "Здесь могла быть ваша реклама"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 6U}, // "Привет, я умная гирлянда"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 7U}, // "ВВЕДИТЕ ТЕКСТ"
            {static_cast<uint8_t>(EffectsGroups::RUNNING_LINE), 8U}, // "Поэма 'Медный всадник'"

            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 0U}, // "Сердце"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 1U}, // Смайлик"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 2U}, // "Прыгающий человечек"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 3U}, // "Файербол"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 4U}, // "Взрыв"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 5U}, // "'С НОВЫМ ГОДОМ' на японском"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 6U}, // "Приветствие на корейском"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 7U}, // "Цифровой сигнал"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 8U}, // "Синусоида"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 9U}, // "Цветные синусоиды"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 10U}, // "Цветные линии #1"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 11U}, // "Цветные линии #2"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 12U}, // "Цветные линии #3"
            {static_cast<uint8_t>(EffectsGroups::ANIMATIONS), 13U}, // "Цветные линии #4"
    };
    constexpr auto ALL_MODES_EFFECT_COUNT = std::size(mainModesData);

    uint32_t startingMillis = millis();

    void initEffectAllModes(uint32_t empty) {
        Effects::state.currentIndex = Effects::EFFECT_DISABLED;
        Effects::state.isAllModesEnable = true;
        Effects::state.effectsGroup = Effects::EFFECT_DISABLED;
        Effects::state.effectSubmode = Effects::EFFECT_DISABLED;
    }
} // namespace

void setup() {
    constexpr uint32_t configPin = 4U; // GPIO4
    pinMode(static_cast<uint8_t>(configPin), static_cast<uint8_t>(INPUT_PULLUP));

    if (!Settings::load()) {
        return;
    }

    StripControl::strip.Begin();
    fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
    StripControl::strip.Show();

    if (const auto buttonState = static_cast<uint32_t>(digitalRead(static_cast<uint8_t>(configPin)));
        buttonState == 1U) {
        Effects::initRunningLine();
        if (Settings::parameters.workMode == static_cast<uint32_t>(Settings::AnimationsDelays::ANDROID_AP)) {
            LocalWifiServer::initAccessPointServer();
        } else if (Settings::parameters.workMode == static_cast<uint32_t>(Settings::AnimationsDelays::ANDROID_STA)) {
            LocalWifiServer::initStationServer();
        } else {
            WebControlServer::init();
        }
    } else {
        SettingsServer::checkServer();
    }

    if (Settings::parameters.startingEffectsGroup != Effects::EFFECT_DISABLED) // Сохранен стартовый режим
    {
        Effects::state.effectsGroup = Settings::parameters.startingEffectsGroup;
        Effects::state.effectSubmode = Settings::parameters.startingEffectSubmode;
        Effects::state.currentIndex = Effects::EFFECT_DISABLED;
    }

    // Serial.begin(9600U);
    // (void)Serial.println(state.effectsGroup);
    // (void)Serial.println(state.effectSubmode);

    delay(1500U);
}

void loop() {

    if (Settings::parameters.workMode == static_cast<uint32_t>(Settings::AnimationsDelays::ANDROID_AP) ||
        Settings::parameters.workMode == static_cast<uint32_t>(Settings::AnimationsDelays::ANDROID_STA)) {
        LocalWifiServer::checkServer();
    } else {
        WebControlServer::checkServer();
    }

    if (Effects::state.isAllModesEnable) {
        effectAllModes();
    }

    if (Effects::state.isScreenClearEnable) {
        Effects::state.isScreenClearEnable = false;
        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
        StripControl::show();
        return;
    }

    if (static_cast<uint32_t>(Effects::state.effectsGroup) < GROUP_COUNT) {
        modeFunctions[Effects::state.effectsGroup](Effects::state.effectSubmode);
    }

    checkWifiAutoOff();
}

namespace {
    void checkWifiAutoOff() {
        static uint32_t lastMillis = 0U;

        if (const uint32_t currentMillis = millis();
            Effects::state.isWifiActive && currentMillis - lastMillis >= 1000U) {
            lastMillis = currentMillis;
            if (Settings::parameters.isWifiAutoOffEnable) {
                if (currentMillis - startingMillis >= 180000U) {
                    (void) WiFi.softAPdisconnect(true);
                    Effects::state.isWifiActive = false;
                }
            }
        }
    }

    void effectAllModes() {
        static std::deque<uint32_t> usedEffects;
        static uint32_t prevTime = 0U;
        static uint32_t randomCounter = 0U;
        static uint32_t allModeDelayLocal = Settings::parameters.allModeDelay;
        static uint32_t allModesWorkTypeLocal = Settings::parameters.allModesWorkType;

        if (const uint32_t currentTime = millis();
            Effects::state.currentIndex == Effects::EFFECT_DISABLED || currentTime - prevTime >= allModeDelayLocal) {

            CommandsHandler::sendVirtualCommand();

            if (Effects::state.currentIndex == Effects::EFFECT_DISABLED) {
                Effects::state.currentIndex = 0U;
            }

            prevTime = currentTime;

            if (allModesWorkTypeLocal == 1U) {
                if (Effects::state.currentIndex >= ALL_MODES_EFFECT_COUNT) {
                    Effects::state.currentIndex = 0U;
                }

                Effects::state.effectsGroup = mainModesData[Effects::state.currentIndex].effectsGroup;
                Effects::state.effectSubmode = mainModesData[Effects::state.currentIndex].subMode;

                Effects::state.currentIndex += 1U;
            } else {
                randomCounter = 0U;
                do {
                    Effects::state.currentIndex = static_cast<uint8_t>(
                            ESP8266TrueRandom.random(0, static_cast<int32_t>(ALL_MODES_EFFECT_COUNT)));
                    randomCounter++;
                    /// ДОБАВИТЬ ПРОПОРЦИОНАЛЬНОСТЬ!!!!!
                    if (randomCounter > 60U) {
                        break;
                    }
                } while (std::find(usedEffects.begin(), usedEffects.end(), Effects::state.currentIndex) !=
                         usedEffects.end());

                usedEffects.push_back(Effects::state.currentIndex);
                if (usedEffects.size() > 55U) // Размер истории используемых эффектов
                {
                    usedEffects.pop_front();
                }

                Effects::state.effectsGroup = mainModesData[Effects::state.currentIndex].effectsGroup;
                Effects::state.effectSubmode = mainModesData[Effects::state.currentIndex].subMode;
            }
            Effects::state.isScreenClearEnable = true;
        }
    }
} // namespace

namespace StripControl {
    void show() {
        const uint8_t globalBr = Settings::parameters.globalBrightness;
        for (uint16_t i = 0U; i < StripControl::MATRIX_LEDS; i++) {
            CRGB c = StripControl::leds[i];
            if (static_cast<uint32_t>(globalBr) < Effects::EFFECT_DISABLED) {
                (void) c.nscale8_video(globalBr);
            }
            strip.SetPixelColor(i, RgbColor(c.r, c.g, c.b));
        }
        strip.Show();
    }
} // namespace StripControl
