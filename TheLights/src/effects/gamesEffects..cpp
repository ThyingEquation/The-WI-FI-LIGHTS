#include "effects.h"

/*
  Эта группа эффектов для любого размера матриц

  Настраиваемые параметры (GamesSettings):
    1) Скорости эффектов

  Наиболее оптимальная яркость для визуализации эффекта drawMatrixMovie (Матрица) - 50%
*/

enum GamesSettings {
    SNAKE_GAME_DELAY = 130,
    TETRIS_GAME_DELAY = 200,
    ARKANOID_GAME_DELAY = 100,
    SPACESHIP_GAME_DELAY = 20
};

static int8_t snakeX[5];
static int8_t snakeY[5];
static int8_t foodX, foodY;

static uint8_t numFallingFigures = 0;

struct FallingFigure {
    int8_t figureIndex{};
    int8_t x{};
    int8_t y{};
    CRGB color;
};

static FallingFigure fallingFigures[5];

const uint8_t figures[14][4][2] = {
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{0, 0}, {1, 0}, {2, 0}, {1, 1}},
        {{0, 0}, {1, 0}, {1, 1}, {2, 1}}, {{0, 0}, {0, 1}, {1, 1}, {2, 1}}, {{0, 0}, {1, 0}, {2, 0}, {2, 1}},
        {{0, 0}, {1, 0}, {2, 0}, {0, 1}}, {{0, 0}, {0, 1}, {0, 2}, {0, 3}}, {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
        {{0, 0}, {0, 1}, {0, 2}, {1, 1}}, {{0, 0}, {0, 1}, {1, 1}, {1, 2}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {0, 1}, {0, 2}, {1, 2}}, {{0, 0}, {0, 1}, {0, 2}, {1, 0}}};

static float shipX = 5.5;
static float enemyX[3] = {2, 6, 10};
static float enemyY[3] = {-1, -3, -5};
static bool enemyAlive[3] = {true, true, true};
static float bX = -1, bY = -1;
static bool bActive = false;

static void drawSnakeGame();
static void drawTetrisGame();
static void drawArkanoidGame();
static void drawSpaceshipGame();
static void drawMatrixMovie();

void drawGamesEffects(const uint8_t subMode) {
    switch (subMode) {
        case 0:
            drawSnakeGame();
            break;

        case 1:
            drawTetrisGame();
            break;

        case 2:
            drawArkanoidGame();
            break;

        case 3:
            drawSpaceshipGame();
            break;

        case 4:
            drawMatrixMovie();
            break;

        default:
            break;
    }
}

static void updateSnake() {
    static uint8_t snakeDirection = 0;

    for (uint8_t i = 5 - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    switch (snakeDirection) {
        case 0:
            snakeY[0]--;
            break;
        case 1:
            snakeX[0]++;
            break;
        case 2:
            snakeY[0]++;
            break;
        case 3:
            snakeX[0]--;
            break;
        default:;
    }

    if (snakeX[0] < 0)
        snakeX[0] = MATRIX_WIDTH - 1;
    if (snakeX[0] >= MATRIX_WIDTH)
        snakeX[0] = 0;
    if (snakeY[0] < 0)
        snakeY[0] = MATRIX_HEIGHT - 1;
    if (snakeY[0] >= MATRIX_HEIGHT)
        snakeY[0] = 0;

    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        foodX = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
        foodY = ESP8266TrueRandom.random(0, MATRIX_HEIGHT);
    }

    if (ESP8266TrueRandom.random(0, 10) == 0) {
        snakeDirection = ESP8266TrueRandom.random(0, 4);
    }
}

static void drawSnakeGame() {
    static uint32_t lastTime = 0;

    if (millis() - lastTime < SNAKE_GAME_DELAY)
        return;
    lastTime = millis();

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    updateSnake();

    for (uint8_t i = 0; i < 5; i++) {
        if (const int16_t ledIndex = xyToIndex(snakeX[i], snakeY[i]); ledIndex >= 0 && ledIndex < MATRIX_LEDS) {
            leds[ledIndex] = CRGB::Green;
        }
    }

    if (const int16_t ledIndex = xyToIndex(foodX, foodY); ledIndex >= 0 && ledIndex < MATRIX_LEDS) {
        leds[ledIndex] = CRGB::White;
    }

    stripShow();
}

