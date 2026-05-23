#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (SpaceSettings):
    1) Скорости эффектов
*/

enum SpaceSettings {
    STAR_SKY_DELAY = 1500,
    CONSTELLATION_DELAY = 10000,
    PULSATING_STAR_DELAY = 200,
    ECLIPSING_ROTATION_SPEED = 3,
    STARFALL_DELAY = 100,
    GALAXY_DELAY = 100,
    NEBULA_DELAY = 130,
    MOON_PHASE_DELAY = 200,
    BLACK_HOLE_DELAY = 100,
    WAVES_DELAY = 20
};

struct Star {
    uint8_t x;
    uint8_t y;
    uint8_t bright;
    CRGB color;
};

struct Constellation {
    Star stars[15];
    uint8_t count;
};

const uint32_t starColors[] = {0x95A7E8, 0x4A5374, 0x1A1E2A, 0xE0E8FF, 0x70747F, 0x202226, 0xFF9F13,
                               0x7F4F09, 0x2F1D03, 0xFC7F14, 0x7E3F0A, 0x2E1704, 0xF25F6A, 0x792F35,
                               0x2D1114, 0x3870FF, 0x1C387F, 0x0A142F, 0xAA96DC, 0x554B6E, 0x1F1B29};

constexpr uint8_t starColorsCount = std::size(starColors);

static Constellation coordinates[] = {
        {{{11, 3, 180}, {7, 0, 255}, {5, 5, 200}, {2, 6, 255}, {0, 11, 180}}, 5}, // Кассиопея
        {{{0, 6, 190}, {2, 8, 180}, {2, 9, 10}, {4, 7, 220}, {6, 7, 70}, {7, 4, 180}, {10, 5, 210}, {11, 9, 255}},
         8}, // Большая Медведица
        {{{0, 2, 100},
          {2, 4, 150},
          {3, 4, 190},
          {3, 11, 30},
          {4, 4, 150},
          {5, 0, 255},
          {5, 11, 10},
          {7, 8, 100},
          {11, 8, 30},
          {11, 6, 30},
          {10, 4, 100},
          {9, 3, 30},
          {8, 3, 30}},
         13}, // Орион
        {{{4, 5, 100}, {5, 8, 100}, {6, 11, 255}, {7, 0, 110}, {8, 3, 110}}, 5}, // Лира
        {{{6, 10, 255}, {3, 5, 210}, {9, 5, 210}, {6, 0, 240}, {7, 3, 130}},
         5}, // Южный Крест (инверсия, положение относительно экватора)
        {{{1, 4, 160}, {3, 0, 200}, {6, 1, 255}, {9, 5, 200}, {8, 10, 160}}, 5}, // Северная Корона
};

static uint16_t currentStarCount = 0;

static void drawStarSky();
static void drawConstellations();
static void drawStarFall();
static void drawPulsatingStar();
static void drawEclipsingBinaryStars();
static void drawSpiralNebula();
static void drawSpiralGalaxy();
static void drawMoonPhases();
static void drawJupiter();
static void drawBlackHole();
static void drawSinusoidWaves(uint8_t sinNum);
static void drawAurora();

void drawSpaceEffects(const uint8_t subMode) {
    switch (subMode) {
        case 0:
            drawStarSky();
            break;

        case 1:
            drawConstellations();
            break;

        case 2:
            drawPulsatingStar();
            break;

        case 3:
            drawEclipsingBinaryStars();
            break;

        case 4:
            drawStarFall();
            break;

        case 5:
            drawSpiralNebula();
            break;

        case 6:
            drawSpiralGalaxy();
            break;

        case 7:
            drawMoonPhases();
            break;

        case 8:
            drawJupiter();
            break;

        case 9:
            drawBlackHole();
            break;

        case 10:
            drawAurora();
            break;

        case 11:
            drawSinusoidWaves(0); // magneticWaves
            break;

        case 12:
            drawSinusoidWaves(1); // rayInterference
            break;

        default:
            break;
    }
}

