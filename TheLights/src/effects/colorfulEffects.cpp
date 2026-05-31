#include "effects.h"

/*
  Эта группа эффектов только для матриц 12х12 и 16х16

  Настраиваемые параметры цветных пятен:
    1) Скорость, переменная speed
    2) Масштаб, переменная scale

    Настраиваемые параметры цветных эффектов (ColorfulDelays):
    1) Скорости эффектов
*/

namespace {
    enum class ColorfulDelays : uint16_t {
        LIGHT_NOISE_DELAY = 50U,
        RAINBOW_WHEEL_DELAY = 20U,
        RAINBOW_WAVE_DELAY = 10U,
        RAINBOW_SNAKE_DELAY = 30U
    };

    CRGBPalette16 currentPalette(CloudColors_p);

    uint32_t speed = 1U;
    uint32_t scale = 25U;
    uint32_t colorLoop = 1U;

    uint8_t noise[16][16];

    void changePaletteAndSettingsPeriodically(bool resetVal);
    void drawColorfulSpots();
    void drawLightNoise();
    void drawDiagonalWaves();
    void drawRainbowWheel();
    void drawRainbowRipples();
    void drawRainbowWave();
    void drawRainbowSnake();
} // namespace

namespace Effects {
    void drawColorfulEffects(const uint32_t subMode) {
        switch (subMode) {
            case 0U:
                drawColorfulSpots();
                break;

            case 1U:
                drawLightNoise();
                break;

            case 2U:
                drawDiagonalWaves();
                break;

            case 3U:
                drawRainbowWheel();
                break;

            case 4U:
                drawRainbowRipples();
                break;

            case 5U:
                drawRainbowWave();
                break;

            case 6U:
                drawRainbowSnake();
                break;

            default:
                break;
        }
    }

    CRGB wheel(uint32_t WheelPos) {
        WheelPos = 255U - WheelPos;
        if (WheelPos < 85U) {
            return {static_cast<fl::u8>(255U - WheelPos * 3U), 0U, static_cast<fl::u8>(WheelPos * 3U)};
        }
        if (WheelPos < 170U) {
            WheelPos -= 85U;
            return {0U, static_cast<fl::u8>(WheelPos * 3U), static_cast<fl::u8>(255U - WheelPos * 3U)};
        }
        WheelPos -= 170U;
        return {static_cast<fl::u8>(WheelPos * 3U), static_cast<fl::u8>(255U - WheelPos * 3U), 0U};
    }
} // namespace Effects

namespace {
    void fillNoise8(const bool resetVal) {
        static uint32_t X = random16();
        static auto Y = static_cast<int32_t>(random16());
        static uint32_t Z = random16();

        if (resetVal) {
            X = random16();
            Y = static_cast<int32_t>(random16());
            Z = random16();
        }

        uint32_t dataSmoothing = 0U;
        if (speed < 50U) {
            dataSmoothing = 400U - speed * 4U;
        }

        for (uint32_t i = 0U; i < StripControl::MATRIX_WIDTH; i++) {
            const uint32_t iOffset = scale * i;
            for (uint32_t j = 0U; j < StripControl::MATRIX_HEIGHT; j++) {
                const uint32_t jOffset = scale * j;

                uint32_t data =
                        inoise8(static_cast<uint16_t>(X + iOffset),
                                static_cast<uint16_t>(Y + static_cast<int32_t>(jOffset)), static_cast<uint16_t>(Z));
                data = qsub8(static_cast<uint8_t>(data), 16U);
                data = qadd8(static_cast<uint8_t>(data), scale8(static_cast<uint8_t>(data), 39U));

                if (dataSmoothing > 0U) {
                    const uint32_t oldData = noise[i][j];
                    const uint32_t newData = static_cast<uint32_t>(scale8(static_cast<uint8_t>(oldData),
                                                                          static_cast<uint8_t>(dataSmoothing))) +
                                             static_cast<uint32_t>(scale8(static_cast<uint8_t>(data),
                                                                          static_cast<uint8_t>(256U - dataSmoothing)));
                    data = newData;
                }

                noise[i][j] = static_cast<uint8_t>(data);
            }
        }

        Z += speed;
        X += speed / 8U;
        Y -= static_cast<int32_t>(speed / 16U);
    }

