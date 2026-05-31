#include "animationFrames.h"
#include "effects.h"

/*
  Эта группа эффектов только для матрицы 12х12

  Jp - Japanese (японский). Текст на оригинале: "あけましておめでとう". Корректное отображение текста "снаружи"
  Kr - Korean (корейский).  Текст на оригинале: "안녕하세요". Корректное отображение текста "снаружи"

  Настраиваемые параметры (AnimationsDelays):
    1) Время отображения одного кадра (задержка)
*/

namespace {
    enum class AnimationsDelays : uint16_t {
        HEART_IMAGE = 1500U,
        SMILE_IMAGE = 800U,
        JUMPING_MAN_IMAGE = 1000U,
        FIREBALL_IMAGE = 700U,
        EXPLOSION_IMAGE = 250U
    };

    struct LetterAnimState {
        uint32_t phase = 0U;
        uint32_t step = 0U;
        uint32_t currentLetter = 0U;
        uint32_t prevMillis = 0U;
    };

    constexpr const uint32_t *heartImage[] = {AnimationsFrames::heart1, AnimationsFrames::heart2,
                                              AnimationsFrames::heart3, AnimationsFrames::heart4};
    constexpr const uint32_t *smileImage[] = {AnimationsFrames::smile1, AnimationsFrames::smile2,
                                              AnimationsFrames::smile3, AnimationsFrames::smile4};
    constexpr const uint32_t *jumpingManImage[] = {AnimationsFrames::jumpingMan1, AnimationsFrames::jumpingMan2};
    constexpr const uint32_t *fireballImage[] = {AnimationsFrames::fireball1, AnimationsFrames::fireball2,
                                                 AnimationsFrames::fireball3, AnimationsFrames::fireball4};
    constexpr const uint32_t *explosionImage[] = {
            AnimationsFrames::explosion1,  AnimationsFrames::explosion2,  AnimationsFrames::explosion3,
            AnimationsFrames::explosion4,  AnimationsFrames::explosion5,  AnimationsFrames::explosion6,
            AnimationsFrames::explosion7,  AnimationsFrames::explosion8,  AnimationsFrames::explosion9,
            AnimationsFrames::explosion10, AnimationsFrames::explosion11, AnimationsFrames::explosion12,
            AnimationsFrames::explosion13, AnimationsFrames::explosion14, AnimationsFrames::explosion15,
            AnimationsFrames::explosion16, AnimationsFrames::explosion17, AnimationsFrames::explosion18};
    constexpr const uint32_t *jpLetters[] = {AnimationsFrames::jpLetter1, AnimationsFrames::jpLetter2,
                                             AnimationsFrames::jpLetter3, AnimationsFrames::jpLetter4,
                                             AnimationsFrames::jpLetter5, AnimationsFrames::jpLetter6,
                                             AnimationsFrames::jpLetter7, AnimationsFrames::jpLetter8,
                                             AnimationsFrames::jpLetter9, AnimationsFrames::jpLetter10};
    constexpr const uint32_t *krLetters[] = {AnimationsFrames::krLetter1, AnimationsFrames::krLetter2,
                                             AnimationsFrames::krLetter3, AnimationsFrames::krLetter4,
                                             AnimationsFrames::krLetter5};

    constexpr const uint32_t *signalsImage[] = {
            AnimationsFrames::signal1, AnimationsFrames::signal2, AnimationsFrames::signal3, AnimationsFrames::signal4,
            AnimationsFrames::signal5, AnimationsFrames::signal6, AnimationsFrames::signal7};

    LetterAnimState stateJp;
    LetterAnimState stateKr;

    void drawAnimation(const uint32_t *const *images, uint32_t numImages, uint32_t interval, bool isNewAnim);
    void shiftLeft(uint32_t a);
    void drawShifted(const uint32_t *image, int32_t offsetY, bool append);
    void processLetters(LetterAnimState &st, const uint32_t *const *letters, uint32_t totalLetters);
} // namespace

