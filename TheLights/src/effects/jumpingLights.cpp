#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (JumpingLightsDelays):
    1) Скорости эффектов
*/

namespace {
    enum class JumpingLightsDelays : uint16_t { JUMPING_SQUARE_DELAY = 90U, JUMPING_POINTS_DELAY = 60U };

    void drawChaos();
    void drawDriftingLine();
    void drawJumpingCircle();
    void drawJumpingSquare();
    void drawJumpingPoints();

    void initPoints();

    int8_t pos[2][8];
    int8_t dir[8];
    uint8_t hue6;

    struct Point {
        int16_t x{0};
        int16_t y{0};
        CRGB color{CRGB::Black};
        int16_t directionX{0};
        int16_t directionY{0};
    };

    Point points[6];
} // namespace

namespace Effects {
    void drawJumpingLights(const uint32_t subMode) {
        static uint8_t firstStartPoints = 0U;
        switch (subMode) {
            case 0U:
                drawChaos();
                break;

            case 1U:
                drawDriftingLine();
                break;

            case 2U:
                drawJumpingCircle();
                break;

            case 3U:
                drawJumpingSquare();
                break;

            case 4U:
                if (firstStartPoints == 0U) {
                    initPoints();
                    firstStartPoints = 1U;
                }
                drawJumpingPoints();
                break;

            default:
                break;
        }
    }

    void drawPixel(const float x, const float y, const CRGB &color) {
        if (x < 0.0F || y < 0.0F || x >= static_cast<float>(StripControl::MATRIX_WIDTH) ||
            y >= static_cast<float>(StripControl::MATRIX_HEIGHT)) {
            return;
        }

        const auto x_int = static_cast<int16_t>(x);
        const auto y_int = static_cast<int16_t>(y);

        const auto xx = static_cast<uint8_t>((x - static_cast<float>(x_int)) * 255.0F);
        const auto yy = static_cast<uint8_t>((y - static_cast<float>(y_int)) * 255.0F);
        const auto ix = static_cast<uint8_t>(255U - static_cast<uint16_t>(xx));
        const auto iy = static_cast<uint8_t>(255U - static_cast<uint16_t>(yy));

        auto wuWeight = [](const uint8_t a, const uint8_t b) -> uint8_t {
            const auto av = static_cast<uint16_t>(a);
            const auto bv = static_cast<uint16_t>(b);
            return static_cast<uint8_t>((av * bv + av + bv) >> 8U);
        };

        const uint8_t wu[4] = {wuWeight(ix, iy), wuWeight(xx, iy), wuWeight(ix, yy), wuWeight(xx, yy)};

        for (uint8_t i = 0U; i < 4U; i++) {
            const auto xn = static_cast<int16_t>(static_cast<int16_t>(x_int) + static_cast<int16_t>(i & 1U));
            const auto yn = static_cast<int16_t>(static_cast<int16_t>(y_int) + static_cast<int16_t>((i >> 1U) & 1U));

            if (xn < 0 || xn >= static_cast<int16_t>(StripControl::MATRIX_WIDTH) || yn < 0 ||
                yn >= static_cast<int16_t>(StripControl::MATRIX_HEIGHT)) {
                continue;
            }

            const auto ledIndex = static_cast<uint16_t>(getIndex(static_cast<uint32_t>(xn), static_cast<uint32_t>(yn)));
            CRGB clr = StripControl::leds[ledIndex];

            if (xn < static_cast<int16_t>(StripControl::MATRIX_WIDTH - 1U) &&
                yn < static_cast<int16_t>(StripControl::MATRIX_HEIGHT - 1U) && yn > 0 && xn > 0) {
                clr.r = qadd8(clr.r, static_cast<uint8_t>(
                                             (static_cast<uint16_t>(color.r) * static_cast<uint16_t>(wu[i])) >> 8U));
                clr.g = qadd8(clr.g, static_cast<uint8_t>(
                                             (static_cast<uint16_t>(color.g) * static_cast<uint16_t>(wu[i])) >> 8U));
                clr.b = qadd8(clr.b, static_cast<uint8_t>(
                                             (static_cast<uint16_t>(color.b) * static_cast<uint16_t>(wu[i])) >> 8U));
            } else {
                clr.r = qadd8(clr.r, static_cast<uint8_t>((static_cast<uint16_t>(color.r) * 85U) >> 8U));
                clr.g = qadd8(clr.g, static_cast<uint8_t>((static_cast<uint16_t>(color.g) * 85U) >> 8U));
                clr.b = qadd8(clr.b, static_cast<uint8_t>((static_cast<uint16_t>(color.b) * 85U) >> 8U));
            }

            StripControl::leds[ledIndex] = clr;
        }
    }
} // namespace Effects