static void addStar(const uint16_t starNum, const uint32_t color) {
    if (starNum < MATRIX_LEDS) {
        if (leds[starNum].r == 0 && leds[starNum].g == 0 && leds[starNum].b == 0) {
            currentStarCount++;
        }
        leds[starNum] = CRGB(color);
    }
}

static void removeStar(const uint16_t starNum) {
    if (starNum < MATRIX_LEDS) {
        if (leds[starNum].r != 0 || leds[starNum].g != 0 || leds[starNum].b != 0) {
            if (currentStarCount > 0)
                currentStarCount--;
        }
        leds[starNum] = CRGB::Black;
    }
}

void drawStarSky() {
    static uint8_t step = 0;
    static uint32_t lastTime = 0;
    const uint32_t currentMillis = millis();
    const uint8_t pulse = beatsin8(15, 120, 255);
    const uint8_t flicker = random8(200, 255);
    const uint8_t brightnessScale = scale8(pulse, flicker);

    CRGB saved[MATRIX_LEDS];
    memcpy(saved, leds, sizeof(leds));

    for (uint16_t i = 0; i < MATRIX_LEDS; i++) {
        leds[i].nscale8(brightnessScale);
    }

    stripShow();
    memcpy(leds, saved, sizeof(leds));
    delay(10);

    if (checkCommandReceived()) {
        const uint16_t rndLed = ESP8266TrueRandom.random(0, MATRIX_LEDS);
        const uint8_t rndColor = ESP8266TrueRandom.random(0, starColorsCount);
        addStar(rndLed, starColors[rndColor]);
    }

    if (currentMillis - lastTime < STAR_SKY_DELAY)
        return;
    lastTime = currentMillis;

    const bool overPopulated = currentStarCount >= MATRIX_LEDS / 6;

    if (step < 2) {
        if (!overPopulated) {
            const uint16_t rndLed = ESP8266TrueRandom.random(0, MATRIX_LEDS);
            const uint8_t rndColor = ESP8266TrueRandom.random(0, starColorsCount);
            addStar(rndLed, starColors[rndColor]);
        }
        step++;
    } else if (step >= 2 && step < 10) {
        const uint16_t rndLed = ESP8266TrueRandom.random(0, MATRIX_LEDS);
        removeStar(rndLed);
        step++;
    } else {
        step = 0;
    }
}

void drawConstellations() {
    static uint8_t currentID = 0;
    static uint32_t lastSwitch = millis();
    static uint32_t lastTime = 0;
    const uint32_t ms = millis();

    if (ms - lastTime < 100)
        return;
    lastTime = ms;

    if (ms - lastSwitch > CONSTELLATION_DELAY) {
        lastSwitch = ms;
        currentID = (currentID + 1) % 6;
    }

    Constellation &c = coordinates[currentID];
    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    const uint8_t pulse = beatsin8(12, 160, 255);

    for (uint8_t i = 0; i < c.count; i++) {
        const uint16_t idx = xyToIndex(c.stars[i].x, c.stars[i].y);

        const uint8_t flicker = random8(190, 255);
        uint8_t finalBright = scale8(c.stars[i].bright, pulse);
        finalBright = scale8(finalBright, flicker);

        leds[idx] = CRGB(200, 225, 255);
        leds[idx].nscale8(finalBright);
    }

    stripShow();
}

static void drawVortex(const int16_t currentRadius) {
    static constexpr int16_t maxRadius = min((MATRIX_WIDTH - 1) / 2, (MATRIX_HEIGHT - 1) / 2);
    constexpr float centerX = (MATRIX_WIDTH - 1) / 2.0f;
    constexpr float centerY = (MATRIX_HEIGHT - 1) / 2.0f;
    static float rotationAngle = 0;

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    for (int16_t r = 0; r <= currentRadius; r++) {
        const float progress = static_cast<float>(r) / maxRadius;
        const uint8_t red = 255 * progress;
        const uint8_t green = 255 * progress;
        const uint8_t blue = 255 * (1.0 - progress);

        const CRGB color(red, green, blue);

        for (int16_t angle = 0; angle < 360; angle += 2) {
            const float rad = radians(angle + rotationAngle);

            const int16_t x = round(centerX + r * cos(rad));
            const int16_t y = round(centerY + r * sin(rad));

            if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
                leds[xyToIndex(x, y)] = color;
            }
        }
    }

    if (currentRadius == 4) {
        leds[126] = CRGB(0x000000);
        leds[82] = CRGB(0x000000);
    } else if (currentRadius == 2) {
        leds[102] = CRGB(0x000000);
        leds[80] = CRGB(0x000000);
    }

    stripShow();

    rotationAngle += 2.5f;
    if (rotationAngle >= 360)
        rotationAngle -= 360;
}

