#include <deque>

#include "colors.h"
#include "effects.h"
#include "images.h"

/*
  Эта группа эффектов только для матриц 12х12

  Настраиваемые параметры (DrawDelays):
    1) Скорости эффектов

  Отдельно. Режим "Рисовалка" работает на матрицах размера 8x8, 12х12 и 16х16
*/

namespace {
    enum class DrawDelays : uint16_t {
        SNAKE_FAST_DELAY = 15U,
        SNAKE_SLOW_DELAY = 50U,
        FULL_FILL_DELAY = 5000U,
        CHAMELEON_SNAKE_DELAY = 70U,
        LIGHT_BREATH_DELAY = 20U,
        DRAW_IMAGES_DELAY = 5000U
    };

    constexpr uint32_t colorsCanvas[] PROGMEM = {0x000000U, // Чёрный
                                                 0xffffffU, // Белый
                                                 0x0000ffU, // Синий
                                                 0x00ff00U, // Лайм
                                                 0x00bfffU, // Морозное небо
                                                 0xff1493U, // Малиновый
                                                 0xffff00U, // Желтый
                                                 0x7fffd4U, // Аквамариновый
                                                 0x00ff7fU, // Весенне-зеленый
                                                 0xffa500U, // Оранжевый
                                                 0xff0000U, // Красный
                                                 0x9400d3U, // Фиолетово-баклажанный
                                                 0xffb6c1U, // Светло-розовый
                                                 0x4b0082U, // Индиго
                                                 0xffd700U, // Золотой
                                                 0x008000U, // Зеленый
                                                 0x22262cU, // Серый
                                                 0x964b00U, // Коричневый
                                                 0x6600ffU, // Персидский синий
                                                 0xd76e00U, // Темно-оранжевый
                                                 0x7cfc00U}; // Зеленая лужайка

    constexpr const uint32_t *images[] = {
            Images::pacman1,  Images::pacman2,  Images::pacman3,     Images::pacman4,   Images::pacman5,
            Images::mushroom, Images::amogus,   Images::cup,         Images::pineapple, Images::alien,
            Images::hummer,   Images::cat,      Images::teaCup,      Images::dino,      Images::hammerAndSickle,
            Images::apple,    Images::bird,     Images::rabbit,      Images::question,  Images::goldenKey,
            Images::star,     Images::sun,      Images::pepe,        Images::pokeball,  Images::microsoft,
            Images::battery,  Images::redHeart, Images::thundercloud};

    void drawSnake(uint16_t delay);
    void drawFullFill();
    void drawMirrorFill();
    void drawSlicesFill();
    void drawChameleonSnake();
    void drawLightBreath();
    void drawImages(uint32_t subMode);
}

namespace Effects {
    void drawOnCanvas(const std::string_view mode, const uint32_t color, const uint32_t ledNum) {
        if (mode == "A") {
            if (ledNum == 257U) {
                const uint32_t rawColor = pgm_read_dword_near(&colorsCanvas[color]);
                fill_solid(&StripControl::leds[0], static_cast<int32_t>(StripControl::MATRIX_LEDS), CRGB(rawColor));
                StripControl::show();
            } else {
                if (const uint32_t index = ledNum; index < StripControl::MATRIX_LEDS) {
                    const uint32_t rawColor = pgm_read_dword_near(&colorsCanvas[color]);
                    StripControl::leds[index] = CRGB(rawColor);
                    StripControl::show();
                }
            }
        } else if (mode == "B") {
            if (ledNum == 257U) {
                fill_solid(&StripControl::leds[0], static_cast<int32_t>(StripControl::MATRIX_LEDS), CRGB::Black);
                StripControl::show();
            } else {
                if (const uint32_t index = ledNum; index < StripControl::MATRIX_LEDS) {
                    StripControl::leds[index] = CRGB::Black;
                    StripControl::show();
                }
            }
        }
    }