namespace {
    void move(const uint8_t id) {
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
            default:
                break;
        }
    }

    void check1(const uint8_t id) {
        const auto idx = static_cast<uint16_t>(static_cast<int16_t>(pos[1][id]) *
                                                       static_cast<int16_t>(StripControl::MATRIX_WIDTH) +
                                               static_cast<int16_t>(pos[0][id]));

        if (StripControl::leds[idx] == CRGB(0U, 0U, 0U)) {
            StripControl::leds[Effects::getIndex(static_cast<uint16_t>(pos[0][id]),
                                                 static_cast<uint16_t>(pos[1][id]))] = CHSV(hue6, 255U, 255U);
        } else {
            StripControl::leds[Effects::getIndex(static_cast<uint16_t>(pos[0][id]),
                                                 static_cast<uint16_t>(pos[1][id]))] = CRGB::Black;
        }
    }

    void check2(const uint8_t id) {
        const auto idx = static_cast<uint16_t>(static_cast<int16_t>(pos[1][id]) *
                                                       static_cast<int16_t>(StripControl::MATRIX_WIDTH) +
                                               static_cast<int16_t>(pos[0][id]));

        if (StripControl::leds[idx] == CRGB(0U, 0U, 0U)) {
            dir[id]++;
        } else {
            dir[id]--;
        }

        if (dir[id] > 3) {
            dir[id] = 0;
        } else if (dir[id] < 0) {
            dir[id] = 3;
        } else {
            // dir[id] already in range, no action needed
        }
    }

    void check3(const uint8_t id) {
        if (pos[0][id] > static_cast<int8_t>(StripControl::MATRIX_WIDTH - 1U)) {
            pos[0][id] = 0;
        }
        if (pos[1][id] > static_cast<int8_t>(StripControl::MATRIX_HEIGHT - 1U)) {
            pos[1][id] = 0;
        }
        if (pos[0][id] < 0) {
            pos[0][id] = static_cast<int8_t>(StripControl::MATRIX_WIDTH - 1U);
        }
        if (pos[1][id] < 0) {
            pos[1][id] = static_cast<int8_t>(StripControl::MATRIX_HEIGHT - 1U);
        }
    }

    void drawChaos() {
        static bool setUp = true;
        static uint32_t lastTime = 0U;

        if ((millis() - lastTime) < 100U) {
            return;
        }
        lastTime = millis();

        if (setUp) {
            setUp = false;

            fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
            StripControl::show();

            for (uint8_t i = 0U; i < 8U; i++) {
                pos[0][i] = static_cast<int8_t>(
                        ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_HEIGHT)));
                pos[1][i] = static_cast<int8_t>(
                        ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_WIDTH)));
                dir[i] = static_cast<int8_t>(ESP8266TrueRandom.random(0, 3));
            }
        }

        for (uint8_t i = 0U; i < 8U; i++) {
            check1(i);
            move(i);
            check3(i);
            check2(i);
        }

        hue6++;

        EVERY_N_SECONDS(8U) { setUp = true; }
        StripControl::show();
    }

    void drawCurve(const float x, const float y, const float x2, const float y2, const float x3, const float y3,
                   const CRGB coll) {
        constexpr uint32_t STEPS = 50U;
        for (uint32_t step = 0U; step <= STEPS; step++) {
            const float u = static_cast<float>(step) / static_cast<float>(STEPS);
            const float invU = 1.0F - u;
            const float b0 = invU * invU;
            const float b1 = 2.0F * invU * u;
            const float b2 = u * u;

            const float xu = b0 * x + b1 * x2 + b2 * x3;
            const float yu = b0 * y + b1 * y2 + b2 * y3;

            Effects::drawPixel(xu, yu, coll);
        }
    }

    void drawDriftingLine() {
        static uint8_t hue = 0U;

        fadeToBlackBy(&StripControl::leds[0], static_cast<uint16_t>(StripControl::MATRIX_LEDS), 30U);
        const uint8_t x1 = beatsin8(18U, 0U, static_cast<uint8_t>(StripControl::MATRIX_WIDTH - 1U));
        const uint8_t x2 = beatsin8(23U, 0U, static_cast<uint8_t>(StripControl::MATRIX_WIDTH - 1U));
        const uint8_t x3 = beatsin8(27U, 0U, static_cast<uint8_t>(StripControl::MATRIX_WIDTH - 1U));

        const uint8_t y1 = beatsin8(20U, 0U, static_cast<uint8_t>(StripControl::MATRIX_HEIGHT - 1U));
        const uint8_t y2 = beatsin8(26U, 0U, static_cast<uint8_t>(StripControl::MATRIX_HEIGHT - 1U));
        const uint8_t y3 = beatsin8(15U, 0U, static_cast<uint8_t>(StripControl::MATRIX_HEIGHT - 1U));

        drawCurve(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2),
                  static_cast<float>(x3), static_cast<float>(y3), CHSV(hue, 255U, 255U));
        hue++;
        StripControl::show();
    }

    void drawJumpingCircle() {
        static XYMap xyMap(static_cast<uint16_t>(StripControl::MATRIX_WIDTH),
                           static_cast<uint16_t>(StripControl::MATRIX_HEIGHT));

        for (uint32_t i = 8U; i-- > 0U;) {
            StripControl::leds[Effects::getIndex(
                    beatsin8(static_cast<uint8_t>(12U + i), 0U, static_cast<uint8_t>(StripControl::MATRIX_WIDTH - 1U)),
                    beatsin8(static_cast<uint8_t>(15U - i), 0U,
                             static_cast<uint8_t>(StripControl::MATRIX_HEIGHT - 1U)))] =
                    CHSV(beatsin8(12U, 0U, 255U), 255U, 255U);
            blur2d(&StripControl::leds[0], static_cast<uint8_t>(StripControl::MATRIX_WIDTH),
                   static_cast<uint8_t>(StripControl::MATRIX_HEIGHT), 16U, xyMap);
        }
        StripControl::show();
    }

    void drawJumpingSquare() {
        static float squareX = 0.0F;
        static float squareY = 0.0F;
        static float directionX = 1.0F;
        static float directionY = 0.95F;
        static CRGB squareColor = CRGB::Red;
        static uint32_t lastTime = 0U;

        if ((millis() - lastTime) < static_cast<uint32_t>(JumpingLightsDelays::JUMPING_SQUARE_DELAY)) {
            return;
        }
        lastTime = millis();

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

        for (uint8_t i = 0U; i < 3U; i++) {
            for (uint8_t j = 0U; j < 3U; j++) {
                const float fx = squareX + static_cast<float>(i);
                const float fy = squareY + static_cast<float>(j);
                if (fx >= 0.0F && fy >= 0.0F) {
                    if (const uint32_t ledIndex =
                                Effects::getIndex(static_cast<uint32_t>(fx), static_cast<uint32_t>(fy));
                        ledIndex < StripControl::MATRIX_LEDS) {
                        StripControl::leds[ledIndex] = squareColor;
                    }
                }
            }
        }
        StripControl::show();

        squareX += directionX;
        squareY += directionY;

        if ((squareX + 3.0F) >= static_cast<float>(StripControl::MATRIX_WIDTH) || squareX < 0.0F) {
            directionX = -directionX;
            squareX = (squareX < 0.0F) ? 0.0F : static_cast<float>(StripControl::MATRIX_WIDTH - 3U);
        }
        if ((squareY + 3.0F) >= static_cast<float>(StripControl::MATRIX_HEIGHT) || squareY < 0.0F) {
            directionY = -directionY;
            squareY = (squareY < 0.0F) ? 0.0F : static_cast<float>(StripControl::MATRIX_HEIGHT - 3U);
        }

        static uint8_t frameCount = 0U;
        frameCount++;
        if (frameCount >= 50U) {
            squareColor = CHSV(random8(), 255U, 255U);
            frameCount = 0U;
        }
    }

    void initPoints() {
        for (auto &pt: points) {
            pt.x = static_cast<int16_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_WIDTH)));
            pt.y = static_cast<int16_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_HEIGHT)));
            pt.color = CHSV(random8(), 255U, 255U);
            pt.directionX = (ESP8266TrueRandom.random(0, 2) == 0) ? static_cast<int16_t>(1) : static_cast<int16_t>(-1);
            pt.directionY = (ESP8266TrueRandom.random(0, 2) == 0) ? static_cast<int16_t>(1) : static_cast<int16_t>(-1);
        }
    }

    void drawJumpingPoints() {
        static uint32_t lastTime = 0U;

        if ((millis() - lastTime) < static_cast<uint32_t>(JumpingLightsDelays::JUMPING_POINTS_DELAY)) {
            return;
        }
        lastTime = millis();

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

        for (const auto &pt: points) {
            if (const auto ledIndex = static_cast<uint16_t>(
                        Effects::getIndex(static_cast<uint32_t>(pt.x), static_cast<uint32_t>(pt.y)));
                ledIndex < StripControl::MATRIX_LEDS) {
                StripControl::leds[ledIndex] = pt.color;
            }
        }
        StripControl::show();

        for (auto &pt: points) {
            pt.x = static_cast<int16_t>(pt.x + pt.directionX);
            pt.y = static_cast<int16_t>(pt.y + pt.directionY);

            if (pt.x >= static_cast<int16_t>(StripControl::MATRIX_WIDTH) || pt.x < 0) {
                pt.directionX = static_cast<int16_t>(-pt.directionX);
                if (random8(10U) > 7U) {
                    pt.directionY = (random8(2U) == 0U) ? static_cast<int16_t>(1) : static_cast<int16_t>(-1);
                }
                pt.x = (pt.x < 0) ? static_cast<int16_t>(0) : static_cast<int16_t>(StripControl::MATRIX_WIDTH - 1U);
            }

            if (pt.y >= static_cast<int16_t>(StripControl::MATRIX_HEIGHT) || pt.y < 0) {
                pt.directionY = static_cast<int16_t>(-pt.directionY);
                if (random8(10U) > 7U) {
                    pt.directionX = (random8(2U) == 0U) ? static_cast<int16_t>(1) : static_cast<int16_t>(-1);
                }
                pt.y = (pt.y < 0) ? static_cast<int16_t>(0) : static_cast<int16_t>(StripControl::MATRIX_HEIGHT - 1U);
            }
        }

        for (auto &pt: points) {
            if (random8(20U) == 0U) {
                pt.color = CHSV(random8(), 255U, 255U);
            }
        }
    }
} // namespace