namespace Effects {
    constexpr uint8_t mainMatrixScheme[] = {
            132U, 131U, 108U, 107U, 84U, 83U, 60U,  59U,  36U,  35U,  12U, 11U, 133U, 130U, 109U, 106U, 85U, 82U,
            61U,  58U,  37U,  34U,  13U, 10U, 134U, 129U, 110U, 105U, 86U, 81U, 62U,  57U,  38U,  33U,  14U, 9U,
            135U, 128U, 111U, 104U, 87U, 80U, 63U,  56U,  39U,  32U,  15U, 8U,  136U, 127U, 112U, 103U, 88U, 79U,
            64U,  55U,  40U,  31U,  16U, 7U,  137U, 126U, 113U, 102U, 89U, 78U, 65U,  54U,  41U,  30U,  17U, 6U,
            138U, 125U, 114U, 101U, 90U, 77U, 66U,  53U,  42U,  29U,  18U, 5U,  139U, 124U, 115U, 100U, 91U, 76U,
            67U,  52U,  43U,  28U,  19U, 4U,  140U, 123U, 116U, 99U,  92U, 75U, 68U,  51U,  44U,  27U,  20U, 3U,
            141U, 122U, 117U, 98U,  93U, 74U, 69U,  50U,  45U,  26U,  21U, 2U,  142U, 121U, 118U, 97U,  94U, 73U,
            70U,  49U,  46U,  25U,  22U, 1U,  143U, 120U, 119U, 96U,  95U, 72U, 71U,  48U,  47U,  24U,  23U, 0U};

    void drawAnimations(const uint32_t subMode) {
        static uint32_t currentSubMode = 255U;
        switch (subMode) {
            case 0U:
                drawAnimation(&heartImage[0], 4U, static_cast<uint32_t>(AnimationsDelays::HEART_IMAGE),
                              currentSubMode != 0U);
                break;

            case 1U:
                drawAnimation(&smileImage[0], 4U, static_cast<uint32_t>(AnimationsDelays::SMILE_IMAGE),
                              currentSubMode != 1U);
                break;

            case 2U:
                drawAnimation(&jumpingManImage[0], 2U, static_cast<uint32_t>(AnimationsDelays::JUMPING_MAN_IMAGE),
                              currentSubMode != 2U);
                break;

            case 3U:
                drawAnimation(&fireballImage[0], 4U, static_cast<uint32_t>(AnimationsDelays::FIREBALL_IMAGE),
                              currentSubMode != 3U);
                break;

            case 4U:
                drawAnimation(&explosionImage[0], 18U, static_cast<uint32_t>(AnimationsDelays::EXPLOSION_IMAGE),
                              currentSubMode != 4U);
                break;

            case 5U:
                processLetters(stateJp, &jpLetters[0], 10U);
                break;

            case 6U:
                processLetters(stateKr, &krLetters[0], 5U);
                break;

            case 7U:
                shiftLeft(0U);
                break;

            case 8U:
                shiftLeft(1U);
                break;

            case 9U:
                shiftLeft(2U);
                break;

            case 10U:
                shiftLeft(3U);
                break;

            case 11U:
                shiftLeft(4U);
                break;

            case 12U:
                shiftLeft(5U);
                break;

            case 13U:
                shiftLeft(6U);
                break;

            case 255U:
                stateJp = {0U, 0U, 0U, 0U};
                stateKr = {0U, 0U, 0U, 0U};
                break;

            default:
                break;
        }
        currentSubMode = subMode;
    }

    void drawPicture(const uint32_t *p2) {
        if (p2 == nullptr) {
            return;
        }
        for (uint32_t i = 0U; i < StripControl::MATRIX_LEDS; i++) {
            if (const uint8_t targetLed = mainMatrixScheme[i]; targetLed < StripControl::MATRIX_LEDS) {
                uint32_t rawColor = 0U;
                (void) memcpy_P(&rawColor, &p2[i], sizeof(uint32_t));
                StripControl::leds[targetLed] = CRGB(rawColor);
            }
        }
    }

    uint32_t getIndex(const uint32_t x, const uint32_t y) {
        uint32_t localTemp = 0U;
        if (x % 2U == 0U) {
            localTemp = x * StripControl::MATRIX_HEIGHT + y;
        } else {
            localTemp = x * StripControl::MATRIX_HEIGHT + (StripControl::MATRIX_HEIGHT - 1U - y);
        }
        return localTemp;
    }
} // namespace Effects

