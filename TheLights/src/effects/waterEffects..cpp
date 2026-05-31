#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц
  Настраиваемые параметры: нет
*/

namespace {
    void drawLagoon();
    void drawPool();
} // namespace

namespace Effects {
    void drawWaterEffects(const uint32_t subMode) {
        switch (subMode) {
            case 0U:
                EVERY_N_MILLISECONDS(20U) { drawLagoon(); }
                break;

            case 1U:
                drawPool();
                break;

            default:
                break;
        }
    }
} // namespace Effects

namespace {
    void drawLagoonOneLayer(const CRGBPalette16 &p, const uint32_t ciStart, const uint32_t waveScale,
                            const uint8_t brightness, const uint32_t iOff) {
        uint32_t ci = ciStart;
        uint32_t waveAngle = iOff;
        const uint32_t waveScaleHalf = waveScale / 2U + 20U;
        for (auto &led: StripControl::leds) {
            waveAngle += 250U;
            const uint32_t s16 = static_cast<uint32_t>(sin16(static_cast<uint16_t>(waveAngle))) + 32768U;
            const uint32_t cs =
                    scale16(static_cast<uint16_t>(s16), static_cast<uint16_t>(waveScaleHalf)) + waveScaleHalf;
            ci += cs;
            const uint32_t sinDex16 = static_cast<uint32_t>(sin16(static_cast<uint16_t>(ci))) + 32768U;
            const auto sinDex8 = static_cast<uint8_t>(scale16(static_cast<uint16_t>(sinDex16), 240U));
            const CRGB c = ColorFromPalette(p, sinDex8, brightness, LINEARBLEND);
            led += c;
        }
    }

    void drawLagoonAddWhitecaps() {
        const uint8_t baseThreshold = beatsin8(9U, 55U, 65U);
        uint8_t wave = beat8(7U);

        for (auto &led: StripControl::leds) {
            const uint8_t threshold = scale8(sin8(wave), 20U) + baseThreshold;
            wave += 7;
            if (const uint8_t l = led.getAverageLight(); l > threshold) {
                const uint8_t overAge = l - threshold;
                const uint8_t overAge2 = qadd8(overAge, overAge);
                led += CRGB(overAge, overAge2, qadd8(overAge2, overAge2));
            }
        }
    }

    void drawLagoonDeepenColors() {
        for (auto &led: StripControl::leds) {
            led.blue = scale8(led.blue, 145U);
            led.green = scale8(led.green, 200U);
            led |= CRGB(2U, 5U, 7U);
        }
    }

    void drawLagoon() {
        static CRGBPalette16 drawLagoonPalette1 = {0x000507U, 0x000409U, 0x00030BU, 0x00030DU, 0x000210U, 0x000212U,
                                                   0x000114U, 0x000117U, 0x000019U, 0x00001CU, 0x000026U, 0x000031U,
                                                   0x00003BU, 0x000046U, 0x14554BU, 0x28AA50U};
        static CRGBPalette16 drawLagoonPalette2 = {0x000507U, 0x000409U, 0x00030BU, 0x00030DU, 0x000210U, 0x000212U,
                                                   0x000114U, 0x000117U, 0x000019U, 0x00001CU, 0x000026U, 0x000031U,
                                                   0x00003BU, 0x000046U, 0x0C5F52U, 0x19BE5FU};
        static CRGBPalette16 drawLagoonPalette3 = {0x000208U, 0x00030EU, 0x000514U, 0x00061AU, 0x000820U, 0x000927U,
                                                   0x000B2DU, 0x000C33U, 0x000E39U, 0x001040U, 0x001450U, 0x001860U,
                                                   0x001C70U, 0x002080U, 0x1040BFU, 0x2060FFU};

        static uint16_t sciStart1 = 0U;
        static uint16_t sciStart2 = 0U;
        static uint16_t sciStart3 = 0U;
        static uint16_t sciStart4 = 0U;
        static uint32_t sLastMs = 0U;
        const uint32_t ms = GET_MILLIS();
        const uint32_t deltaMs = ms - sLastMs;
        sLastMs = ms;
        const uint16_t speedFactor1 = beatsin16(3U, 179U, 269U);
        const uint16_t speedFactor2 = beatsin16(4U, 179U, 269U);
        const uint32_t deltaMs1 = deltaMs * speedFactor1 / 256U;
        const uint32_t deltaMs2 = deltaMs * speedFactor2 / 256U;
        const uint32_t deltaMs21 = (deltaMs1 + deltaMs2) / 2U;
        sciStart1 += deltaMs1 * beatsin88(1011U, 10U, 13U);
        sciStart2 -= deltaMs21 * beatsin88(777U, 8U, 11U);
        sciStart3 -= deltaMs1 * beatsin88(501U, 5U, 7U);
        sciStart4 -= deltaMs2 * beatsin88(257U, 4U, 6U);

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB(2U, 6U, 10U));

        drawLagoonOneLayer(drawLagoonPalette1, sciStart1, beatsin16(3U, 11U * 256U, 14U * 256U),
                           beatsin8(10U, 70U, 130U), 0U - beat16(301U));
        drawLagoonOneLayer(drawLagoonPalette2, sciStart2, beatsin16(4U, 6U * 256U, 9U * 256U), beatsin8(17U, 40U, 80U),
                           beat16(401U));
        drawLagoonOneLayer(drawLagoonPalette3, sciStart3, 6U * 256U, beatsin8(9U, 10U, 38U), 0U - beat16(503U));
        drawLagoonOneLayer(drawLagoonPalette3, sciStart4, 5U * 256U, beatsin8(8U, 10U, 28U), beat16(601U));

        drawLagoonAddWhitecaps();
        drawLagoonDeepenColors();
        StripControl::show();
    }

    void drawPool() {
        static CRGBPalette16 currentPalette(PartyColors_p);
        static XYMap xyMap(static_cast<uint16_t>(StripControl::MATRIX_WIDTH),
                           static_cast<uint16_t>(StripControl::MATRIX_HEIGHT));
        static bool loadingFlag = true;
        static uint8_t hue = 0U;
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < 20U) {
            return;
        }
        lastTime = millis();

        const uint32_t t = millis() / 16U;

        if (loadingFlag) {
            loadingFlag = false;
            hue = 150U;
        }

        fill_solid(currentPalette, 16, CHSV(hue, 255U, 230U));
        currentPalette[10] = CHSV(hue, 195U, 255U);
        currentPalette[9] = CHSV(hue, 0U, 220U);
        currentPalette[8] = CHSV(hue, 0U, 210U);
        currentPalette[7] = CHSV(hue, 195U, 255U);

        blur2d(&StripControl::leds[0], static_cast<uint8_t>(StripControl::MATRIX_WIDTH),
               static_cast<uint8_t>(StripControl::MATRIX_HEIGHT), 100U, xyMap);

        for (uint32_t y = 0U; y < StripControl::MATRIX_HEIGHT; y++) {
            for (uint32_t x = 0U; x < StripControl::MATRIX_WIDTH; x++) {
                const uint8_t pixelHue8 = inoise8(static_cast<uint16_t>(x * 30U), static_cast<uint16_t>(y * 30U),
                                                  static_cast<uint16_t>(t));
                StripControl::leds[Effects::getIndex(x, y)] = ColorFromPalette(currentPalette, pixelHue8);
            }
        }
        blur2d(&StripControl::leds[0], static_cast<uint8_t>(StripControl::MATRIX_WIDTH),
               static_cast<uint8_t>(StripControl::MATRIX_HEIGHT), 32U, xyMap);

        StripControl::show();
    }
} // namespace