static void drawPulsatingStar() {
    static constexpr int16_t maxRadius = min((MATRIX_WIDTH - 1) / 2, (MATRIX_HEIGHT - 1) / 2);
    static int16_t currentRadius = 0;
    static int8_t direction = 1;
    static uint32_t lastUpdateTime = 0;

    if (const uint32_t currentMillis = millis(); currentMillis - lastUpdateTime >= PULSATING_STAR_DELAY) {
        lastUpdateTime = currentMillis;
        drawVortex(currentRadius);
        currentRadius += direction;

        if (currentRadius >= maxRadius) {
            direction = -1;
        } else if (currentRadius <= 0) {
            direction = 1;
        }
    }
}

static float mapStar(const float x, const float inMin, const float inMax, const float outMin, const float outMax) {
    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

static void drawStar(const float x, const float y, const float r, const CRGB color) {
    for (int8_t ix = static_cast<int>(x) - 4; ix <= static_cast<int>(x) + 4; ix++) {
        for (int8_t iy = static_cast<int>(y) - 4; iy <= static_cast<int>(y) + 4; iy++) {
            if (ix >= 0 && ix < MATRIX_WIDTH && iy >= 0 && iy < MATRIX_HEIGHT) {

                if (const float dist = sqrt((ix - x) * (ix - x) + (iy - y) * (iy - y)); dist < r) {
                    const uint16_t idx = xyToIndex(ix, iy);
                    const float centerFactor = 1.0 - dist / r;
                    const CRGB finalColor = blend(color, CRGB::White, pow(centerFactor, 2) * 255);

                    uint8_t alpha = 255;
                    if (constexpr float edgeSoftness = 0.8; dist > r - edgeSoftness) {
                        alpha = mapStar(dist, r - edgeSoftness, r, 255, 0);
                    }

                    leds[idx] = finalColor;
                    leds[idx].nscale8(alpha);
                }
            }
        }
    }
}

static void drawEclipsingBinaryStars() {
    static float angle = 0;
    static uint32_t lastTime = 0;
    const uint32_t now = millis();
    angle += static_cast<float>(ECLIPSING_ROTATION_SPEED) / 1000.0 * (now - lastTime);
    lastTime = now;

    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
        leds[i] = CRGB::Black;

    constexpr float centerX = 5.5;
    constexpr float centerY = 5.5;
    constexpr float orbitRadius = 4.0;

    const float xA = centerX + cos(angle) * orbitRadius;
    const float zA = sin(angle);
    const float xB = centerX + cos(angle + PI) * orbitRadius;
    const float zB = sin(angle + PI);

    const float sizeA = mapStar(zA, -1.0, 1.0, 1.0, 3.0);
    const float sizeB = mapStar(zB, -1.0, 1.0, 1.0, 3.0);

    if (zA < zB) {
        drawStar(xA, centerY, sizeA, CRGB(255, 60, 0));
        drawStar(xB, centerY, sizeB, CRGB(0, 50, 255));
    } else {
        drawStar(xB, centerY, sizeB, CRGB(0, 50, 255));
        drawStar(xA, centerY, sizeA, CRGB(255, 60, 0));
    }

    stripShow();
}

void drawStarFall() {
    static uint8_t currentCol[4] = {11, 7, 3, 0};
    static uint8_t currentRow[4] = {11, 7, 3, 0};
    constexpr uint8_t lineLength = 7;
    static uint32_t lastTime = 0;

    auto drawLine = [](const uint8_t col, const uint8_t row, const uint8_t length, const bool clear) {
        for (uint8_t j = 0; j < length; ++j) {
            if (j > row || j > col)
                continue;
            const uint8_t rowIndex = row - j;
            if (const uint8_t colIndex = col - j; rowIndex < MATRIX_HEIGHT && colIndex < MATRIX_WIDTH) {
                const uint16_t pixelIndex = xyToIndex(colIndex, rowIndex);
                if (clear) {
                    leds[pixelIndex] = CRGB::Black;
                } else {
                    const uint8_t brightness = j == length - 1 ? 255 : 5 * j;
                    leds[pixelIndex] = hsv2rgb_spectrum(CHSV(0, 0, brightness));
                }
            }
        }
    };

    if (millis() - lastTime < STARFALL_DELAY)
        return;
    lastTime = millis();

    for (uint8_t i = 0; i < 4; ++i) {
        drawLine(currentCol[i], currentRow[i], lineLength, false);
    }

    stripShow();

    for (uint8_t i = 0; i < 4; ++i) {
        drawLine(currentCol[i], currentRow[i], lineLength, true);
    }

    for (uint8_t i = 0; i < 4; ++i) {
        if (currentRow[i] > 0) {
            --currentRow[i];
            --currentCol[i];
        } else {
            currentCol[i] = MATRIX_WIDTH + 4 + ESP8266TrueRandom.random(0, 10);
            currentRow[i] = MATRIX_WIDTH + 6 + ESP8266TrueRandom.random(0, 10);
        }
    }
}

static float noiseField(const float x, const float y, const float t) {
    float n = sin(x * 1.7 + t * 0.3) * cos(y * 1.3 - t * 0.2);
    n += sin(x * 3.1 - y * 2.4 + t * 0.15) * 0.5;
    n += cos(x * 0.9 + y * 2.1 + t * 0.4) * 0.3;
    return (n / 1.8 + 1.0) / 2.0;
}

static void drawSpiralGalaxy() {
    static float rotationOffset = 0;
    static uint32_t lastTime = 0;

    if (millis() - lastTime < GALAXY_DELAY)
        return;
    lastTime = millis();

    rotationOffset += 0.008;

    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
        leds[i] = CRGB::Black;

    for (int8_t y = 0; y < 12; y++) {
        for (int8_t x = 0; x < 12; x++) {
            constexpr uint8_t numArms = 2;
            constexpr float tightness = 0.75;
            constexpr float centerY = 5.5;
            constexpr float centerX = 5.5;
            const float dx = x - centerX;
            const float dy = y - centerY;
            const float r = sqrt(dx * dx + dy * dy);
            const float angle = atan2(dy, dx);
            const float spiral = (sin(numArms * angle - tightness * r + rotationOffset) + 1.0) / 2.0;
            const float armVal = pow(spiral, 4);
            const float bulge = exp(-r * 0.75) * 1.8;
            const float nebulaNoise = noiseField(dx * 0.6, dy * 0.6, rotationOffset);
            const float cloudLayer = noiseField(dx * 1.2 + 5.0, dy * 1.1, rotationOffset * 0.7);
            const float nebulaBoost = nebulaNoise * 0.4 * armVal + cloudLayer * 0.15;
            float finalBright = armVal * 0.6 + bulge + nebulaBoost;

            if (finalBright > 1.0)
                finalBright = 1.0;

            constexpr float edge = 6.0;
            const float mask = r < edge ? max(0.0f, 1.0f - r / edge * (r / edge)) : 0.0f;
            finalBright *= mask;

            if (finalBright > 0.03) {
                constexpr CRGB coreColor = CRGB(255, 220, 160); // тёплое ядро
                constexpr CRGB armColor = CRGB(60, 80, 220); // синие рукава
                constexpr CRGB nebulaColor = CRGB(160, 30, 120); // пурпурная эмиссия
                constexpr CRGB coldColor = CRGB(10, 10, 80); // холодный фон галактики

                const float coreWeight = exp(-r * 1.2);
                const float armWeight = armVal * (1.0 - coreWeight);
                const float nebulaWeight = nebulaNoise * 0.3 * (1.0 - coreWeight);
                const float coldWeight = max(0.0f, 1.0f - coreWeight - armWeight - nebulaWeight);

                const CRGB color = CRGB(coreColor.r * coreWeight + armColor.r * armWeight +
                                                nebulaColor.r * nebulaWeight + coldColor.r * coldWeight,

                                        coreColor.g * coreWeight + armColor.g * armWeight +
                                                nebulaColor.g * nebulaWeight + coldColor.g * coldWeight,

                                        coreColor.b * coreWeight + armColor.b * armWeight +
                                                nebulaColor.b * nebulaWeight + coldColor.b * coldWeight);

                const uint8_t grain = random8(220, 255);
                const uint16_t idx = xyToIndex(x, y);
                leds[idx] = color;
                leds[idx].nscale8(static_cast<uint8_t>(finalBright * 255));
                leds[idx].nscale8(grain);
            }
        }
    }

    stripShow();
}

static float sNoise(const float x, const float y) {
    return (sin(x * 1.7 + y * 0.9) + sin(x * 0.8 - y * 2.1) + cos(x * 2.3 + y * 1.4)) / 3.0;
}

static float nebulaCloud(const float x, const float y, const float t) {
    const float n = sNoise(x * 0.8 + t * 0.07, y * 0.8 - t * 0.05);
    const float n2 = sNoise(x * 1.6 - t * 0.04, y * 1.5 + t * 0.06) * 0.5;
    const float n3 = sNoise(x * 3.2 + t * 0.03, -y * 2.8 + t * 0.02) * 0.25;
    return (n + n2 + n3 + 1.75) / 3.5;
}

void drawSpiralNebula() {
    static float rotAngle = 0;
    static uint32_t lastTime = 0;

    if (millis() - lastTime < NEBULA_DELAY)
        return;
    lastTime = millis();

    rotAngle += 0.012;

    constexpr float cx = MATRIX_WIDTH / 2.0 - 0.5;
    constexpr float cy = MATRIX_HEIGHT / 2.0 - 0.5;
    const float maxR = sqrt(cx * cx + cy * cy);

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    for (uint8_t px = 0; px < MATRIX_WIDTH; px++) {
        for (uint8_t py = 0; py < MATRIX_HEIGHT; py++) {
            const float dx = px - cx;
            const float dy = py - cy;
            const float r = sqrt(dx * dx + dy * dy);
            const float rn = r / maxR;

            const float rx = dx * cos(rotAngle) - dy * sin(rotAngle);
            const float ry = dx * sin(rotAngle) + dy * cos(rotAngle);

            const float cloud = nebulaCloud(rx * 0.45, ry * 0.45, rotAngle);
            const float angle = atan2(ry, rx);
            float spiral = (sin(2.0 * angle - 0.5 * r + rotAngle) + 1.0) / 2.0;
            spiral = pow(spiral, 2) * 0.5;

            const float core = exp(-r * 0.38);
            float brightness = cloud * 0.6 + spiral + core;
            const float cornerBoost = 1.0 + rn * 0.3;
            brightness *= cornerBoost;

            if (brightness > 1.0)
                brightness = 1.0;
            if (brightness < 0.06)
                continue;

            float cw = exp(-r * 0.8);
            float aw = spiral * (1.0 - cw);
            float nw = cloud * (1.0 - cw) * (1.0 - spiral * 0.7);
            float fw = max(0.0f, 1.0f - cw - aw - nw);

            const float wSum = cw + aw + nw + fw + 0.001f;
            cw /= wSum;
            aw /= wSum;
            nw /= wSum;
            fw /= wSum;

            const uint8_t r8 = static_cast<uint8_t>(255 * cw + 0 * aw + 220 * nw + 0 * fw);
            const uint8_t g8 = static_cast<uint8_t>(200 * cw + 60 * aw + 0 * nw + 0 * fw);
            const uint8_t b8 = static_cast<uint8_t>(80 * cw + 255 * aw + 140 * nw + 120 * fw);

            brightness = brightness * brightness * (3.0 - 2.0 * brightness);

            const uint16_t idx = xyToIndex(px, py);
            leds[idx] = CRGB(r8, g8, b8);
            leds[idx].nscale8(static_cast<uint8_t>(brightness * 255));
        }
    }
    stripShow();
}

static void drawMoonPhases() {
    static uint32_t lastTime = 0;

    if (millis() - lastTime < MOON_PHASE_DELAY)
        return;
    lastTime = millis();

    const uint32_t ms = millis();
    const float phase = ms * 0.00015;

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    for (uint8_t y = 0; y < 12; y++) {
        for (uint8_t x = 0; x < 12; x++) {
            constexpr float centerY = 5.5;
            constexpr float centerX = 5.5;
            const float dx = x - centerX;
            const float dy = y - centerY;
            const float distSq = dx * dx + dy * dy;

            if (constexpr float radius = 5.6; distSq <= radius * radius) {
                const float dz = sqrt(radius * radius - distSq);
                const float lightX = sin(phase);
                const float lightZ = cos(phase);

                float intensity = dx / radius * lightX + dz / radius * lightZ;

                if (intensity < 0)
                    intensity = 0;

                const uint8_t crater = (x * 7 + y * 13) % 10 < 3 ? 180 : 255;

                uint8_t bright = intensity * 255;
                bright = scale8(bright, crater);

                CRGB moonColor = CRGB(200, 215, 255); // лунный цвет

                if (bright < 15) {
                    leds[xyToIndex(x, y)] = CRGB(2, 2, 5);
                } else {
                    leds[xyToIndex(x, y)] = moonColor.nscale8(bright);
                }
            }
        }
    }
    stripShow();
}

static void drawJupiter() {
    static float offsetGRS = 0;

    offsetGRS += 0.05;

    for (uint8_t y = 0; y < 12; y++) {
        for (uint8_t x = 0; x < 12; x++) {
            const float dx = x - 5.5;
            const float dy = y - 5.5;

            if (const float distSq = dx * dx + dy * dy; distSq <= 36.0) {
                CRGB color;

                if (y <= 2 || y >= 9)
                    color = CRGB(80, 60, 40);
                else if (y == 3 || y == 8)
                    color = CRGB(150, 100, 50);
                else if (y == 4 || y == 7)
                    color = CRGB(100, 50, 10);
                else
                    color = CRGB(180, 140, 90);

                if (y == 3 || y == 4) {
                    if (const uint8_t grsPos = static_cast<uint8_t>(offsetGRS) % 24;
                        (y == 3 && (x == grsPos || x == grsPos + 1)) ||
                        (y == 4 && (x == grsPos || x == grsPos + 1 || x == grsPos - 1))) {
                        color = CRGB(180, 40, 10);
                    }
                }

                if (distSq > 30.0)
                    color.fadeToBlackBy(80);
                if (const uint16_t pix = xyToIndex(x, y); pix < 144)
                    leds[pix] = color;
            } else {
                if (const uint16_t pix = xyToIndex(x, y); pix < 144)
                    leds[pix] = CRGB::Black;
            }
        }
    }
    stripShow();
}

static void drawBlackHole() {
    constexpr float cx = MATRIX_WIDTH / 2.0 - 0.5;
    constexpr float cy = MATRIX_HEIGHT / 2.0 - 0.5;
    static uint8_t angleOffset = 0;
    static uint8_t hueShift = 0;
    static uint32_t lastTime = 0;

    if (millis() - lastTime < BLACK_HOLE_DELAY)
        return;
    lastTime = millis();

    angleOffset++;
    hueShift = (hueShift + 2) % 255;

    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
            constexpr float maxDist = 7.5;
            const float dx = x - cx;
            const float dy = y - cy;
            const float dist = sqrt(dx * dx + dy * dy);
            const float angle = atan2(dy, dx) * 57.3;

            if (dist < 1.5) {
                leds[xyToIndex(x, y)] = CRGB::Black;
                continue;
            }

            float normDist = (dist - 1.5) / (maxDist - 1.5);
            normDist = constrain(normDist, 0.0, 1.0);

            float baseBrightness = (1.0 - normDist) * (1.0 - normDist) * 255.0;
            const uint8_t hue = static_cast<uint8_t>(20.0 - normDist * 15.0);

            const float spiral = sin(radians(angle * 3.5 - dist * 12.0 + angleOffset * 4.0));
            const float spiralAmount = (1.0 - normDist) * 40.0;
            baseBrightness += spiral * spiralAmount;

            const float side = sin(radians(angle + angleOffset * 1.8));
            const float sideAmount = (1.0 - normDist) * 30.0;
            baseBrightness += side * sideAmount;

            if (normDist > 0.75) {
                const float edgeFade = (normDist - 0.75) / 0.25;
                baseBrightness *= 1.0 - edgeFade;
            }

            const uint8_t brightness = constrain((int) baseBrightness, 0, 255);
            const uint8_t sat = constrain((uint8_t) (255 - normDist * 80), 175, 255);

            leds[xyToIndex(x, y)] = CHSV(hue + hueShift / 8, sat, brightness);
        }
    }
    stripShow();
}

