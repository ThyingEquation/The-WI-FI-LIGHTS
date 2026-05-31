#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (SpaceDelays):
    1) Скорости эффектов
*/

namespace {
    enum class SpaceDelays : uint16_t {
        STAR_SKY_DELAY = 1500U,
        CONSTELLATION_DELAY = 10000U,
        PULSATING_STAR_DELAY = 200U,
        ECLIPSING_ROTATION_SPEED = 3U,
        STARFALL_DELAY = 100U,
        GALAXY_DELAY = 100U,
        NEBULA_DELAY = 130U,
        MOON_PHASE_DELAY = 200U,
        BLACK_HOLE_DELAY = 100U,
        WAVES_DELAY = 20U
    };

    struct Star {
        uint8_t x{0U};
        uint8_t y{0U};
        uint8_t bright{0U};
    };

    struct Constellation {
        Star stars[15];
        uint8_t count{};
    };

    constexpr uint32_t starColors[] = {0x95A7E8U, 0x4A5374U, 0x1A1E2AU, 0xE0E8FFU, 0x70747FU, 0x202226U, 0xFF9F13U,
                                       0x7F4F09U, 0x2F1D03U, 0xFC7F14U, 0x7E3F0AU, 0x2E1704U, 0xF25F6AU, 0x792F35U,
                                       0x2D1114U, 0x3870FFU, 0x1C387FU, 0x0A142FU, 0xAA96DCU, 0x554B6EU, 0x1F1B29U};

    constexpr uint8_t starColorsCount = std::size(starColors);

    Constellation coordinates[] = {
            {{{11U, 3U, 180U}, {7U, 0U, 255U}, {5U, 5U, 200U}, {2U, 6U, 255U}, {0U, 11U, 180U}}, 5U},
            {{{0U, 6U, 190U},
              {2U, 8U, 180U},
              {2U, 9U, 10U},
              {4U, 7U, 220U},
              {6U, 7U, 70U},
              {7U, 4U, 180U},
              {10U, 5U, 210U},
              {11U, 9U, 255U}},
             8U},
            {{{0U, 2U, 100U},
              {2U, 4U, 150U},
              {3U, 4U, 190U},
              {3U, 11U, 30U},
              {4U, 4U, 150U},
              {5U, 0U, 255U},
              {5U, 11U, 10U},
              {7U, 8U, 100U},
              {11U, 8U, 30U},
              {11U, 6U, 30U},
              {10U, 4U, 100U},
              {9U, 3U, 30U},
              {8U, 3U, 30U}},
             13U},
            {{{4U, 5U, 100U}, {5U, 8U, 100U}, {6U, 11U, 255U}, {7U, 0U, 110U}, {8U, 3U, 110U}}, 5U},
            {{{6U, 10U, 255U}, {3U, 5U, 210U}, {9U, 5U, 210U}, {6U, 0U, 240U}, {7U, 3U, 130U}}, 5U},
            {{{1U, 4U, 160U}, {3U, 0U, 200U}, {6U, 1U, 255U}, {9U, 5U, 200U}, {8U, 10U, 160U}}, 5U},
    };

    uint16_t currentStarCount = 0U;

    void drawStarSky();
    void drawConstellations();
    void drawStarFall();
    void drawPulsatingStar();
    void drawEclipsingBinaryStars();
    void drawSpiralNebula();
    void drawSpiralGalaxy();
    void drawMoonPhases();
    void drawJupiter();
    void drawBlackHole();
    void drawSinusoidWaves(uint32_t sinNum);
    void drawAurora();
} // namespace

namespace Effects {
    void drawSpaceEffects(const uint32_t subMode) {
        switch (subMode) {
            case 0U:
                drawStarSky();
                break;
            case 1U:
                drawConstellations();
                break;
            case 2U:
                drawPulsatingStar();
                break;
            case 3U:
                drawEclipsingBinaryStars();
                break;
            case 4U:
                drawStarFall();
                break;
            case 5U:
                drawSpiralNebula();
                break;
            case 6U:
                drawSpiralGalaxy();
                break;
            case 7U:
                drawMoonPhases();
                break;
            case 8U:
                drawJupiter();
                break;
            case 9U:
                drawBlackHole();
                break;
            case 10U:
                drawAurora();
                break;
            case 11U:
                drawSinusoidWaves(0U);
                break;
            case 12U:
                drawSinusoidWaves(1U);
                break;
            default:
                break;
        }
    }
} // namespace Effects

