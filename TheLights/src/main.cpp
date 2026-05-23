#include <deque>

#include "effects.h"
#include "lightsServer.h"
#include "lightsSettings.h"

AppSettings settings;
DeviceEffectsState deviceEffectsState;

CRGB leds[MATRIX_LEDS];
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip(MATRIX_LEDS);

static void initEffectAllModes(uint8_t empty);
static void effectAllModes();

void (*modeFunctions[])(uint8_t) = { // порядок группы эффекта в этом массиве = коду группы эффекта
        drawColorfulEffects, drawRunningLights,  drawJumpingLights, drawFlickeringLights,
        drawWaterEffects,    drawWeatherEffects, drawGamesEffects,  drawSpaceEffects,
        drawCanvasEffects,   drawRunningLine,    drawAnimations,    initEffectAllModes};
static constexpr uint8_t GROUP_COUNT = std::size(modeFunctions);

enum EffectsGroupMode // Главные группы эффектов. Порядок эффектов аналогичен андроид приложению
{
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
        {COLORFUL_EFFECTS, 0}, // "Цветные пятна"
        {COLORFUL_EFFECTS, 1}, // "Световой шум"
        {COLORFUL_EFFECTS, 2}, // "Диагональные волны"
        {COLORFUL_EFFECTS, 3}, // "Крутящаяся радуга"
        {COLORFUL_EFFECTS, 4}, // "Радужная рябь"
        {COLORFUL_EFFECTS, 5}, // "Радуга волной"
        {COLORFUL_EFFECTS, 6}, // "Радуга змейкой"

        {RUNNING_LIGHTS, 0}, // "Медленный огонек"
        {RUNNING_LIGHTS, 1}, // "Быстрый огонек"
        {RUNNING_LIGHTS, 2}, // "Цветной огонек"
        {RUNNING_LIGHTS, 3}, // "Цветная змейка"
        {RUNNING_LIGHTS, 4}, // "Бегущие огоньки #1"
        {RUNNING_LIGHTS, 5}, // "Бегущие огоньки #2"
        {RUNNING_LIGHTS, 6}, // "Бегущие огоньки #3"

        {JUMPING_LIGHTS, 0}, // "Хаос"
        {JUMPING_LIGHTS, 1}, // "Дрейфующая линия"
        {JUMPING_LIGHTS, 2}, // "Прыгающие круги"
        {JUMPING_LIGHTS, 3}, // "Прыгающий квадрат"
        {JUMPING_LIGHTS, 4}, // "Прыгающие точки"

        {FLICKERING_LIGHTS, 0}, // "Летящие огни"
        {FLICKERING_LIGHTS, 1}, // "Конфетти"
        {FLICKERING_LIGHTS, 2}, // "Мерцающие огни"

        {WATER_EFFECTS, 0}, // "Лагуна"
        {WATER_EFFECTS, 1}, // "Бассейн"

        {WEATHER_EFFECTS, 0}, // "Снегопад"
        {WEATHER_EFFECTS, 1}, // "Метель"
        {WEATHER_EFFECTS, 2}, // "Дождь"
        {WEATHER_EFFECTS, 3}, // "Ливень"

        {GAMES_EFFECTS, 0}, // "Игра змейка"
        {GAMES_EFFECTS, 1}, // "Тетрис"
        {GAMES_EFFECTS, 2}, // "Арканоид"
        {GAMES_EFFECTS, 3}, // "Космические корабли"
        {GAMES_EFFECTS, 4}, // "Эффект из к/ф матрица"

        {SPACE_EFFECTS, 0}, // "Звездное небо"
        {SPACE_EFFECTS, 1}, // "Созвездия"
        {SPACE_EFFECTS, 2}, // "Пульсирующая звезда"
        {SPACE_EFFECTS, 3}, // "Затменные звезды"
        {SPACE_EFFECTS, 4}, // "Метеоритный дождь"
        {SPACE_EFFECTS, 5}, // "Спиральная туманность"
        {SPACE_EFFECTS, 6}, // "Спиральная галактика"
        {SPACE_EFFECTS, 7}, // "Фазы луны"
        {SPACE_EFFECTS, 8}, // "Юпитер"
        {SPACE_EFFECTS, 9}, // "Черная дыра"
        {SPACE_EFFECTS, 10}, // "Северное сияние"
        {SPACE_EFFECTS, 11}, // "Магнитные волны"
        {SPACE_EFFECTS, 12}, // "Интерференция лучей"

        {CANVAS_EFFECTS, 0}, // "Полная заливка"
        {CANVAS_EFFECTS, 1}, // "Зеркальная заливка"
        {CANVAS_EFFECTS, 2}, // "Заливка линиями"
        {CANVAS_EFFECTS, 3}, // "Быстрая заливка змейкой"
        {CANVAS_EFFECTS, 4}, // "Медленная заливка змейкой"
        {CANVAS_EFFECTS, 5}, // "Цветная заливка змейкой"
        {CANVAS_EFFECTS, 6}, // "Цветное дыхание"

        {CANVAS_EFFECTS, 254}, // "Все картинки подряд"
        {CANVAS_EFFECTS, 255}, // "Все картинки беспорядочно"

        {RUNNING_LINE, 0}, // "С наступающим Новым годом"
        {RUNNING_LINE, 1}, // "С Новым годом"
        {RUNNING_LINE, 2}, // "Happy New Year"
        {RUNNING_LINE, 3}, // "С Рождеством"
        {RUNNING_LINE, 4}, // "Merry Christmas"
        {RUNNING_LINE, 5}, // "Здесь могла быть ваша реклама"
        {RUNNING_LINE, 6}, // "Привет, я умная гирлянда"
        {RUNNING_LINE, 7}, // "ВВЕДИТЕ ТЕКСТ"
        {RUNNING_LINE, 8}, // "Поэма 'Медный всадник'"