    void drawCanvasEffects(const uint32_t subMode) {
        switch (subMode) {
            case 0U:
                drawFullFill();
                break;

            case 1U:
                drawMirrorFill();
                break;

            case 2U:
                drawSlicesFill();
                break;

            case 3U:
                drawSnake(static_cast<uint16_t>(DrawDelays::SNAKE_FAST_DELAY));
                break;

            case 4U:
                drawSnake(static_cast<uint16_t>(DrawDelays::SNAKE_SLOW_DELAY));
                break;

            case 5U:
                drawChameleonSnake();
                break;

            case 6U:
                drawLightBreath();
                break;

            case 99U ... 255U:
                drawImages(subMode);
                break;

            default:
                break;
        }
    }
}

namespace {
    void drawSnake(const uint16_t delay) {
        static uint32_t color = 0U;
        static uint32_t ledsCount = 0U;
        static uint32_t previousMillis = 0U;

        if (Effects::checkCommandReceived()) {
            color = 0U;
            ledsCount = 0U;
            previousMillis = 0U;
        }

        if (millis() - previousMillis < delay) {
            return;
        }

        previousMillis = millis();

        if (ledsCount >= StripControl::MATRIX_LEDS) {
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            ledsCount = 0U;
        }

        const CRGB newColor = pgm_read_dword(&Colors::mainColors[color]);
        StripControl::leds[ledsCount] = newColor;

        StripControl::show();
        ledsCount++;
    }

    void drawFullFill() {
        static uint32_t color = 0U;
        static bool isPaused = true;
        static uint32_t currentDistance = 0U;
        static uint32_t lastUpdateTime = 0U;
        static uint32_t pauseStartTime = 0U;

        if (Effects::checkCommandReceived()) {
            color = 0U;
            isPaused = true;
            currentDistance = 0U;
            lastUpdateTime = 0U;
            pauseStartTime = 0U;
        }

        if (isPaused) {
            if (millis() - pauseStartTime >= static_cast<uint16_t>(DrawDelays::FULL_FILL_DELAY)) {
                isPaused = false;
                currentDistance = 0U;
                color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            }
            return;
        }

        if (millis() - lastUpdateTime < 65U) {
            return;
        }
        lastUpdateTime = millis();
        fill_solid(&StripControl::leds[0], static_cast<int32_t>(StripControl::MATRIX_LEDS), CRGB::Black);

        const uint32_t rawColor = pgm_read_dword_near(&Colors::mainColors[color]);

        for (int32_t x = 0; x < static_cast<int32_t>(StripControl::MATRIX_WIDTH); x++) {
            for (int32_t y = 0; y < static_cast<int32_t>(StripControl::MATRIX_HEIGHT); y++) {
                const auto dx = static_cast<uint32_t>(abs(x - 5));
                if (const auto dy = static_cast<uint32_t>(abs(y - 5)); dx <= currentDistance && dy <= currentDistance) {
                    StripControl::leds[Effects::getIndex(static_cast<uint32_t>(x), static_cast<uint32_t>(y))] =
                            CRGB(rawColor);
                }
            }
        }
        StripControl::show();

        currentDistance++;

        if (currentDistance > 6U) {
            isPaused = true;
            pauseStartTime = millis();
        }
    }

    void drawMirrorFill() {
        static uint32_t color = 0U;
        static uint32_t step = 0U;
        static bool isPaused = false;
        static bool isClearing = false;
        static uint32_t lastTime = 0U;
        static uint32_t pauseStart = 0U;

        if (Effects::checkCommandReceived()) {
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            step = 0U;
            isPaused = false;
            isClearing = false;
            lastTime = 0U;
        }

        if (isPaused) {
            if (millis() - pauseStart >= static_cast<uint16_t>(DrawDelays::FULL_FILL_DELAY)) {
                isPaused = false;
                isClearing = true;
                step = StripControl::MATRIX_HEIGHT / 2U - 1U;
            }
            return;
        }

        if (millis() - lastTime < 80U) {
            return;
        }
        lastTime = millis();

        const uint32_t rawColor = pgm_read_dword_near(&Colors::mainColors[color]);
        const CRGB fillColor = isClearing ? CRGB::Black : CRGB(rawColor);

        const uint32_t topRow = isClearing ? StripControl::MATRIX_HEIGHT / 2U - 1U - step : step;
        const uint32_t bottomRow =
                isClearing ? StripControl::MATRIX_HEIGHT / 2U + step : StripControl::MATRIX_HEIGHT - 1U - step;

        for (uint32_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
            StripControl::leds[Effects::getIndex(x, topRow)] = fillColor;
            StripControl::leds[Effects::getIndex(x, bottomRow)] = fillColor;
        }
        StripControl::show();

        if (!isClearing) {
            step++;
            if (step >= StripControl::MATRIX_HEIGHT / 2U) {
                isPaused = true;
                pauseStart = millis();
            }
        } else {
            if (step == 0U) {
                isClearing = false;
                step = 0U;
                color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            } else {
                step--;
            }
        }
    }