static bool checkCollision(const uint8_t figureIndex, const int8_t x) {
    for (uint8_t i = 0; i < 4; i++) {
        const int8_t px = x + figures[figureIndex][i][0];
        const int8_t py = MATRIX_HEIGHT - 1 + figures[figureIndex][i][1];
        if (px < 0 || px >= MATRIX_WIDTH || py < -4) {
            return true;
        }
        for (uint8_t j = 0; j < numFallingFigures; j++) {
            const FallingFigure &fig = fallingFigures[j];
            for (uint8_t k = 0; k < 4; k++) {
                const int8_t fx = fig.x + figures[fig.figureIndex][k][0];
                const int8_t fy = fig.y + figures[fig.figureIndex][k][1];
                if (px == fx && py == fy) {
                    return true;
                }
            }
        }
    }
    return false;
}

static void addNewFigure() {
    if (numFallingFigures < 5) {
        const int8_t figureIndex = ESP8266TrueRandom.random(0, 14);
        int8_t x, y;
        uint8_t attempts = 0;
        do {
            x = ESP8266TrueRandom.random(0, MATRIX_WIDTH - 2);
            y = MATRIX_HEIGHT - 1;
            attempts++;
            if (attempts > 20)
                return;
        } while (checkCollision(figureIndex, x));

        const CRGB color = CHSV(random8(), 255, 255);
        fallingFigures[numFallingFigures] = {figureIndex, x, y, color};
        numFallingFigures++;
    }
}

static void drawTetrisGame() {
    static uint32_t lastTime = 0;

    if (millis() - lastTime < TETRIS_GAME_DELAY)
        return;
    lastTime = millis();

    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
        leds[i] = CRGB::Black;

    for (int8_t i = 0; i < numFallingFigures; i++) {
        FallingFigure &fig = fallingFigures[i];
        fig.y--;

        if (fig.y < -4) {
            for (uint8_t j = i; j < numFallingFigures - 1; j++) {
                fallingFigures[j] = fallingFigures[j + 1];
            }
            numFallingFigures--;
            i--;
        } else {
            for (uint8_t f = 0; f < 4; f++) {
                const int16_t px = static_cast<int16_t>(fig.x) + figures[fig.figureIndex][f][0];
                const int16_t py = static_cast<int16_t>(fig.y) + figures[fig.figureIndex][f][1];
                if (px >= 0 && px < MATRIX_WIDTH && py >= 0 && py < MATRIX_HEIGHT) {
                    leds[xyToIndex(px, py)] = fig.color;
                }
            }
        }
    }

    if (random8(100) < 25) {
        addNewFigure();
    }

    stripShow();
}

static void drawArkanoidGame() {
    static uint32_t lastTime = 0;

    if (millis() - lastTime < ARKANOID_GAME_DELAY)
        return;
    lastTime = millis();

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);

    static CRGB tileColors[] = {CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Red};
    constexpr uint8_t totalTiles = 20;
    constexpr uint8_t platformWidth = 4;

    for (uint8_t i = 0; i < totalTiles; i++) {
        constexpr uint8_t tilesPerRow = 4;
        constexpr uint8_t tileWidth = 3;
        const uint8_t row = i / tilesPerRow;
        const uint8_t col = i % tilesPerRow;
        const uint8_t startIndex = row * tileWidth * tilesPerRow + col * tileWidth;

        for (uint8_t j = 0; j < tileWidth; j++) {
            if (const uint8_t ledIndex = mainMatrixScheme[startIndex + j]; ledIndex < MATRIX_LEDS) {
                leds[ledIndex] = tileColors[col];
            }
        }
    }

    static int16_t platformPosition = 4;
    static int16_t platformDirection = 1;
    constexpr uint8_t platformRowOffset = MATRIX_LEDS / 12 - 1;

    platformPosition += platformDirection;
    if (platformPosition >= 12 - platformWidth || platformPosition < 0) {
        platformDirection = -platformDirection;
        platformPosition += platformDirection;
    }

    for (uint8_t i = 0; i < platformWidth; i++) {
        if (const uint8_t ledIndex = mainMatrixScheme[platformRowOffset * 12 + platformPosition + i];
            ledIndex < MATRIX_LEDS) {
            leds[ledIndex] = i == 0 || i == platformWidth - 1 ? CRGB::Red : CRGB::White;
        }
    }

    static uint8_t ballX = 6, ballY = 5, ballDX = 1, ballDY = -1;

    leds[xyToIndex(ballX, ballY)] = CRGB::Black;

    ballX += ballDX;
    ballY += ballDY;

    if (ballX <= 0 || ballX >= 11) {
        ballDX = -ballDX;
    }
    if (constexpr uint8_t ballMinY = 0, ballMaxY = 6; ballY >= ballMaxY || ballY <= ballMinY) {
        ballDY = -ballDY;
    }

    leds[xyToIndex(ballX, ballY)] = CRGB::White;

    stripShow();
}