    void mapNoiseToLedsUsingPalette(const bool resetVal) {
        static uint32_t iHue = 0U;

        if (resetVal) {
            iHue = 0U;
        }

        for (uint32_t i = 0U; i < StripControl::MATRIX_WIDTH; i++) {
            for (uint32_t j = 0U; j < StripControl::MATRIX_HEIGHT; j++) {
                uint32_t index = noise[j][i];
                uint32_t bri = noise[i][j];

                if (colorLoop > 0U) {
                    index += iHue;
                }

                if (bri > 127U) {
                    bri = 255U;
                } else {
                    bri = dim8_raw(static_cast<uint8_t>(bri * 2U));
                }

                const CRGB color =
                        ColorFromPalette(currentPalette, static_cast<uint8_t>(index), static_cast<uint8_t>(bri));
                StripControl::leds[Effects::getIndex(i, j)] = color;
            }
        }

        iHue += 1U;
    }

    void SetupRandomPalette() {
        currentPalette = CRGBPalette16(CHSV(random8(), 255U, 32U), CHSV(random8(), 255U, 255U),
                                       CHSV(random8(), 128U, 255U), CHSV(random8(), 255U, 255U));
    }

    void changePaletteAndSettingsPeriodically(const bool resetVal) {
        const uint32_t secondHand = millis() / 1000U / 10U % 60U;
        static uint32_t lastSecond = 99U;

        if (resetVal) {
            lastSecond = 99U;
        }

        if (lastSecond != secondHand) {
            lastSecond = secondHand;
            if (secondHand % 5U == 0U) {
                SetupRandomPalette();
                speed = 1U;
                scale = 30U;
                colorLoop = secondHand == 25U ? 0U : 1U;
            }
        }
    }

    void drawColorfulSpots() {
        if (Effects::checkCommandReceived()) {
            speed = 1U;
            scale = 25U;
            colorLoop = 1U;
            changePaletteAndSettingsPeriodically(true);
            fillNoise8(true);
            mapNoiseToLedsUsingPalette(true);
        } else {
            changePaletteAndSettingsPeriodically(false);
            fillNoise8(false);
            mapNoiseToLedsUsingPalette(false);
        }
        StripControl::show();
    }

    void drawLightNoise() {
        static uint32_t lightersPosX6[32] = {};
        static uint32_t lightersPosY6[32] = {};
        static uint32_t lightersSpeedX6[32] = {};
        static uint32_t lightersSpeedY6[32] = {};
        static uint8_t lightersSpeedZ[32] = {};
        static uint8_t lColor6[32] = {};
        static uint8_t mass6[32] = {};
        static bool loadingFlag6 = true;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(ColorfulDelays::LIGHT_NOISE_DELAY)) {
            return;
        }
        lastTime = millis();