    void drawSlicesFill() {
        static uint32_t color = 0U;
        static uint32_t order[StripControl::MATRIX_HEIGHT] = {};
        static uint32_t filled = 0U;
        static bool isPaused = false;
        static bool isClearing = false;
        static uint32_t lastTime = 0U;
        static uint32_t pauseStart = 0U;

        if (Effects::checkCommandReceived()) {
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            filled = 0U;
            isPaused = false;
            isClearing = false;
            lastTime = 0U;

            for (uint32_t i = 0U; i < StripControl::MATRIX_HEIGHT; i++) {
                order[i] = i;
            }
            for (uint32_t i = StripControl::MATRIX_HEIGHT - 1U; i > 0U; i--) {
                const uint8_t j = random8(static_cast<uint8_t>(i + 1U));
                const uint32_t tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
        }

        if (isPaused) {
            if (millis() - pauseStart >= static_cast<uint16_t>(DrawDelays::FULL_FILL_DELAY)) {
                isPaused = false;
                isClearing = true;
                filled = 0U;

                for (uint32_t i = StripControl::MATRIX_HEIGHT - 1U; i > 0U; i--) {
                    const uint8_t j = random8(static_cast<uint8_t>(i + 1U));
                    const uint32_t tmp = order[i];
                    order[i] = order[j];
                    order[j] = tmp;
                }
            }
            return;
        }

        if (millis() - lastTime < 100U) {
            return;
        }
        lastTime = millis();

        const uint32_t rawColor = pgm_read_dword_near(&Colors::mainColors[color]);
        const CRGB fillColor = isClearing ? CRGB::Black : CRGB(rawColor);

        for (uint32_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
            StripControl::leds[Effects::getIndex(x, order[filled])] = fillColor;
        }
        StripControl::show();

        filled++;

        if (filled >= StripControl::MATRIX_HEIGHT) {
            if (!isClearing) {
                isPaused = true;
                pauseStart = millis();
            } else {
                isClearing = false;
                filled = 0U;
                color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
                for (uint32_t i = StripControl::MATRIX_HEIGHT - 1U; i > 0U; i--) {
                    const uint8_t j = random8(static_cast<uint8_t>(i + 1U));
                    const uint32_t tmp = order[i];
                    order[i] = order[j];
                    order[j] = tmp;
                }
            }
        }
    }

    void drawChameleonSnake() {
        static uint32_t arrPos = 0U;
        static uint32_t arrVolume = 0U;
        static uint32_t color = 0U;
        static uint32_t pos = 0U;
        static uint32_t n = 0U;
        static uint32_t previousMillis = 0U;

        if (Effects::checkCommandReceived()) {
            arrPos = 0U;
            arrVolume = 0U;
            color = 0U;
            pos = 0U;
            n = 0U;
            previousMillis = 0U;
        }

        if (millis() - previousMillis < static_cast<uint16_t>(DrawDelays::CHAMELEON_SNAKE_DELAY)) {
            return;
        }
        previousMillis = millis();

        if (n >= StripControl::MATRIX_LEDS) {
            arrVolume = 0U;
            arrPos = 0U;
            pos = 0U;
            n = 0U;
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            return;
        }

        if (constexpr uint8_t chameleon[] = {3U,  20U, 5U, 10U, 5U, 1U,  3U, 6U,  2U, 8U, 7U, 2U,
                                             15U, 8U,  3U, 7U,  4U, 10U, 2U, 15U, 6U, 2U, 5U};
            n <= pos + chameleon[arrPos]) {
            const uint32_t rawColor = pgm_read_dword_near(&Colors::mainColors[color]);
            StripControl::leds[n] = CRGB(rawColor);
            StripControl::show();
            n++;
        } else {
            pos = pos + chameleon[arrPos] + 1U;
            arrPos++;
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
            n = pos;
            arrVolume++;
        }

        if (arrVolume > 21U) {
            arrVolume = 0U;
            arrPos = 0U;
            pos = 0U;
            n = 0U;
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
        }
    }

    void drawLightBreath() {
        static uint32_t color = 0U;
        static uint8_t brightness = 0U;
        static int8_t direction = 1;
        static uint32_t lastTime = 0U;

        if (Effects::checkCommandReceived()) {
            color = 0U;
            brightness = 0U;
            direction = 1;
            lastTime = 0U;
        }

        if (millis() - lastTime < static_cast<uint16_t>(DrawDelays::LIGHT_BREATH_DELAY)) {
            return;
        }
        lastTime = millis();

        const uint32_t rawColor = pgm_read_dword_near(&Colors::mainColors[color]);
        auto baseColor = CRGB(rawColor);
        (void) baseColor.nscale8_video(brightness);
        fill_solid(&StripControl::leds[0], static_cast<int32_t>(StripControl::MATRIX_LEDS), baseColor);

        StripControl::show();

        if (brightness >= 255U) {
            direction = -1;
        } else if (brightness == 0U) {
            direction = 1;
            color = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 128));
        }