void setPixelSafe(const float y, const float x, const CRGB color) {
    const int8_t iy = static_cast<int8_t>(round(y));
    const int8_t ix = static_cast<int8_t>(round(x));
    if (iy < 0 || iy >= MATRIX_HEIGHT || ix < 0 || ix >= MATRIX_WIDTH)
        return;
    leds[xyToIndex(ix, iy)] = color;
}

static void drawSpaceshipGame() {
    static uint32_t lastTime = 0;

    if (millis() - lastTime < SPACESHIP_GAME_DELAY)
        return;
    lastTime = millis();

    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
        leds[i].nscale8(140);

    int8_t target = -1;
    float minY = -10;
    for (int i = 0; i < 3; i++) {
        if (enemyAlive[i] && enemyY[i] > minY) {
            minY = enemyY[i];
            target = i;
        }
    }

    if (target != -1) {
        if (shipX < enemyX[target])
            shipX += 0.12;
        if (shipX > enemyX[target])
            shipX -= 0.12;
    }
    shipX = constrain(shipX, 0, 11);

    if (!bActive && target != -1 && enemyY[target] > 0) {
        bX = shipX;
        bY = 10;
        bActive = true;
    }

    if (bActive) {
        bY -= 0.4;
        if (bY < 0)
            bActive = false;

        for (int i = 0; i < 3; i++) {
            if (enemyAlive[i] && abs(bX - enemyX[i]) < 1.0 && abs(bY - enemyY[i]) < 1.0) {
                enemyAlive[i] = false;
                bActive = false;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        enemyY[i] += 0.08;

        if (!enemyAlive[i] || enemyY[i] > 12) {
            enemyAlive[i] = true;
            enemyY[i] = -random8(1, 5);
            enemyX[i] = random8(0, 12);
        }
    }

    setPixelSafe(11, static_cast<int>(shipX), CRGB::Green);

    for (int i = 0; i < 3; i++) {
        if (enemyAlive[i]) {
            setPixelSafe(static_cast<int>(enemyY[i]), static_cast<int>(enemyX[i]), CRGB::Red);
            setPixelSafe(static_cast<int>(enemyY[i]) - 1, static_cast<int>(enemyX[i]), CRGB(50, 0, 0));
        }
    }

    if (bActive) {
        setPixelSafe(static_cast<int>(bY), static_cast<int>(bX), CRGB::LightBlue);
        setPixelSafe(static_cast<int>(bY) + 1, static_cast<int>(bX), CRGB(0, 50, 80));
    }
    stripShow();
}

static void drawMatrixMovie() {
    static uint8_t currentCol[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 6};
    static uint8_t currentRow[11] = {7, 8, 5, 6, 13, 2, 5, 11, 8, 7, 11};
    static uint8_t lineLength[11] = {9, 6, 5, 7, 8, 9, 7, 5, 7, 8, 5};
    static uint32_t lastTime = 0;

    auto drawLine = [](const uint8_t col, const uint8_t row, const uint8_t length, const bool clear) {
        for (uint8_t j = 0; j < length; ++j) {
            if (const uint8_t rowIndex = row - j; rowIndex < MATRIX_HEIGHT) {
                const uint16_t pixelIndex = xyToIndex(col, rowIndex);
                if (clear) {
                    leds[pixelIndex] = CRGB::Black;
                } else {
                    const uint8_t brightness = j == length - 1 ? 255 : 5 + 5 * j;
                    leds[pixelIndex] = hsv2rgb_spectrum(CHSV(90, 255, brightness));
                }
            }
        }
    };

    if (millis() - lastTime < 120)
        return;
    lastTime = millis();

    for (uint8_t i = 0; i < 11; i++) {
        drawLine(currentCol[i], currentRow[i], lineLength[i], true);
    }

    for (uint8_t i = 0; i < 11; i++) {
        if (currentRow[i] > 0) {
            --currentRow[i];
        } else {
            currentCol[i] = ESP8266TrueRandom.random(0, MATRIX_WIDTH);
            currentRow[i] = MATRIX_HEIGHT + 8 + ESP8266TrueRandom.random(0, MATRIX_HEIGHT + 3);
            lineLength[i] = 5 + ESP8266TrueRandom.random(0, 5);
        }
    }

    for (uint8_t i = 0; i < 11; i++) {
        drawLine(currentCol[i], currentRow[i], lineLength[i], false);
    }

    stripShow();
}
