#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (JumpingLightsSettings):
    1) Скорости эффектов
*/

enum JumpingLightsSettings { JUMPING_SQUARE_DELAY = 90, JUMPING_POINTS_DELAY = 60 };

static void drawChaos();
static void drawDriftingLine();
static void drawJumpingCircle();
static void drawJumpingSquare();
static void drawJumpingPoints();

static void initPoints();

static int8_t pos[2][8];
static int8_t dir[8];
static byte hue6;

struct Point {
    int16_t x;
    int16_t y;
    CRGB color;
    int16_t directionX;
    int16_t directionY;
};

static Point points[6];

void drawJumpingLights(const uint8_t subMode) {
    static uint8_t firstStartPoints = 0;
    switch (subMode) {
        case 0:
            drawChaos();
            break;

        case 1:
            drawDriftingLine();
            break;

        case 2:
            drawJumpingCircle();
            break;

        case 3:
            drawJumpingSquare();
            break;

        case 4:
            if (firstStartPoints == 0) {
                initPoints();
                firstStartPoints = 1;
            }
            drawJumpingPoints();
            break;

        default:
            break;
    }
}

void drawPixel(const float x, const float y, const CRGB &color) {
    if (x < 0 || y < 0 || x > static_cast<float>(MATRIX_WIDTH) - 1 || y > static_cast<float>(MATRIX_HEIGHT) - 1)
        return;

    const uint8_t xx = (x - static_cast<int16_t>(x)) * 255;
    const uint8_t yy = (y - static_cast<int16_t>(y)) * 255;
    const uint8_t ix = 255 - xx;
    const uint8_t iy = 255 - yy;

#define WU_WEIGHT(a, b) ((uint8_t) (((a) * (b) + (a) + (b)) >> 8))
    const uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy), WU_WEIGHT(ix, yy), WU_WEIGHT(xx, yy)};

    for (uint8_t i = 0; i < 4; i++) {
        const int16_t xn = x + (i & 1);
        const int16_t yn = y + (i >> 1 & 1);
        CRGB clr = leds[xyToIndex(xn, yn)];
        if (xn < static_cast<int16_t>(MATRIX_WIDTH) - 1 && yn < static_cast<int16_t>(MATRIX_HEIGHT) - 1 && yn > 0 &&
            xn > 0) {
            clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
            clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
            clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
        } else {
            clr.r = qadd8(clr.r, (color.r * 85) >> 8);
            clr.g = qadd8(clr.g, (color.g * 85) >> 8);
            clr.b = qadd8(clr.b, (color.b * 85) >> 8);
        }
        leds[xyToIndex(xn, yn)] = clr;
    }
#undef WU_WEIGHT
}

static void move(const byte id) {
    switch (dir[id]) {
        case 0:
            pos[1][id]++;
            break;
        case 1:
            pos[0][id]++;
            break;
        case 2:
            pos[1][id]--;
            break;
        case 3:
            pos[0][id]--;
            break;
        default:;
    }
}

static void check1(const byte id) {
    if (leds[pos[1][id] * MATRIX_WIDTH + pos[0][id]] == CRGB(0, 0, 0))
        leds[xyToIndex(pos[0][id], pos[1][id])] = CHSV(hue6, 255, 255);
    else
        leds[xyToIndex(pos[0][id], pos[1][id])] = 0;
}

static void check2(const byte id) {
    if (leds[pos[1][id] * MATRIX_WIDTH + pos[0][id]] == CRGB(0, 0, 0)) {
        dir[id]++;
    } else {
        dir[id]--;
    }

    if (dir[id] > 3) {
        dir[id] = 0;
    } else if (dir[id] < 0) {
        dir[id] = 3;
    }
}

static void check3(const byte id) {
    if (pos[0][id] > MATRIX_WIDTH - 1) {
        pos[0][id] = 0;
    }
    if (pos[1][id] > MATRIX_HEIGHT - 1) {
        pos[1][id] = 0;
    }
    if (pos[0][id] < 0) {
        pos[0][id] = MATRIX_WIDTH - 1;
    }
    if (pos[1][id] < 0) {
        pos[1][id] = MATRIX_HEIGHT - 1;
    }
}

static void drawChaos() {
    static bool setUp = true;
    static uint32_t lastTime = 0;

    if (millis() - lastTime < 100)
        return;
    lastTime = millis();

    if (setUp) {
        setUp = false;

        fill_solid(leds, MATRIX_LEDS, CRGB::Black);
        stripShow();

        for (byte i = 0; i < 8; i++) {
            pos[0][i] = ESP8266TrueRandom.random(0, MATRIX_HEIGHT);
            pos[1][i] = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
            dir[i] = ESP8266TrueRandom.random(0, 3);
        }
    }

    for (byte i = 0; i < 8; i++) {
        check1(i);
        move(i);
        check3(i);
        check2(i);
    }

    hue6++;

    EVERY_N_SECONDS(8) { setUp = true; }
    stripShow();
}