static void drawAurora() {
    static uint32_t ms = 0;
    ms = millis();

    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        const uint8_t curtain1 = inoise8(x * 30 + ms / 50, ms / 80);
        const uint8_t curtain2 = inoise8(x * 40 + 500, ms / 60 + 300);
        uint8_t curtain = lerp8by8(curtain1, curtain2, 128);

        curtain = map(curtain, 40, 210, 100, 255);
        curtain = constrain(curtain, 0, 255);

        const uint8_t hue = 85 + scale8(inoise8(x * 20, ms / 120), 60) - 10;

        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
            uint8_t y_gradient = map(y, 0, MATRIX_HEIGHT - 1, 255, 0);
            y_gradient = scale8(y_gradient, y_gradient);

            uint8_t rays = inoise8(x * 60 + ms / 70, y * 40 + ms / 50);
            rays = map(rays, 40, 210, 160, 255);

            uint8_t brightness = scale8(curtain, y_gradient);
            brightness = scale8(brightness, rays);

            const uint8_t localHue = hue - scale8(255 - y_gradient, 25);

            leds[xyToIndex(x, y)] = CHSV(localHue, 230, brightness);
        }
    }

    stripShow();
}

static void drawSinusoidWaves(const uint8_t sinNum) {
    constexpr uint8_t matrixHeightMajor = MATRIX_HEIGHT / 2 + MATRIX_HEIGHT % 2;
    constexpr uint8_t matrixWidthMajor = MATRIX_WIDTH / 2 + MATRIX_WIDTH % 2;
    constexpr float eS3Speed = 0.004 * 15 + 0.015;
    const float eS3Size = mapStar(255, 1, 255, 3, 9);
    const uint8_t scale = map8(55, 50, 150);
    const float timeShift = millis();
    static uint32_t lastTime = 0;

    auto calculateColor = [&](uint8_t const x, uint8_t const y, float const freq1, float const freq2,
                              const float timeMult = 1.0f) -> uint8_t {
        const float cx = y - matrixHeightMajor + eS3Size * sin16(eS3Speed * freq1 * timeShift * timeMult) / 32767.0;
        const float cy = x - matrixWidthMajor + eS3Size * cos16(eS3Speed * freq2 * timeShift * timeMult) / 32767.0;
        return static_cast<uint8_t>(127 * (1 + sin16(127 * scale * sqrt(cx * cx + cy * cy)) / 32767.0));
    };

    if (millis() - lastTime < (sinNum == 0 ? WAVES_DELAY : WAVES_DELAY))
        return;
    lastTime = millis();

    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
            switch (sinNum) {
                case 0:
                    leds[xyToIndex(x, y)].r = 255 - calculateColor(x, y, 98.301, 72.0874);
                    leds[xyToIndex(x, y)].b = 255 - calculateColor(x, y, 134.3447, 170.3884);
                    break;

                case 1: {
                    CRGB color;
                    color.r = 255 - calculateColor(x, y, 98.301, 72.0874);
                    color.g = 255 - calculateColor(x, y, 68.8107, 65.534);
                    color.b = 255 - calculateColor(x, y, 134.3447, 170.3884);
                    leds[xyToIndex(x, y)] = color;
                    break;
                }
                default:
                    break;
            }
        }
    }
    stripShow();
}