namespace {
    void drawShifted(const uint32_t *image, const int32_t offsetY, const bool append) {
        if (image == nullptr) {
            return;
        }
        if (!append) {
            fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
        }
        for (int32_t row = 0; static_cast<uint32_t>(row) < StripControl::MATRIX_HEIGHT; row++) {
            const auto imgRow = static_cast<int32_t>(row + offsetY);
            if (imgRow < 0 || static_cast<uint32_t>(imgRow) >= StripControl::MATRIX_HEIGHT) {
                continue;
            }
            for (int32_t col = 0; static_cast<uint32_t>(col) < StripControl::MATRIX_WIDTH; col++) {
                uint32_t color = 0U;
                (void) memcpy_P(&color,
                                &image[static_cast<uint32_t>(imgRow) * StripControl::MATRIX_HEIGHT +
                                       static_cast<uint32_t>(col)],
                                sizeof(uint32_t));
                if (color > 0U) {
                    if (const auto ledIndex = static_cast<uint32_t>(pgm_read_byte(
                                &Effects::mainMatrixScheme[static_cast<uint32_t>(row) * StripControl::MATRIX_HEIGHT +
                                                           static_cast<uint32_t>(col)]));
                        ledIndex < StripControl::MATRIX_LEDS) {
                        StripControl::leds[ledIndex] = CRGB(color);
                    }
                }
            }
        }
    }

    void processLetters(LetterAnimState &st, const uint32_t *const *letters, const uint32_t totalLetters) {
        const uint32_t currentMillis = millis();
        if (currentMillis - st.prevMillis < 120U) {
            return;
        }
        st.prevMillis = currentMillis;

        switch (st.phase) {
            case 0U:
                if (st.step <= StripControl::MATRIX_HEIGHT) {
                    drawShifted(letters[0], static_cast<int8_t>(StripControl::MATRIX_HEIGHT - st.step), false);
                    st.step++;
                } else {
                    st.phase = 1U;
                    st.step = 0U;
                    st.currentLetter = 0U;
                }
                break;

            case 1U:
                if (st.currentLetter < totalLetters - 1U) {
                    if (st.step <= StripControl::MATRIX_HEIGHT + 3U) {
                        drawShifted(letters[st.currentLetter], -static_cast<int32_t>(st.step), false);
                        if (const auto nextOffset = static_cast<int32_t>(StripControl::MATRIX_HEIGHT + 3U - st.step);
                            static_cast<uint32_t>(nextOffset) <= StripControl::MATRIX_HEIGHT) {
                            drawShifted(letters[st.currentLetter + 1U], nextOffset, true);
                        }
                        st.step++;
                    } else {
                        st.step = 0U;
                        st.currentLetter++;
                    }
                } else {
                    st.phase = 2U;
                    st.step = 0U;
                }
                break;

            case 2U:
                if (st.step < 12U) {
                    drawShifted(letters[totalLetters - 1U], -static_cast<int32_t>(st.step), false);
                    st.step++;
                } else {
                    st.phase = 3U;
                }
                break;

            case 3U:
                if (st.step <= StripControl::MATRIX_HEIGHT) {
                    drawShifted(letters[totalLetters - 1U], -static_cast<int32_t>(st.step), false);
                    st.step++;
                } else {
                    st.phase = 0U;
                    st.step = 0U;
                    st.currentLetter = 0U;
                }
                break;
            default:;
        }
        StripControl::show();
    }

    void shiftLeft(const uint32_t a) {
        static uint32_t shift = 0U;
        static uint32_t lastA = 255U;
        static uint32_t lastTime = 0U;

        if (lastA != a) {
            shift = 0U;
            lastA = a;
        }

        if (millis() - lastTime < 100U) {
            return;
        }
        lastTime = millis();

        for (uint32_t row = 0U; row < StripControl::MATRIX_HEIGHT; row++) {
            for (uint32_t col = 0U; col < StripControl::MATRIX_WIDTH; col++) {
                const uint32_t index = (col + shift) % 12U;
                uint32_t color = 0U;
                (void) memcpy_P(&color, signalsImage[a] + (row * 12U + index), sizeof(uint32_t));
                StripControl::leds[Effects::getIndex(col, row)] = CRGB(color);
            }
        }
        StripControl::show();

        shift = (shift + 1U) % 12U;
    }

    void drawAnimation(const uint32_t *const *images, const uint32_t numImages, const uint32_t interval,
                       const bool isNewAnim) {
        static uint32_t previousMillis = 0U;
        static uint32_t currentImageIndex = 0U;
        const uint32_t currentMillis = millis();

        if (isNewAnim) {
            currentImageIndex = 0U;
            previousMillis = currentMillis;
            Effects::drawPicture(images[0]);
            StripControl::show();
            currentImageIndex = 1U;
            return;
        }

        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            if (currentImageIndex >= numImages) {
                currentImageIndex = 0U;
            }
            Effects::drawPicture(images[currentImageIndex]);
            StripControl::show();
            currentImageIndex = (currentImageIndex + 1U) % numImages;
        }
    }
} // namespace
