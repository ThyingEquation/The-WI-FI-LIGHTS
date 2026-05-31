#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (GamesDelays):
    1) Скорости эффектов

  Наиболее оптимальная яркость для визуализации эффекта drawMatrixMovie (Матрица) - 50%
*/

namespace {
    enum class GamesDelays : uint16_t {
        SNAKE_GAME_DELAY = 130U,
        TETRIS_GAME_DELAY = 200U,
        ARKANOID_GAME_DELAY = 100U,
        SPACESHIP_GAME_DELAY = 20U
    };

    int8_t snakeX[5];
    int8_t snakeY[5];
    int8_t foodX;
    int8_t foodY;

    uint32_t numFallingFigures = 0U;

    struct FallingFigure {
        int8_t figureIndex{};
        int8_t x{};
        int8_t y{};
        CRGB color{CRGB::Black};
    };

    FallingFigure fallingFigures[5];

    const uint8_t figures[14][4][2] = {
            {{0U, 0U}, {1U, 0U}, {2U, 0U}, {3U, 0U}}, {{0U, 0U}, {1U, 0U}, {0U, 1U}, {1U, 1U}},
            {{0U, 0U}, {1U, 0U}, {2U, 0U}, {1U, 1U}}, {{0U, 0U}, {1U, 0U}, {1U, 1U}, {2U, 1U}},
            {{0U, 0U}, {0U, 1U}, {1U, 1U}, {2U, 1U}}, {{0U, 0U}, {1U, 0U}, {2U, 0U}, {2U, 1U}},
            {{0U, 0U}, {1U, 0U}, {2U, 0U}, {0U, 1U}}, {{0U, 0U}, {0U, 1U}, {0U, 2U}, {0U, 3U}},
            {{0U, 0U}, {0U, 1U}, {1U, 0U}, {1U, 1U}}, {{0U, 0U}, {0U, 1U}, {0U, 2U}, {1U, 1U}},
            {{0U, 0U}, {0U, 1U}, {1U, 1U}, {1U, 2U}}, {{0U, 0U}, {1U, 0U}, {0U, 1U}, {1U, 1U}},
            {{0U, 0U}, {0U, 1U}, {0U, 2U}, {1U, 2U}}, {{0U, 0U}, {0U, 1U}, {0U, 2U}, {1U, 0U}}};

    float shipX = 5.5F;
    float enemyX[3] = {2.0F, 6.0F, 10.0F};
    float enemyY[3] = {-1.0F, -3.0F, -5.0F};
    bool enemyAlive[3] = {true, true, true};
    float bX = -1.0F;
    float bY = -1.0F;
    bool bActive = false;

    void drawSnakeGame();
    void drawTetrisGame();
    void drawArkanoidGame();
    void drawSpaceshipGame();
    void drawMatrixMovie();
} // namespace

namespace Effects {
    void drawGamesEffects(const uint32_t subMode) {
        switch (subMode) {
            case 0U:
                drawSnakeGame();
                break;

            case 1U:
                drawTetrisGame();
                break;

            case 2U:
                drawArkanoidGame();
                break;

            case 3U:
                drawSpaceshipGame();
                break;

            case 4U:
                drawMatrixMovie();
                break;

            default:
                break;
        }
    }
} // namespace Effects