        static XYMap xyMap(static_cast<uint16_t>(StripControl::MATRIX_WIDTH),
                           static_cast<uint16_t>(StripControl::MATRIX_HEIGHT));
        if (loadingFlag6) {
            loadingFlag6 = false;
            randomSeed(millis());
            for (byte i = 0U; i < 32U; i++) {
                lightersSpeedX6[i] = static_cast<uint32_t>(-10 + ESP8266TrueRandom.random(0, 21));
                lightersSpeedY6[i] = static_cast<uint32_t>(-10 + ESP8266TrueRandom.random(0, 21));
                mass6[i] = static_cast<uint8_t>(ESP8266TrueRandom.random(0, 6) + 5);
                lightersSpeedZ[i] = static_cast<uint8_t>(3 + ESP8266TrueRandom.random(0, 23));
                lightersPosX6[i] = static_cast<uint32_t>(
                        ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_WIDTH * 10U)));
                lightersPosY6[i] = static_cast<uint32_t>(
                        ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_HEIGHT * 10U)));
                lColor6[i] = static_cast<uint8_t>(ESP8266TrueRandom.random(0, 9) * 28);
            }
        }

        blur2d(&StripControl::leds[0], static_cast<uint8_t>(StripControl::MATRIX_WIDTH),
               static_cast<uint8_t>(StripControl::MATRIX_HEIGHT), 30U, xyMap);
        fadeToBlackBy(&StripControl::leds[0], static_cast<uint16_t>(StripControl::MATRIX_LEDS), 25U);

        for (byte i = 0U; i < 32U; i++) {
            lColor6[i]++;

            lightersPosX6[i] += static_cast<uint32_t>(static_cast<double>(mass6[i]) *
                                                      cos(radians(static_cast<double>(lightersSpeedY6[i]))) /
                                                      static_cast<double>(map(255, 1, 255, 10, 1)));
            lightersPosY6[i] += static_cast<uint32_t>(static_cast<double>(mass6[i]) *
                                                      sin(radians(static_cast<double>(lightersSpeedY6[i]))) /
                                                      static_cast<double>(map(255, 1, 255, 10, 1)));
            lightersSpeedY6[i] += static_cast<uint32_t>(static_cast<double>(lightersSpeedX6[i]) /
                                                        static_cast<double>(map(255, 1, 255, 20, 2)));

            if (lightersPosY6[i] >= (StripControl::MATRIX_HEIGHT - 1U) * 10U) {
                lightersPosY6[i] = (StripControl::MATRIX_HEIGHT - 1U) * 10U - 1U;
                lightersSpeedY6[i] = static_cast<uint32_t>(360U - lightersSpeedY6[i]);
            }
            if (lightersPosX6[i] >= (StripControl::MATRIX_WIDTH - 1U) * 10U) {
                lightersPosX6[i] = (StripControl::MATRIX_WIDTH - 1U) * 10U - 1U;
                lightersSpeedY6[i] = static_cast<uint32_t>(180U - lightersSpeedY6[i]);
            }

            CRGB color = CHSV(lColor6[i], 255U,
                              beatsin8(static_cast<uint8_t>(static_cast<double>(lightersSpeedZ[i]) /
                                                            static_cast<double>(map(255, 1, 255, 10, 1))),
                                       128U, 255U));
            Effects::drawPixel(static_cast<float>(lightersPosX6[i]) / 10.0F,
                               static_cast<float>(lightersPosY6[i]) / 10.0F, color);
        }

        EVERY_N_SECONDS(15U) {
            randomSeed(millis());
            for (byte i = 0U; i < 32U; i++) {
                lightersSpeedX6[i] = static_cast<uint32_t>(-10 + ESP8266TrueRandom.random(0, 21));
                lightersSpeedY6[i] = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 360));
                mass6[i] = static_cast<uint8_t>(5 + ESP8266TrueRandom.random(0, 6));
                lightersSpeedZ[i] = static_cast<uint8_t>(3 + ESP8266TrueRandom.random(0, 23));
            }
        }
        StripControl::show();
    }

    void drawDiagonalWaves() {
        static float hue = 0.0F;
        static float wave = 0.0F;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < 40U) {
            return;
        }
        lastTime = millis();

        hue += 0.3F;
        wave += 2.5F;

        if (hue >= 256.0F) {
            hue -= 256.0F;
        }
        if (wave >= 256.0F) {
            wave -= 256.0F;
        }

        for (uint32_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
            for (uint32_t y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
                const uint32_t index = Effects::getIndex(x, y);

                // волна по диагонали
                const uint8_t brightness = sin8(static_cast<uint8_t>(x * 20U + y * 20U + static_cast<uint8_t>(wave)));

                // цвет медленно плывёт + небольшое смещение по диагонали
                const uint8_t pixelHue = static_cast<uint8_t>(hue) + (x + y) * 4U;

                StripControl::leds[index] = CHSV(pixelHue, 240U, brightness);
            }
        }
        StripControl::show();
    }

    // void drawDiagonalWaves() {
    //     static uint8_t hue = 0U;
    //     static uint32_t lastTime = 0U;
    //
    //     if (millis() - lastTime < 25U) {
    //         return;
    //     }
    //     lastTime = millis();
    //
    //     for (uint32_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
    //         for (uint32_t y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
    //             const uint32_t index = Effects::getIndex(x, y);
    //             const uint8_t brightness = sin8(static_cast<uint8_t>(x * 8U + y * 8U + hue));
    //             StripControl::leds[index] = CHSV(hue, 255U, brightness);
    //         }
    //     }
    //     hue++;
    //     StripControl::show();
    // }

    void drawRainbowWheel() {
        static uint8_t angleTable[StripControl::MATRIX_HEIGHT][StripControl::MATRIX_WIDTH] = {};
        static uint8_t distTable[StripControl::MATRIX_HEIGHT][StripControl::MATRIX_WIDTH] = {};
        static bool tablesReady = false;
        static uint32_t rotation = 0U;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(ColorfulDelays::RAINBOW_WHEEL_DELAY)) {
            return;
        }

        rotation += (millis() - lastTime) * 10U;
        lastTime = millis();

        for (uint32_t y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
            for (uint32_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
                if (!tablesReady) {
                    tablesReady = true;
                    constexpr float cx = static_cast<float>(StripControl::MATRIX_WIDTH) / 2.0F - 0.5F;
                    constexpr float cy = static_cast<float>(StripControl::MATRIX_HEIGHT) / 2.0F - 0.5F;
                    for (uint32_t yi = 0U; yi < StripControl::MATRIX_HEIGHT; yi++) {
                        for (uint32_t xi = 0U; xi < StripControl::MATRIX_WIDTH; xi++) {
                            const float dx = static_cast<float>(xi) - cx;
                            const float dy = static_cast<float>(yi) - cy;
                            angleTable[yi][xi] = static_cast<uint8_t>(atan2(dy, dx) * 128.0F / PI + 128.0F);
                            distTable[yi][xi] = static_cast<uint8_t>(constrain(sqrt(dx * dx + dy * dy) * 30, 30, 255));
                        }
                    }
                }
                StripControl::leds[Effects::getIndex(x, y)] =
                        CHSV(static_cast<uint8_t>(static_cast<uint32_t>(angleTable[y][x]) + (rotation >> 8)), 255U,
                             distTable[y][x]);
            }
        }
        StripControl::show();
    }

    void drawRainbowRipples() {
        static uint32_t sPseudotime = 0U;
        static uint32_t sLastMillis = 0U;
        static uint32_t sHue = 0U;

        const uint32_t brightnessThetaInc16 = beatsin88(203U, 25U * 256U, 40U * 256U);
        const uint32_t msMultiplier = beatsin88(147U, 23U, 60U);

        uint32_t hue16 = sHue;
        const uint16_t hueInc16 = beatsin88(113U, 1U, 3000U);

        const uint32_t ms = millis();
        const uint32_t deltaMs = ms - sLastMillis;
        sLastMillis = ms;
        sPseudotime += deltaMs * msMultiplier;
        sHue += deltaMs * beatsin88(400U, 5U, 9U);
        uint32_t brightnessTheta16 = sPseudotime;

        for (uint16_t i = 0U; i < StripControl::MATRIX_LEDS; i++) {
            hue16 += hueInc16;
            const uint32_t hue8 = hue16 / 256U;

            brightnessTheta16 += brightnessThetaInc16;
            const uint32_t b16 = static_cast<uint32_t>(sin16(static_cast<uint16_t>(brightnessTheta16))) + 32768U;

            const uint32_t bri16 = static_cast<uint32_t>(b16) * static_cast<uint32_t>(b16) / 65536U;
            uint32_t bri8 = static_cast<uint32_t>(bri16) * 128U / 65536U;
            bri8 += 255U - 128U;

            CRGB newColor = CHSV(static_cast<uint8_t>(hue8), 255U, static_cast<uint8_t>(bri8));

            uint32_t pixelNumber = i;

            pixelNumber = StripControl::MATRIX_LEDS - 1U - pixelNumber;

            (void) nblend(StripControl::leds[pixelNumber], newColor, 64U);
        }
        StripControl::show();
    }

    void drawRainbowWave() {
        static uint32_t waveRainbow = 0U;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(ColorfulDelays::RAINBOW_WAVE_DELAY)) {
            return;
        }
        lastTime = millis();

        if (++waveRainbow >= 256U) {
            waveRainbow = 0U;
        }
        for (uint16_t i = 0U; i < StripControl::MATRIX_LEDS; i++) {
            StripControl::leds[i] = Effects::wheel((i + waveRainbow) & 255U);
        }
        StripControl::show();
    }

    void fadeAll() {
        for (auto &led: StripControl::leds) {
            (void) led.nscale8(250U);
        }
    }

    void drawRainbowSnake() {
        static int16_t position = 0;
        static int16_t direction = 1;
        static uint8_t hue = 0U;
        static uint32_t lastTime = 0U;

        if (Effects::checkCommandReceived()) {
            position = 0;
            direction = 1;
            hue = 0U;
            lastTime = 0U;
        }

        if (millis() - lastTime < static_cast<uint32_t>(ColorfulDelays::RAINBOW_SNAKE_DELAY)) {
            return;
        }
        lastTime = millis();

        if (position >= 0 && static_cast<uint32_t>(position) < StripControl::MATRIX_LEDS) {
            StripControl::leds[position] = CHSV(hue++, 255U, 255U);
        }

        fadeAll();
        StripControl::show();

        position = static_cast<int16_t>((position + direction));

        if (position >= static_cast<int16_t>(StripControl::MATRIX_LEDS)) {
            position = static_cast<int16_t>(StripControl::MATRIX_LEDS - 2U);
            direction = -1;
        } else if (position < 0) {
            position = 1;
            direction = 1;
        }
    }
} // namespace