static void drawCurve(const float x, const float y, const float x2, const float y2, const float x3, const float y3,
                      const CRGB coll) {
    for (float u = 0.0; u <= 1.0; u += 0.02) {
        const float invU = 1.0 - u;
        const float b0 = invU * invU;
        const float b1 = 2 * invU * u;
        const float b2 = u * u;

        const float xu = b0 * x + b1 * x2 + b2 * x3;
        const float yu = b0 * y + b1 * y2 + b2 * y3;

        drawPixel(xu, yu, coll);
    }
}

static void drawDriftingLine() {
    static byte hue;

    fadeToBlackBy(leds, MATRIX_LEDS, 30);
    const byte x1 = beatsin8(18, 0, MATRIX_WIDTH - 1);
    const byte x2 = beatsin8(23, 0, MATRIX_WIDTH - 1);
    const byte x3 = beatsin8(27, 0, MATRIX_WIDTH - 1);

    const byte y1 = beatsin8(20, 0, MATRIX_HEIGHT - 1);
    const byte y2 = beatsin8(26, 0, MATRIX_HEIGHT - 1);
    const byte y3 = beatsin8(15, 0, MATRIX_HEIGHT - 1);

    drawCurve(x1, y1, x2, y2, x3, y3, CHSV(hue, 255, 255));
    hue++;
    stripShow();
}

static void drawJumpingCircle() {
    static XYMap xyMap(MATRIX_WIDTH, MATRIX_HEIGHT);

    for (byte i = 8; i--;) {
        leds[xyToIndex(beatsin8(12 + i, 0, MATRIX_WIDTH - 1), beatsin8(15 - i, 0, MATRIX_HEIGHT - 1))] =
                CHSV(beatsin8(12, 0, 255), 255, 255);
        blur2d(leds, MATRIX_WIDTH, MATRIX_HEIGHT, 16, xyMap);
    }
    stripShow();
}

static void drawJumpingSquare() {
    static float squareX = 0;
    static float squareY = 0;
    static float directionX = 1.0f;
    static float directionY = 0.95f;
    static CRGB squareColor = CRGB::Red;
    static uint32_t lastTime = 0;

    if (millis() - lastTime < JUMPING_SQUARE_DELAY)
        return;
    lastTime = millis();

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            if (const uint16_t ledIndex = xyToIndex(squareX + i, squareY + j); ledIndex < MATRIX_LEDS) {
                leds[ledIndex] = squareColor;
            }
        }
    }
    stripShow();

    squareX += directionX;
    squareY += directionY;

    if (squareX + 3 >= MATRIX_WIDTH || squareX < 0) {
        directionX = -directionX;
        squareX = max(static_cast<float>(0), min(squareX, static_cast<float>(MATRIX_WIDTH - 3)));
    }
    if (squareY + 3 >= MATRIX_HEIGHT || squareY < 0) {
        directionY = -directionY;
        squareY = max(static_cast<float>(0), min(squareY, static_cast<float>(MATRIX_HEIGHT - 3)));
    }

    static uint8_t frameCount = 0;
    if (++frameCount >= 50) {
        squareColor = CHSV(random8(), 255, 255);
        frameCount = 0;
    }
}

static void initPoints() {
    for (uint8_t i = 0; i < 6; i++) {
        points[i].x = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
        points[i].y = ESP8266TrueRandom.random(0, MATRIX_HEIGHT);
        points[i].color = CHSV(random8(), 255, 255);
        points[i].directionX = ESP8266TrueRandom.random(0, 2) == 0 ? 1 : -1;
        points[i].directionY = ESP8266TrueRandom.random(0, 2) == 0 ? 1 : -1;
    }
}

static void drawJumpingPoints() {
    static uint32_t lastTime = 0;

    if (millis() - lastTime < JUMPING_POINTS_DELAY)
        return;
    lastTime = millis();

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    for (uint8_t i = 0; i < 6; i++) {
        if (const uint16_t ledIndex = xyToIndex(points[i].x, points[i].y); ledIndex < MATRIX_LEDS) {
            leds[ledIndex] = points[i].color;
        }
    }
    stripShow();

    for (uint8_t i = 0; i < 6; i++) {
        points[i].x += points[i].directionX;
        points[i].y += points[i].directionY;

        if (points[i].x >= MATRIX_WIDTH || points[i].x < 0) {
            points[i].directionX = -points[i].directionX;
            if (random8(10) > 7)
                points[i].directionY = random8(2) == 0 ? 1 : -1;

            points[i].x = max(static_cast<int16_t>(0), min(points[i].x, static_cast<int16_t>(MATRIX_WIDTH - 1)));
        }

        if (points[i].y >= MATRIX_HEIGHT || points[i].y < 0) {
            points[i].directionY = -points[i].directionY;
            if (random8(10) > 7)
                points[i].directionX = random8(2) == 0 ? 1 : -1;

            points[i].y = max(static_cast<int16_t>(0), min(points[i].y, static_cast<int16_t>(MATRIX_HEIGHT - 1)));
        }
    }

    for (uint8_t i = 0; i < 6; i++) {
        if (random8(20) == 0) {
            points[i].color = CHSV(random8(), 255, 255);
        }
    }
}