namespace {
    void updateSnake() {
        static uint32_t snakeDirection = 0U;

        for (uint32_t i = 5U - 1U; i > 0U; i--) {
            snakeX[i] = snakeX[i - 1U];
            snakeY[i] = snakeY[i - 1U];
        }

        switch (snakeDirection) {
            case 0U:
                snakeY[0]--;
                break;
            case 1U:
                snakeX[0]++;
                break;
            case 2U:
                snakeY[0]++;
                break;
            case 3U:
                snakeX[0]--;
                break;
            default:;
        }

        if (snakeX[0] < 0) {
            snakeX[0] = static_cast<int8_t>(StripControl::MATRIX_WIDTH - 1U);
        }
        if (snakeX[0] >= static_cast<int8_t>(StripControl::MATRIX_WIDTH)) {
            snakeX[0] = 0;
        }
        if (snakeY[0] < 0) {
            snakeY[0] = static_cast<int8_t>(StripControl::MATRIX_HEIGHT - 1U);
        }
        if (snakeY[0] >= static_cast<int8_t>(StripControl::MATRIX_HEIGHT)) {
            snakeY[0] = 0;
        }

        if (snakeX[0] == foodX && snakeY[0] == foodY) {
            foodX = static_cast<int8_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_WIDTH)));
            foodY = static_cast<int8_t>(ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_HEIGHT)));
        }

        if (ESP8266TrueRandom.random(0, 10) == 0) {
            snakeDirection = static_cast<uint32_t>(ESP8266TrueRandom.random(0, 4));
        }
    }

    void drawSnakeGame() {
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(GamesDelays::SNAKE_GAME_DELAY)) {
            return;
        }
        lastTime = millis();

        fill_solid(&StripControl::leds[0], static_cast<int32_t>(StripControl::MATRIX_LEDS), CRGB::Black);
        updateSnake();

        for (uint32_t i = 0U; i < 5U; i++) {
            if (const auto ledIndex = static_cast<int32_t>(
                        Effects::getIndex(static_cast<uint32_t>(snakeX[i]), static_cast<uint32_t>(snakeY[i])));
                ledIndex >= 0 && ledIndex < static_cast<int32_t>(StripControl::MATRIX_LEDS)) {
                StripControl::leds[ledIndex] = CRGB::Green;
            }
        }

        if (const auto ledIndex =
                    static_cast<int32_t>(Effects::getIndex(static_cast<uint32_t>(foodX), static_cast<uint32_t>(foodY)));
            ledIndex >= 0 && ledIndex < static_cast<int32_t>(StripControl::MATRIX_LEDS)) {
            StripControl::leds[ledIndex] = CRGB::White;
        }

        StripControl::show();
    }

    bool checkCollision(const uint8_t figureIndex, const int8_t x) {
        for (uint32_t i = 0U; i < 4U; i++) {
            const auto px = static_cast<int8_t>(x + static_cast<int8_t>(figures[figureIndex][i][0]));
            const auto py = static_cast<int8_t>(StripControl::MATRIX_HEIGHT - 1U + figures[figureIndex][i][1]);
            if (px < 0 || px >= static_cast<int8_t>(StripControl::MATRIX_WIDTH) || py < -4) {
                return true;
            }
            for (uint32_t j = 0U; j < numFallingFigures; j++) {
                const FallingFigure &fig = fallingFigures[j];
                for (uint32_t k = 0U; k < 4U; k++) {
                    const int8_t fx = fig.x + static_cast<int8_t>(figures[fig.figureIndex][k][0]);
                    const int8_t fy = fig.y + static_cast<int8_t>(figures[fig.figureIndex][k][1]);
                    if (px == fx && py == fy) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void addNewFigure() {
        if (numFallingFigures < 5U) {
            const auto figureIndex = static_cast<int8_t>(ESP8266TrueRandom.random(0, 14));
            int8_t x = 0;
            int8_t y = 0;
            uint32_t attempts = 0U;

            do {
                x = static_cast<int8_t>(
                        ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_WIDTH - 2U)));
                y = static_cast<int8_t>(StripControl::MATRIX_HEIGHT - 1U);
                attempts++;
                if (attempts > 20U) {
                    return;
                }
            } while (checkCollision(static_cast<uint8_t>(figureIndex), x));

            const CRGB color = CHSV(random8(), 255U, 255U);
            fallingFigures[numFallingFigures] = {figureIndex, x, y, color};
            numFallingFigures++;
        }
    }

    void drawTetrisGame() {
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(GamesDelays::TETRIS_GAME_DELAY)) {
            return;
        }
        lastTime = millis();

        for (auto &led: StripControl::leds) {
            led = CRGB::Black;
        }

        for (int8_t i = 0; i < static_cast<int8_t>(numFallingFigures); i++) {
            FallingFigure &fig = fallingFigures[i];
            fig.y--;

            if (fig.y < -4) {
                for (int8_t j = i; j < static_cast<int8_t>(numFallingFigures - 1U); j++) {
                    fallingFigures[j] = fallingFigures[j + 1];
                }
                numFallingFigures--;
                i--;
            } else {
                for (uint32_t f = 0U; f < 4U; f++) {
                    const auto px = static_cast<int16_t>(fig.x + static_cast<int16_t>(figures[fig.figureIndex][f][0]));
                    const auto py = static_cast<int16_t>(fig.y + static_cast<int16_t>(figures[fig.figureIndex][f][1]));
                    if (px >= 0 && px < static_cast<int16_t>(StripControl::MATRIX_WIDTH) && py >= 0 &&
                        py < static_cast<int16_t>(StripControl::MATRIX_HEIGHT)) {
                        StripControl::leds[Effects::getIndex(static_cast<uint32_t>(px), static_cast<uint32_t>(py))] =
                                fig.color;
                    }
                }
            }
        }

        if (random8(100U) < 25U) {
            addNewFigure();
        }

        StripControl::show();
    }

    void drawArkanoidGame() {
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(GamesDelays::ARKANOID_GAME_DELAY)) {
            return;
        }
        lastTime = millis();

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);

        static CRGB tileColors[] = {CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Red};
        constexpr uint32_t totalTiles = 20U;
        constexpr uint32_t platformWidth = 4U;

        for (uint32_t i = 0U; i < totalTiles; i++) {
            constexpr uint32_t tilesPerRow = 4U;
            constexpr uint32_t tileWidth = 3U;
            const auto row = static_cast<uint32_t>(static_cast<double>(i) / static_cast<double>(tilesPerRow));
            const auto col = static_cast<uint32_t>(i % tilesPerRow);
            const auto startIndex = static_cast<uint32_t>(static_cast<double>(row) * static_cast<double>(tileWidth) *
                                                                  static_cast<double>(tilesPerRow) +
                                                          static_cast<double>(col) * static_cast<double>(tileWidth));

            for (uint32_t j = 0U; j < tileWidth; j++) {
                if (const uint32_t ledIndex = Effects::mainMatrixScheme[startIndex + j];
                    ledIndex < StripControl::MATRIX_LEDS) {
                    StripControl::leds[ledIndex] = tileColors[col];
                }
            }
        }

        static int32_t platformPosition = 4;
        static int32_t platformDirection = 1;
        constexpr auto platformRowOffset = static_cast<uint32_t>(StripControl::MATRIX_LEDS / 12U - 1U);

        platformPosition += platformDirection;
        if (platformPosition >= 12 - static_cast<int8_t>(platformWidth) || platformPosition < 0) {
            platformDirection = -platformDirection;
            platformPosition += platformDirection;
        }

        for (uint32_t i = 0U; i < platformWidth; i++) {
            if (const uint32_t ledIndex = Effects::mainMatrixScheme[platformRowOffset * 12U +
                                                                    static_cast<uint32_t>(platformPosition) + i];
                ledIndex < StripControl::MATRIX_LEDS) {
                StripControl::leds[ledIndex] = i == 0U || i == platformWidth - 1U ? CRGB::Red : CRGB::White;
            }
        }

        static int8_t ballX = 6;
        static int8_t ballY = 5;
        static int8_t ballDX = 1;
        static int8_t ballDY = 1;

        StripControl::leds[Effects::getIndex(static_cast<uint32_t>(ballX), static_cast<uint32_t>(ballY))] = CRGB::Black;

        ballX += ballDX;
        ballY += ballDY;

        if (ballX <= 0U || ballX >= 11U) {
            ballDX = -ballDX;
        }

        constexpr int32_t ballMinY = 0;
        constexpr int32_t ballMaxY = 6;
        if (ballY >= ballMaxY || ballY <= ballMinY) {
            ballDY = -ballDY;
        }

        StripControl::leds[Effects::getIndex(static_cast<uint32_t>(ballX), static_cast<uint32_t>(ballY))] = CRGB::White;
        StripControl::show();
    }

    void setPixelSafe(const float y, const float x, const CRGB color) {
        const auto iy = static_cast<int8_t>(round(y));
        const auto ix = static_cast<int8_t>(round(x));
        if (iy < 0 || iy >= static_cast<int8_t>(StripControl::MATRIX_HEIGHT) || ix < 0 ||
            ix >= static_cast<int8_t>(StripControl::MATRIX_WIDTH)) {
            return;
        }
        StripControl::leds[Effects::getIndex(static_cast<uint32_t>(ix), static_cast<uint32_t>(iy))] = color;
    }

    void drawSpaceshipGame() {
        static uint32_t lastTime = 0U;

        if (millis() - lastTime < static_cast<uint32_t>(GamesDelays::SPACESHIP_GAME_DELAY)) {
            return;
        }
        lastTime = millis();

        for (uint16_t i = 0U; i < StripControl::MATRIX_LEDS; i++) {
            (void) StripControl::leds[i].nscale8(140U);
        }

        int8_t target = -1;
        float minY = -10.0F;
        for (uint32_t i = 0U; i < 3U; i++) {
            if (enemyAlive[i] && enemyY[i] > minY) {
                minY = enemyY[i];
                target = static_cast<int8_t>(i);
            }
        }

        if (target != -1) {
            if (shipX < enemyX[target]) {
                shipX += 0.12;
            }
            if (shipX > enemyX[target]) {
                shipX -= 0.12;
            }
        }
        shipX = constrain(shipX, 0, 11);

        if (!bActive && target != -1 && enemyY[target] > 0.0F) {
            bX = shipX;
            bY = 10.0F;
            bActive = true;
        }

        if (bActive) {
            bY -= 0.4;
            if (bY < 0.0F) {
                bActive = false;
            }

            for (uint32_t i = 0U; i < 3U; i++) {
                if (enemyAlive[i] && abs(bX - enemyX[i]) < 1.0 && abs(bY - enemyY[i]) < 1.0) {
                    enemyAlive[i] = false;
                    bActive = false;
                }
            }
        }

        for (uint32_t i = 0U; i < 3U; i++) {
            enemyY[i] += 0.08;

            if (!enemyAlive[i] || enemyY[i] > 12.0F) {
                enemyAlive[i] = true;
                enemyY[i] = -random8(1U, 5U);
                enemyX[i] = random8(0U, 12U);
            }
        }

        setPixelSafe(11.0F, static_cast<float>(shipX), CRGB::Green);

        for (uint32_t i = 0U; i < 3U; i++) {
            if (enemyAlive[i]) {
                setPixelSafe(static_cast<float>(enemyY[i]), static_cast<float>(enemyX[i]), CRGB::Red);
                setPixelSafe(static_cast<float>(enemyY[i]) - 1.0F, static_cast<float>(enemyX[i]), CRGB(50U, 0U, 0U));
            }
        }

        if (bActive) {
            setPixelSafe(static_cast<float>(bY), static_cast<float>(bX), CRGB::LightBlue);
            setPixelSafe(static_cast<float>(bY) + 1.0F, static_cast<float>(bX), CRGB(0U, 50U, 80U));
        }
        StripControl::show();
    }

    void drawMatrixMovie() {
        static uint8_t currentCol[11] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 6U};
        static uint8_t currentRow[11] = {7U, 8U, 5U, 6U, 13U, 2U, 5U, 11U, 8U, 7U, 11U};
        static uint8_t lineLength[11] = {9U, 6U, 5U, 7U, 8U, 9U, 7U, 5U, 7U, 8U, 5U};
        static uint32_t lastTime = 0U;

        auto drawLine = [](const uint8_t col, const uint8_t row, const uint8_t length, const bool clear) {
            for (uint32_t j = 0U; j < length; ++j) {
                if (const uint32_t rowIndex = row - j; rowIndex < StripControl::MATRIX_HEIGHT) {
                    const uint32_t pixelIndex =
                            Effects::getIndex(static_cast<uint32_t>(col), static_cast<uint32_t>(rowIndex));
                    if (clear) {
                        StripControl::leds[pixelIndex] = CRGB::Black;
                    } else {
                        const uint32_t brightness = j == length - 1U ? 255U : 5U + 5U * j;
                        StripControl::leds[pixelIndex] =
                                hsv2rgb_spectrum(CHSV(90U, 255U, static_cast<uint8_t>(brightness)));
                    }
                }
            }
        };

        if (millis() - lastTime < 120U) {
            return;
        }
        lastTime = millis();

        for (uint32_t i = 0U; i < 11U; i++) {
            drawLine(currentCol[i], currentRow[i], lineLength[i], true);
        }

        for (uint32_t i = 0U; i < 11U; i++) {
            if (currentRow[i] > 0U) {
                --currentRow[i];
            } else {
                currentCol[i] = static_cast<uint8_t>(
                        ESP8266TrueRandom.random(0, static_cast<int32_t>(StripControl::MATRIX_WIDTH)));
                currentRow[i] = StripControl::MATRIX_HEIGHT + 8U +
                                static_cast<uint8_t>(ESP8266TrueRandom.random(
                                        0, static_cast<int32_t>(StripControl::MATRIX_HEIGHT + 3U)));
                lineLength[i] = 5U + static_cast<uint8_t>(ESP8266TrueRandom.random(0, 5));
            }
        }

        for (uint32_t i = 0U; i < 11U; i++) {
            drawLine(currentCol[i], currentRow[i], lineLength[i], false);
        }

        StripControl::show();
    }
} // namespace