        {ANIMATIONS, 0}, // "Сердце"
        {ANIMATIONS, 1}, // Смайлик"
        {ANIMATIONS, 2}, // "Прыгающий человечек"
        {ANIMATIONS, 3}, // "Файербол"
        {ANIMATIONS, 4}, // "Взрыв"
        {ANIMATIONS, 5}, // "'С НОВЫМ ГОДОМ' на японском"
        {ANIMATIONS, 6}, // "Приветствие на корейском"
        {ANIMATIONS, 7}, // "Цифровой сигнал"
        {ANIMATIONS, 8}, // "Синусоида"
        {ANIMATIONS, 9}, // "Цветные синусоиды"
        {ANIMATIONS, 10}, // "Цветные линии #1"
        {ANIMATIONS, 11}, // "Цветные линии #2"
        {ANIMATIONS, 12}, // "Цветные линии #3"
        {ANIMATIONS, 13}, // "Цветные линии #4"
};
constexpr auto ALL_MODES_EFFECT_COUNT = std::size(mainModesData);

static uint32_t startingMillis = millis();

void stripShow() {
    const uint8_t globalBr = settings.globalBrightness;
    for (uint16_t i = 0; i < MATRIX_LEDS; i++) {
        CRGB c = leds[i];
        if (globalBr < 255)
            c.nscale8_video(globalBr);
        strip.SetPixelColor(i, RgbColor(c.r, c.g, c.b));
    }
    strip.Show();
}

static void initEffectAllModes(uint8_t empty) {
    deviceEffectsState.currentIndex = 255;
    deviceEffectsState.isAllModesEnable = true;
    deviceEffectsState.effectsGroup = 255;
    deviceEffectsState.effectSubmode = 255;
}

void setup() {
    if (!loadSettings()) {
        return;
    }

    initRunningLine();
    initLightServer();

    if (settings.startingEffectsGroup != 255) // Сохранен стартовый режим
    {
        deviceEffectsState.effectsGroup = settings.startingEffectsGroup;
        deviceEffectsState.effectSubmode = settings.startingEffectSubmode;
        deviceEffectsState.currentIndex = 255;
    }

    // Serial.begin(9600);
    // Serial.println(deviceEffectsState.effectsGroup);
    // Serial.println(deviceEffectsState.effectSubmode);

    strip.Begin();
    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    strip.Show();

    delay(100);
}

void loop() {
    static uint32_t lastMillis = 0;
    const uint32_t currentMillis = millis();

    checkLightServer();

    if (deviceEffectsState.isAllModesEnable) {
        effectAllModes();
    }

    if (deviceEffectsState.isScreenClearEnable) {
        deviceEffectsState.isScreenClearEnable = false;
        fill_solid(leds, MATRIX_LEDS, CRGB::Black);
        stripShow();
        return;
    }

    if (deviceEffectsState.effectsGroup < GROUP_COUNT) {
        modeFunctions[deviceEffectsState.effectsGroup](deviceEffectsState.effectSubmode);
    }

    if (deviceEffectsState.isWifiActive && currentMillis - lastMillis >= 1000) {
        lastMillis = currentMillis;
        if (settings.isWifiAutoOffEnable) {
            if (currentMillis - startingMillis >= 180000) {
                WiFi.softAPdisconnect(true);
                deviceEffectsState.isWifiActive = false;
            }
        }
    }
}

static void effectAllModes() {
    static std::deque<uint8_t> usedEffects;
    static uint32_t prevTime = 0;
    static uint8_t randomCounter = 0;
    static uint32_t allModeDelayLocal = settings.allModeDelay;
    static uint8_t allModesWorkTypeLocal = settings.allModesWorkType;

    if (const uint32_t currentTime = millis();
        deviceEffectsState.currentIndex == 255 || currentTime - prevTime >= allModeDelayLocal) {

        sendVirtualCommand();

        if (deviceEffectsState.currentIndex == 255) {
            deviceEffectsState.currentIndex = 0;
        }

        prevTime = currentTime;

        if (allModesWorkTypeLocal == 1) {
            if (deviceEffectsState.currentIndex >= ALL_MODES_EFFECT_COUNT) {
                deviceEffectsState.currentIndex = 0;
            }

            deviceEffectsState.effectsGroup = mainModesData[deviceEffectsState.currentIndex].effectsGroup;
            deviceEffectsState.effectSubmode = mainModesData[deviceEffectsState.currentIndex].subMode;

            deviceEffectsState.currentIndex += 1;
        } else {
            randomCounter = 0;
            do {
                deviceEffectsState.currentIndex = ESP8266TrueRandom.random(0, ALL_MODES_EFFECT_COUNT);
                randomCounter++;
                /// ДОБАВИТЬ ПРОПОРЦИОНАЛЬНОСТЬ!!!!!
                if (randomCounter > 30)
                    break;
            } while (std::find(usedEffects.begin(), usedEffects.end(), deviceEffectsState.currentIndex) !=
                     usedEffects.end());

            usedEffects.push_back(deviceEffectsState.currentIndex);
            if (usedEffects.size() > 40) // Размер истории используемых эффектов
            {
                usedEffects.pop_front();
            }

            deviceEffectsState.effectsGroup = mainModesData[deviceEffectsState.currentIndex].effectsGroup;
            deviceEffectsState.effectSubmode = mainModesData[deviceEffectsState.currentIndex].subMode;
        }
        deviceEffectsState.isScreenClearEnable = true;
    }
}