namespace {
    void addStar(const uint16_t starNum, const uint32_t color) {
        if (starNum < StripControl::MATRIX_LEDS) {
            if (StripControl::leds[starNum].r == 0U && StripControl::leds[starNum].g == 0U &&
                StripControl::leds[starNum].b == 0U) {
                currentStarCount++;
            }
            StripControl::leds[starNum] = CRGB(color);
        }
    }

    void removeStar(const uint16_t starNum) {
        if (starNum < StripControl::MATRIX_LEDS) {
            if (StripControl::leds[starNum].r != 0U || StripControl::leds[starNum].g != 0U ||
                StripControl::leds[starNum].b != 0U) {
                if (currentStarCount > 0U) {
                    currentStarCount--;
                }
            }
            StripControl::leds[starNum] = CRGB::Black;
        }
    }

    void drawStarSky() {
        static uint32_t step = 0U;
        static uint32_t lastTime = 0U;
        const uint32_t currentMillis = millis();
        const uint8_t pulse = beatsin8(15U, 120U, 255U);
        const uint8_t flicker = random8(200U, 255U);
        const uint8_t brightnessScale = scale8(pulse, flicker);

        CRGB saved[StripControl::MATRIX_LEDS];
        (void) memcpy(&saved[0], &StripControl::leds[0], sizeof(StripControl::leds));

        for (auto &led: StripControl::leds) {
            (void) led.nscale8(brightnessScale);
        }

        StripControl::show();
        (void) memcpy(&StripControl::leds[0], &saved[0], sizeof(StripControl::leds));
        delay(10U);

        if (Effects::checkCommandReceived()) {
            const auto rndLed =
                    static_cast<uint16_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_LEDS)));
            const auto rndColor =
                    static_cast<uint8_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(starColorsCount)));
            addStar(rndLed, starColors[rndColor]);
        }

        if (currentMillis - lastTime < static_cast<uint32_t>(SpaceDelays::STAR_SKY_DELAY)) {
            return;
        }
        lastTime = currentMillis;

        const bool overPopulated = currentStarCount >= StripControl::MATRIX_LEDS / 6U;

        if (step < 2U) {
            if (!overPopulated) {
                const auto rndLed = static_cast<uint16_t>(
                        ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_LEDS)));
                const auto rndColor =
                        static_cast<uint8_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(starColorsCount)));
                addStar(rndLed, starColors[rndColor]);
            }
            step++;
        } else if (step < 10U) {
            const auto rndLed =
                    static_cast<uint16_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_LEDS)));
            removeStar(rndLed);
            step++;
        } else {
            step = 0U;
        }
    }

    void drawConstellations() {
        static uint32_t currentID = 0U;
        static uint32_t lastSwitch = millis();
        static uint32_t lastTime = 0U;
        const uint32_t ms = millis();

        if (ms - lastTime < 100U) {
            return;
        }
        lastTime = ms;

        if (ms - lastSwitch > static_cast<uint32_t>(SpaceDelays::CONSTELLATION_DELAY)) {
            lastSwitch = ms;
            currentID = static_cast<uint32_t>((static_cast<uint16_t>(currentID) + 1U) % 6U);
        }

        Constellation &c = coordinates[currentID];
        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

        const uint32_t pulse = beatsin8(12U, 160U, 255U);

        for (uint32_t i = 0U; i < c.count; i++) {
            const uint32_t idx = Effects::getIndex(c.stars[i].x, c.stars[i].y);
            const uint8_t flicker = random8(190U, 255U);
            uint8_t finalBright = scale8(c.stars[i].bright, static_cast<uint8_t>(pulse));
            finalBright = scale8(finalBright, flicker);
            StripControl::leds[idx] = CRGB(200U, 225U, 255U);
            (void) StripControl::leds[idx].nscale8(finalBright);
        }

        StripControl::show();
    }

    void drawVortex(const int32_t currentRadius) {
        static constexpr int32_t maxRadius = static_cast<int32_t>(
                min((StripControl::MATRIX_WIDTH - 1U) / 2U, (StripControl::MATRIX_HEIGHT - 1U) / 2U));
        constexpr float centerX = static_cast<float>(StripControl::MATRIX_WIDTH - 1U) / 2.0F;
        constexpr float centerY = static_cast<float>(StripControl::MATRIX_HEIGHT - 1U) / 2.0F;
        static float rotationAngle = 0.0F;

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

        for (int32_t r = 0; r <= currentRadius; r++) {
            const float progress = static_cast<float>(r) / static_cast<float>(maxRadius);
            const auto red = static_cast<uint8_t>(255.0F * progress);
            const auto green = static_cast<uint8_t>(255.0F * progress);
            const auto blue = static_cast<uint8_t>(255.0F * (1.0F - progress));
            const CRGB color(red, green, blue);

            for (int32_t angle = 0; angle < 360; angle += 2) {
                const float rad = radians(static_cast<float>(angle) + rotationAngle);
                const auto x = static_cast<int32_t>(round(centerX + static_cast<float>(r) * cos(rad)));
                const auto y = static_cast<int32_t>(round(centerY + static_cast<float>(r) * sin(rad)));
                if (x >= 0 && x < static_cast<int32_t>(StripControl::MATRIX_WIDTH) && y >= 0 &&
                    y < static_cast<int32_t>(StripControl::MATRIX_HEIGHT)) {
                    StripControl::leds[Effects::getIndex(static_cast<uint32_t>(x), static_cast<uint32_t>(y))] = color;
                }
            }
        }

        if (currentRadius == 4) {
            StripControl::leds[126U] = CRGB(0x000000U);
            StripControl::leds[82U] = CRGB(0x000000U);
        } else if (currentRadius == 2) {
            StripControl::leds[102U] = CRGB(0x000000U);
            StripControl::leds[80U] = CRGB(0x000000U);
        }

        StripControl::show();

        rotationAngle += 2.5F;
        if (rotationAngle >= 360.0F) {
            rotationAngle -= 360.0F;
        }
    }

    void drawPulsatingStar() {
        static constexpr int32_t maxRadius = static_cast<int32_t>(
                min((StripControl::MATRIX_WIDTH - 1U) / 2U, (StripControl::MATRIX_HEIGHT - 1U) / 2U));
        static int32_t currentRadius = 0;
        static int8_t direction = 1;
        static uint32_t lastUpdateTime = 0U;

        if (const uint32_t currentMillis = millis();
            currentMillis - lastUpdateTime >= static_cast<uint32_t>(SpaceDelays::PULSATING_STAR_DELAY)) {
            lastUpdateTime = currentMillis;
            drawVortex(currentRadius);
            currentRadius = static_cast<int32_t>(currentRadius + static_cast<int16_t>(direction));

            if (currentRadius >= maxRadius) {
                direction = -1;
            } else if (currentRadius <= 0) {
                direction = 1;
            } else {
                // in range
            }
        }
    }

    float mapStar(const float x, const float inMin, const float inMax, const float outMin, const float outMax) {
        return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }

    void drawStar(const float x, const float y, const float r, const CRGB color) {

        const auto ix0 = static_cast<int16_t>(static_cast<int16_t>(x) - 4);
        const auto ix1 = static_cast<int16_t>(static_cast<int16_t>(x) + 4);
        const auto iy0 = static_cast<int16_t>(static_cast<int16_t>(y) - 4);
        const auto iy1 = static_cast<int16_t>(static_cast<int16_t>(y) + 4);

        for (int16_t ix = ix0; ix <= ix1; ix++) {
            for (int16_t iy = iy0; iy <= iy1; iy++) {
                if (ix >= 0 && ix < static_cast<int16_t>(StripControl::MATRIX_WIDTH) && iy >= 0 &&
                    iy < static_cast<int16_t>(StripControl::MATRIX_HEIGHT)) {
                    const auto fx = static_cast<float>(ix);
                    const auto fy = static_cast<float>(iy);

                    if (const float dist = sqrt((fx - x) * (fx - x) + (fy - y) * (fy - y)); dist < r) {
                        const uint32_t idx = Effects::getIndex(static_cast<uint32_t>(ix), static_cast<uint32_t>(iy));
                        const float centerFactor = 1.0F - dist / r;
                        const CRGB finalColor =
                                blend(color, CRGB::White, static_cast<uint8_t>(powf(centerFactor, 2.0F) * 255.0F));

                        uint8_t alpha = 255U;
                        if (constexpr float edgeSoftness = 0.8F; dist > r - edgeSoftness) {
                            alpha = static_cast<uint8_t>(mapStar(dist, r - edgeSoftness, r, 255.0F, 0.0F));
                        }

                        StripControl::leds[idx] = finalColor;
                        (void) StripControl::leds[idx].nscale8(alpha);
                    }
                }
            }
        }
    }

    void drawEclipsingBinaryStars() {
        static float angle = 0.0F;
        static uint32_t lastTime = 0U;
        const uint32_t now = millis();

        angle += static_cast<float>(static_cast<uint32_t>(SpaceDelays::ECLIPSING_ROTATION_SPEED)) / 1000.0F *
                 static_cast<float>(now - lastTime);
        lastTime = now;

        for (auto &led: StripControl::leds) {
            led = CRGB::Black;
        }

        constexpr float centerX = 5.5F;
        constexpr float centerY = 5.5F;
        constexpr float orbitRadius = 4.0F;

        const float xA = centerX + cos(angle) * orbitRadius;
        const float zA = sin(angle);
        const float xB = centerX + static_cast<float>(cos(angle + PI)) * orbitRadius;
        const auto zB = static_cast<float>(sin(angle + PI));

        const float sizeA = mapStar(zA, -1.0F, 1.0F, 1.0F, 3.0F);
        const float sizeB = mapStar(zB, -1.0F, 1.0F, 1.0F, 3.0F);

        if (zA < zB) {
            drawStar(xA, centerY, sizeA, CRGB(255U, 60U, 0U));
            drawStar(xB, centerY, sizeB, CRGB(0U, 50U, 255U));
        } else {
            drawStar(xB, centerY, sizeB, CRGB(0U, 50U, 255U));
            drawStar(xA, centerY, sizeA, CRGB(255U, 60U, 0U));
        }

        StripControl::show();
    }

    void drawStarFall() {
        static uint8_t currentCol[4] = {11U, 7U, 3U, 0U};
        static uint8_t currentRow[4] = {11U, 7U, 3U, 0U};
        constexpr uint8_t lineLength = 7U;
        static uint32_t lastTime = 0U;

        auto drawLine = [](const uint8_t col, const uint8_t row, const uint8_t length, const bool clear) {
            for (uint32_t j = 0U; j < length; ++j) {
                if (j > row || j > col) {
                    continue;
                }
                const uint32_t rowIndex = static_cast<uint8_t>(row - j);
                const uint32_t colIndex = static_cast<uint8_t>(col - j);
                if (rowIndex < StripControl::MATRIX_HEIGHT && colIndex < StripControl::MATRIX_WIDTH) {
                    const uint32_t pixelIndex = Effects::getIndex(colIndex, rowIndex);
                    if (clear) {
                        StripControl::leds[pixelIndex] = CRGB::Black;
                    } else {
                        const uint8_t brightness = (j == length - 1U) ? 255U : static_cast<uint8_t>(5U * j);
                        StripControl::leds[pixelIndex] = hsv2rgb_spectrum(CHSV(0U, 0U, brightness));
                    }
                }
            }
        };

        if (millis() - lastTime < static_cast<uint32_t>(SpaceDelays::STARFALL_DELAY)) {
            return;
        }
        lastTime = millis();

        for (uint8_t i = 0U; i < 4U; ++i) {
            drawLine(currentCol[i], currentRow[i], lineLength, false);
        }
        StripControl::show();

        for (uint8_t i = 0U; i < 4U; ++i) {
            drawLine(currentCol[i], currentRow[i], lineLength, true);
        }

        for (uint8_t i = 0U; i < 4U; ++i) {
            if (currentRow[i] > 0U) {
                --currentRow[i];
                --currentCol[i];
            } else {
                currentCol[i] = static_cast<uint8_t>(static_cast<uint16_t>(StripControl::MATRIX_WIDTH) + 4U +
                                                     static_cast<uint16_t>(ESP8266TrueRandom.random(0, 10)));
                currentRow[i] = static_cast<uint8_t>(static_cast<uint16_t>(StripControl::MATRIX_WIDTH) + 6U +
                                                     static_cast<uint16_t>(ESP8266TrueRandom.random(0, 10)));
            }
        }
    }

    float noiseField(const float x, const float y, const float t) {
        float n = sin(x * 1.7F + t * 0.3F) * cos(y * 1.3F - t * 0.2F);
        n += sin(x * 3.1F - y * 2.4F + t * 0.15F) * 0.5F;
        n += cos(x * 0.9F + y * 2.1F + t * 0.4F) * 0.3F;
        return (n / 1.8F + 1.0F) / 2.0F;
    }

    void drawSpiralGalaxy() {
        static float rotationOffset = 0.0F;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(SpaceDelays::GALAXY_DELAY)) {
            return;
        }
        lastTime = millis();

        rotationOffset += 0.008F;

        for (auto &led: StripControl::leds) {
            led = CRGB::Black;
        }

        for (uint8_t y = 0U; y < 12U; y++) {
            for (uint8_t x = 0U; x < 12U; x++) {
                constexpr uint8_t numArms = 2U;
                constexpr float tightness = 0.75F;
                constexpr float centerY = 5.5F;
                constexpr float centerX = 5.5F;
                const float dx = static_cast<float>(x) - centerX;
                const float dy = static_cast<float>(y) - centerY;
                const float r = sqrt(dx * dx + dy * dy);
                const float angle = atan2(dy, dx);
                const float spiral =
                        (sin(static_cast<float>(numArms) * angle - tightness * r + rotationOffset) + 1.0F) / 2.0F;
                const float armVal = powf(spiral, 4.0F);
                const float bulge = expf(-r * 0.75F) * 1.8F;
                const float nebulaNoise = noiseField(dx * 0.6F, dy * 0.6F, rotationOffset);
                const float cloudLayer = noiseField(dx * 1.2F + 5.0F, dy * 1.1F, rotationOffset * 0.7F);
                const float nebulaBoost = nebulaNoise * 0.4F * armVal + cloudLayer * 0.15F;
                float finalBright = armVal * 0.6F + bulge + nebulaBoost;

                if (finalBright > 1.0F) {
                    finalBright = 1.0F;
                }

                constexpr float edge = 6.0F;
                const float mask = (r < edge) ? max(0.0F, 1.0F - (r / edge) * (r / edge)) : 0.0F;
                finalBright *= mask;

                if (finalBright > 0.03F) {
                    constexpr auto coreColor = CRGB(255U, 220U, 160U);
                    constexpr auto armColor = CRGB(60U, 80U, 220U);
                    constexpr auto nebulaColor = CRGB(160U, 30U, 120U);
                    constexpr auto coldColor = CRGB(10U, 10U, 80U);

                    const float coreWeight = expf(-r * 1.2F);
                    const float armWeight = armVal * (1.0F - coreWeight);
                    const float nebulaWeight = nebulaNoise * 0.3F * (1.0F - coreWeight);
                    const float coldWeight = max(0.0F, 1.0F - coreWeight - armWeight - nebulaWeight);

                    const auto color = CRGB(static_cast<uint8_t>(static_cast<float>(coreColor.r) * coreWeight +
                                                                 static_cast<float>(armColor.r) * armWeight +
                                                                 static_cast<float>(nebulaColor.r) * nebulaWeight +
                                                                 static_cast<float>(coldColor.r) * coldWeight),
                                            static_cast<uint8_t>(static_cast<float>(coreColor.g) * coreWeight +
                                                                 static_cast<float>(armColor.g) * armWeight +
                                                                 static_cast<float>(nebulaColor.g) * nebulaWeight +
                                                                 static_cast<float>(coldColor.g) * coldWeight),
                                            static_cast<uint8_t>(static_cast<float>(coreColor.b) * coreWeight +
                                                                 static_cast<float>(armColor.b) * armWeight +
                                                                 static_cast<float>(nebulaColor.b) * nebulaWeight +
                                                                 static_cast<float>(coldColor.b) * coldWeight));

                    const uint8_t grain = random8(220U, 255U);
                    const uint32_t idx = Effects::getIndex(x, y);
                    StripControl::leds[idx] = color;
                    (void) StripControl::leds[idx].nscale8(static_cast<uint8_t>(finalBright * 255.0F));
                    (void) StripControl::leds[idx].nscale8(grain);
                }
            }
        }

        StripControl::show();
    }

    float sNoise(const float x, const float y) {
        return (sin(x * 1.7F + y * 0.9F) + sin(x * 0.8F - y * 2.1F) + cos(x * 2.3F + y * 1.4F)) / 3.0F;
    }

    float nebulaCloud(const float x, const float y, const float t) {
        const float n = sNoise(x * 0.8F + t * 0.07F, y * 0.8F - t * 0.05F);
        const float n2 = sNoise(x * 1.6F - t * 0.04F, y * 1.5F + t * 0.06F) * 0.5F;
        const float n3 = sNoise(x * 3.2F + t * 0.03F, -y * 2.8F + t * 0.02F) * 0.25F;
        return (n + n2 + n3 + 1.75F) / 3.5F;
    }

    void drawSpiralNebula() {
        static float rotAngle = 0.0F;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(SpaceDelays::NEBULA_DELAY)) {
            return;
        }
        lastTime = millis();

        rotAngle += 0.012F;

        constexpr float cx = static_cast<float>(StripControl::MATRIX_WIDTH) / 2.0F - 0.5F;
        constexpr float cy = static_cast<float>(StripControl::MATRIX_HEIGHT) / 2.0F - 0.5F;
        const float maxR = sqrt(cx * cx + cy * cy);

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

        for (uint8_t px = 0U; px < StripControl::MATRIX_WIDTH; px++) {
            for (uint8_t py = 0U; py < StripControl::MATRIX_HEIGHT; py++) {
                const float dx = static_cast<float>(px) - cx;
                const float dy = static_cast<float>(py) - cy;
                const float r = sqrt(dx * dx + dy * dy);
                const float rn = r / maxR;

                const float rx = dx * cos(rotAngle) - dy * sin(rotAngle);
                const float ry = dx * sin(rotAngle) + dy * cos(rotAngle);

                const float cloud = nebulaCloud(rx * 0.45F, ry * 0.45F, rotAngle);
                const float ang = atan2(ry, rx);
                float spiral = (sin(2.0F * ang - 0.5F * r + rotAngle) + 1.0F) / 2.0F;
                spiral = powf(spiral, 2.0F) * 0.5F;

                const float core = expf(-r * 0.38F);
                float brightness = cloud * 0.6F + spiral + core;
                const float cornerBoost = 1.0F + rn * 0.3F;
                brightness *= cornerBoost;

                if (brightness > 1.0F) {
                    brightness = 1.0F;
                }
                if (brightness < 0.06F) {
                    continue;
                }

                float cw = expf(-r * 0.8F);
                float aw = spiral * (1.0F - cw);
                float nw = cloud * (1.0F - cw) * (1.0F - spiral * 0.7F);
                float fw = max(0.0F, 1.0F - cw - aw - nw);

                const float wSum = cw + aw + nw + fw + 0.001F;
                cw /= wSum;
                aw /= wSum;
                nw /= wSum;
                fw /= wSum;

                const auto r8 = static_cast<uint8_t>(255.0F * cw + 0.0F * aw + 220.0F * nw + 0.0F * fw);
                const auto g8 = static_cast<uint8_t>(200.0F * cw + 60.0F * aw + 0.0F * nw + 0.0F * fw);
                const auto b8 = static_cast<uint8_t>(80.0F * cw + 255.0F * aw + 140.0F * nw + 120.0F * fw);

                // smoothstep
                brightness = brightness * brightness * (3.0F - 2.0F * brightness);

                const uint32_t idx = Effects::getIndex(px, py);
                StripControl::leds[idx] = CRGB(r8, g8, b8);
                (void) StripControl::leds[idx].nscale8(static_cast<uint8_t>(brightness * 255.0F));
            }
        }
        StripControl::show();
    }

    void drawMoonPhases() {
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(SpaceDelays::MOON_PHASE_DELAY)) {
            return;
        }
        lastTime = millis();

        const float phase = static_cast<float>(millis()) * 0.00015F;

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

        for (uint8_t y = 0U; y < 12U; y++) {
            for (uint8_t x = 0U; x < 12U; x++) {
                constexpr float centerY = 5.5F;
                constexpr float centerX = 5.5F;
                const float dx = static_cast<float>(x) - centerX;
                const float dy = static_cast<float>(y) - centerY;
                const float distSq = dx * dx + dy * dy;

                if (constexpr float radius = 5.6F; distSq <= radius * radius) {
                    const float dz = sqrt(radius * radius - distSq);
                    const float lightX = sin(phase);
                    const float lightZ = cos(phase);

                    float intensity = dx / radius * lightX + dz / radius * lightZ;
                    if (intensity < 0.0F) {
                        intensity = 0.0F;
                    }

                    const uint8_t crater =
                            (static_cast<uint16_t>(x) * 7U + static_cast<uint16_t>(y) * 13U) % 10U < 3U ? 180U : 255U;

                    auto bright = static_cast<uint8_t>(intensity * 255.0F);
                    bright = scale8(bright, crater);

                    auto moonColor = CRGB(200U, 215U, 255U);

                    if (bright < 15U) {
                        StripControl::leds[Effects::getIndex(x, y)] = CRGB(2U, 2U, 5U);
                    } else {
                        StripControl::leds[Effects::getIndex(x, y)] = moonColor.nscale8(bright);
                    }
                }
            }
        }
        StripControl::show();
    }

    void drawJupiter() {
        static float offsetGRS = 0.0F;

        offsetGRS += 0.05F;

        for (uint8_t y = 0U; y < 12U; y++) {
            for (uint8_t x = 0U; x < 12U; x++) {
                const float dx = static_cast<float>(x) - 5.5F;
                const float dy = static_cast<float>(y) - 5.5F;

                if (const float distSq = dx * dx + dy * dy; distSq <= 36.0F) {
                    CRGB color;

                    if (y <= 2U || y >= 9U) {
                        color = CRGB(80U, 60U, 40U);
                    } else if (y == 3U || y == 8U) {
                        color = CRGB(150U, 100U, 50U);
                    } else if (y == 4U || y == 7U) {
                        color = CRGB(100U, 50U, 10U);
                    } else {
                        color = CRGB(180U, 140U, 90U);
                    }

                    if (y == 3U || y == 4U) {
                        const auto grsPos = static_cast<uint32_t>(offsetGRS) % 24U;
                        const auto grsNext = static_cast<uint32_t>(grsPos + 1U);
                        if (const uint32_t grsPrev = (grsPos > 0U) ? static_cast<uint32_t>(grsPos - 1U) : 0U;
                            (y == 3U && (x == grsPos || x == grsNext)) ||
                            (y == 4U && (x == grsPos || x == grsNext || x == grsPrev))) {
                            color = CRGB(180U, 40U, 10U);
                        }
                    }

                    if (distSq > 30.0F) {
                        (void) color.fadeToBlackBy(80U);
                    }
                    if (const uint32_t pix = Effects::getIndex(x, y); pix < 144U) {
                        StripControl::leds[pix] = color;
                    }
                } else {
                    if (const uint32_t pix = Effects::getIndex(x, y); pix < 144U) {
                        StripControl::leds[pix] = CRGB::Black;
                    }
                }
            }
        }
        StripControl::show();
    }

    void drawBlackHole() {
        constexpr float cx = static_cast<float>(StripControl::MATRIX_WIDTH) / 2.0F - 0.5F;
        constexpr float cy = static_cast<float>(StripControl::MATRIX_HEIGHT) / 2.0F - 0.5F;
        static uint8_t angleOffset = 0U;
        static uint8_t hueShift = 0U;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(SpaceDelays::BLACK_HOLE_DELAY)) {
            return;
        }
        lastTime = millis();

        angleOffset++;
        hueShift = static_cast<uint8_t>((static_cast<uint16_t>(hueShift) + 2U) % 255U);

        for (uint8_t y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
            for (uint8_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
                constexpr float maxDist = 7.5F;
                const float dx = static_cast<float>(x) - cx;
                const float dy = static_cast<float>(y) - cy;
                const float dist = sqrt(dx * dx + dy * dy);
                const float ang = atan2(dy, dx) * 57.3F;

                if (dist < 1.5F) {
                    StripControl::leds[Effects::getIndex(x, y)] = CRGB::Black;
                    continue;
                }

                float normDist = (dist - 1.5F) / (maxDist - 1.5F);
                normDist = constrain(normDist, 0.0F, 1.0F);

                float baseBrightness = (1.0F - normDist) * (1.0F - normDist) * 255.0F;
                const auto hue = static_cast<uint8_t>(20.0F - normDist * 15.0F);

                const auto spiral = static_cast<float>(
                        sin(radians(ang * 3.5F - dist * 12.0F + static_cast<float>(angleOffset) * 4.0F)));
                const float spiralAmount = (1.0F - normDist) * 40.0F;
                baseBrightness += spiral * spiralAmount;

                const auto side = static_cast<float>(sin(radians(ang + static_cast<float>(angleOffset) * 1.8F)));
                const float sideAmount = (1.0F - normDist) * 30.0F;
                baseBrightness += side * sideAmount;

                if (normDist > 0.75F) {
                    const float edgeFade = (normDist - 0.75F) / 0.25F;
                    baseBrightness *= 1.0F - edgeFade;
                }

                const auto brightness = static_cast<uint8_t>(constrain(
                        static_cast<int16_t>(baseBrightness), static_cast<int16_t>(0), static_cast<int16_t>(255)));
                const auto sat =
                        static_cast<uint8_t>(constrain(static_cast<uint8_t>(255.0F - normDist * 80.0F),
                                                       static_cast<uint8_t>(175U), static_cast<uint8_t>(255U)));

                StripControl::leds[Effects::getIndex(x, y)] =
                        CHSV(static_cast<uint8_t>(static_cast<uint16_t>(hue) + static_cast<uint16_t>(hueShift) / 8U),
                             sat, brightness);
            }
        }
        StripControl::show();
    }

    void drawAurora() {
        const uint32_t ms = millis();

        for (uint32_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
            const uint8_t curtain1 =
                    inoise8(static_cast<uint16_t>(x * 30U + ms / 50U), static_cast<uint16_t>(ms / 80U));
            const uint8_t curtain2 =
                    inoise8(static_cast<uint16_t>(x * 40U + 500U), static_cast<uint16_t>(ms / 60U + 300U));
            uint8_t curtain = lerp8by8(curtain1, curtain2, 128U);

            curtain = static_cast<uint8_t>(map(static_cast<int16_t>(curtain), 40, 210, 100, 255));
            curtain = static_cast<uint8_t>(constrain(static_cast<uint16_t>(curtain), 0U, 255U));

            const auto hue = static_cast<uint8_t>(
                    85U + scale8(inoise8(static_cast<uint16_t>(x * 20U), static_cast<uint16_t>(ms / 120U)), 60U) - 10U);

            for (uint32_t y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
                auto y_gradient = static_cast<uint8_t>(map(
                        static_cast<int32_t>(y), 0, static_cast<int16_t>(StripControl::MATRIX_HEIGHT - 1U), 255, 0));
                y_gradient = scale8(y_gradient, y_gradient);

                uint8_t rays =
                        inoise8(static_cast<uint16_t>(x * 60U + ms / 70U), static_cast<uint16_t>(y * 40U + ms / 50U));
                rays = static_cast<uint8_t>(map(static_cast<int16_t>(rays), 40, 210, 160, 255));

                uint8_t brightness = scale8(curtain, y_gradient);
                brightness = scale8(brightness, rays);

                const auto localHue = static_cast<uint8_t>(hue - scale8(static_cast<uint8_t>(255U - y_gradient), 25U));

                StripControl::leds[Effects::getIndex(x, y)] = CHSV(localHue, 230U, brightness);
            }
        }

        StripControl::show();
    }

    void drawSinusoidWaves(const uint32_t sinNum) {
        constexpr uint8_t matrixHeightMajor = StripControl::MATRIX_HEIGHT / 2U + StripControl::MATRIX_HEIGHT % 2U;
        constexpr uint8_t matrixWidthMajor = StripControl::MATRIX_WIDTH / 2U + StripControl::MATRIX_WIDTH % 2U;
        constexpr float eS3Speed = 0.004F * 15.0F + 0.015F;
        const float eS3Size = mapStar(255.0F, 1.0F, 255.0F, 3.0F, 9.0F);
        const uint8_t scale = map8(55U, 50U, 150U);
        const auto timeShift = static_cast<float>(millis());
        static uint32_t lastTime = 0U;

        auto calculateColor = [&](const uint8_t x, const uint8_t y, const float freq1, const float freq2,
                                  const float timeMult = 1.0F) -> uint8_t {
            const float cx =
                    static_cast<float>(y) - static_cast<float>(matrixHeightMajor) +
                    eS3Size *
                            static_cast<float>(sin16(static_cast<uint16_t>(eS3Speed * freq1 * timeShift * timeMult))) /
                            32767.0F;
            const float cy =
                    static_cast<float>(x) - static_cast<float>(matrixWidthMajor) +
                    eS3Size *
                            static_cast<float>(cos16(static_cast<uint16_t>(eS3Speed * freq2 * timeShift * timeMult))) /
                            32767.0F;
            return static_cast<uint8_t>(
                    127.0F * (1.0F + static_cast<float>(sin16(static_cast<uint16_t>(127.0F * static_cast<float>(scale) *
                                                                                    sqrt(cx * cx + cy * cy)))) /
                                             32767.0F));
        };

        if (millis() - lastTime < static_cast<uint32_t>(SpaceDelays::WAVES_DELAY)) {
            return;
        }
        lastTime = millis();

        for (uint8_t y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
            for (uint8_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
                switch (sinNum) {
                    case 0U:
                        StripControl::leds[Effects::getIndex(x, y)].r =
                                static_cast<uint8_t>(255U - calculateColor(x, y, 98.301F, 72.0874F));
                        StripControl::leds[Effects::getIndex(x, y)].b =
                                static_cast<uint8_t>(255U - calculateColor(x, y, 134.3447F, 170.3884F));
                        break;

                    case 1U: {
                        CRGB color;
                        color.r = static_cast<uint8_t>(255U - calculateColor(x, y, 98.301F, 72.0874F));
                        color.g = static_cast<uint8_t>(255U - calculateColor(x, y, 68.8107F, 65.534F));
                        color.b = static_cast<uint8_t>(255U - calculateColor(x, y, 134.3447F, 170.3884F));
                        StripControl::leds[Effects::getIndex(x, y)] = color;
                        break;
                    }
                    default:;
                }
            }
        }
        StripControl::show();
    }
} // namespace