        brightness += static_cast<uint32_t>(direction);
    }

    void drawImages(const uint32_t subMode) {
        static uint32_t imageNum = 0U;
        static uint32_t locImgNum = 0U;
        static uint32_t previousMillis = 0U;
        static uint32_t previousMillisImgNum = 0U;
        static bool enableNewImg = false;
        static constexpr uint32_t IMAGES_COUNT = std::size(images);
        static uint32_t randomCounter = 0U;
        static std::deque<uint32_t> usedImg;

        const uint32_t currentMillis = millis();

        if (Effects::checkCommandReceived()) {
            imageNum = 0U;
            locImgNum = 0U;
            previousMillis = 0U;
            previousMillisImgNum = 0U;
            enableNewImg = true;
            randomCounter = 0U;
        }

        if (subMode == 254U) {
            if (currentMillis - previousMillis >= static_cast<uint16_t>(DrawDelays::DRAW_IMAGES_DELAY)) {
                previousMillis = currentMillis;

                if (imageNum < IMAGES_COUNT) {
                    Effects::drawPicture(images[imageNum]);
                    StripControl::show();
                    ++imageNum;
                } else {
                    imageNum = 0U;
                }
            }
        } else if (subMode == 255U) {
            if (currentMillis - previousMillis >= static_cast<uint16_t>(DrawDelays::DRAW_IMAGES_DELAY)) {
                previousMillis = currentMillis;

                if (imageNum < IMAGES_COUNT) {
                    Effects::drawPicture(images[imageNum]);
                    StripControl::show();
                    do {
                        imageNum = static_cast<uint32_t>(ESP8266TrueRandom.random(0, static_cast<long>(IMAGES_COUNT)));
                        randomCounter++;
                        if (randomCounter > 15U) {
                            break;
                        }
                    } while (std::find(usedImg.begin(), usedImg.end(), imageNum) != usedImg.end());

                    usedImg.push_back(imageNum);
                    if (usedImg.size() > 14U) {
                        usedImg.pop_front();
                    }
                } else {
                    imageNum = 0U;
                }
            }
        } else if (subMode > 100U && subMode < 129U && locImgNum != subMode && enableNewImg) {
            enableNewImg = false;
            locImgNum = subMode;
            Effects::drawPicture(images[subMode - 101U]);
            StripControl::show();
        }

        if (currentMillis - previousMillisImgNum >= 5000U) {
            previousMillisImgNum = currentMillis;
            locImgNum = 0U;
        }
    }
}
