#include "effects.h"
#include "animationFrames.h"

/*
  Эта группа эффектов только для матрицы 12х12

  Jp - Japanese (японский). Текст на оригинале: "あけましておめでとう". Корректное отображение текста "снаружи"
  Kr - Korean (корейский).  Текст на оригинале: "안녕하세요". Корректное отображение текста "снаружи"

  Настраиваемые параметры (animationsSettings):
    1) Время отображения одного кадра (задержка)
*/

enum animationsSettings
{
  HEART_IMAGE = 1500,
  SMILE_IMAGE = 800,
  JUMPING_MAN_IMAGE = 1000,
  FIREBALL_IMAGE = 700,
  EXPLOSION_IMAGE = 250
};

struct LetterAnimState
{
  uint8_t phase = 0;
  uint8_t step = 0;
  uint8_t currentLetter = 0;
  uint32_t prevMillis = 0;
};

static const uint32_t *const heartImage[] = {heart_1, heart_2, heart_3, heart_4};
static const uint32_t *const smileImage[] = {smile_1, smile_2, smile_3, smile_4};
static const uint32_t *const jumpingManImage[] = {jumpingMan_1, jumpingMan_2};
static const uint32_t *const fireballImage[] = {fireball_1, fireball_2, fireball_3, fireball_4};
static const uint32_t *const explosionImage[] = {explosion_1, explosion_2, explosion_3, explosion_4, explosion_5,
                                                 explosion_6, explosion_7, explosion_8, explosion_9, explosion_10, explosion_11, explosion_12,
                                                 explosion_13, explosion_14, explosion_15, explosion_16, explosion_17, explosion_18};
static const uint32_t *const jpLetters[] = {jpLetter_1, jpLetter_2, jpLetter_3, jpLetter_4, jpLetter_5,
                                            jpLetter_6, jpLetter_7, jpLetter_8, jpLetter_9, jpLetter_10};
static const uint32_t *const krLetters[] = {krLetter_1, krLetter_2, krLetter_3, krLetter_4, krLetter_5};

static const uint32_t *const signalsImage[] = {signal_1, signal_2, signal_3, signal_4, signal_5, signal_6, signal_7};

const uint8_t mainMatrixScheme[] = {
    132, 131, 108, 107, 84, 83, 60, 59, 36, 35, 12, 11,
    133, 130, 109, 106, 85, 82, 61, 58, 37, 34, 13, 10,
    134, 129, 110, 105, 86, 81, 62, 57, 38, 33, 14, 9,
    135, 128, 111, 104, 87, 80, 63, 56, 39, 32, 15, 8,
    136, 127, 112, 103, 88, 79, 64, 55, 40, 31, 16, 7,
    137, 126, 113, 102, 89, 78, 65, 54, 41, 30, 17, 6,
    138, 125, 114, 101, 90, 77, 66, 53, 42, 29, 18, 5,
    139, 124, 115, 100, 91, 76, 67, 52, 43, 28, 19, 4,
    140, 123, 116, 99, 92, 75, 68, 51, 44, 27, 20, 3,
    141, 122, 117, 98, 93, 74, 69, 50, 45, 26, 21, 2,
    142, 121, 118, 97, 94, 73, 70, 49, 46, 25, 22, 1,
    143, 120, 119, 96, 95, 72, 71, 48, 47, 24, 23, 0};

static LetterAnimState stateJp;
static LetterAnimState stateKr;

static void drawAnimation(const uint32_t *const *images, uint32_t numImages, uint32_t interval, bool isNewAnim);
static void shiftLeft(uint8_t a);
static void drawShifted(const uint32_t *image, int8_t offsetY, bool append);
static void processLetters(LetterAnimState &st, const uint32_t *const letters[], uint8_t totalLetters);

void drawAnimations(uint8_t subMode)
{
  static uint8_t currentSubMode = 255;
  switch (subMode)
  {
  case 0:
    drawAnimation(heartImage, 4, HEART_IMAGE, currentSubMode == 0 ? false : true);
    break;

  case 1:
    drawAnimation(smileImage, 4, SMILE_IMAGE, currentSubMode == 1 ? false : true);
    break;

  case 2:
    drawAnimation(jumpingManImage, 2, JUMPING_MAN_IMAGE, currentSubMode == 2 ? false : true);
    break;

  case 3:
    drawAnimation(fireballImage, 4, FIREBALL_IMAGE, currentSubMode == 3 ? false : true);
    break;

  case 4:
    drawAnimation(explosionImage, 18, EXPLOSION_IMAGE, currentSubMode == 4 ? false : true);
    break;

  case 5:
    processLetters(stateJp, jpLetters, 10);
    break;

  case 6:
    processLetters(stateKr, krLetters, 5);
    break;

  case 7:
    shiftLeft(0);
    break;

  case 8:
    shiftLeft(1);
    break;

  case 9:
    shiftLeft(2);
    break;

  case 10:
    shiftLeft(3);
    break;

  case 11:
    shiftLeft(4);
    break;

  case 12:
    shiftLeft(5);
    break;

  case 13:
    shiftLeft(6);
    break;

  case 255:
    stateJp = {0, 0, 0, 0};
    stateKr = {0, 0, 0, 0};
    break;

  default:
    break;
  }

  currentSubMode = subMode;
}

void drawPicture(const uint8_t p1[], const uint32_t *p2)
{
  if (p2 == NULL)
    return;
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    uint8_t targetLed = p1[i];
    if (targetLed < MATRIX_LEDS)
    {
      uint32_t rawColor;
      memcpy_P(&rawColor, &p2[i], sizeof(uint32_t));
      leds[targetLed] = CRGB(rawColor);
    }
  }
}

uint16_t XY(uint16_t x, uint16_t y)
{
  if (x % 2 == 0)
  {
    return x * MATRIX_HEIGHT + y;
  }
  else
  {
    return x * MATRIX_HEIGHT + (MATRIX_HEIGHT - 1 - y);
  }
}

static void drawShifted(const uint32_t *image, int8_t offsetY, bool append)
{
  if (image == NULL)
    return;
  if (!append)
    FastLED.clear();

  for (int8_t row = 0; row < 12; row++)
  {
    int8_t imgRow = row + offsetY;
    if (imgRow < 0 || imgRow >= 12)
      continue;
    for (int8_t col = 0; col < 12; col++)
    {
      uint32_t color;
      memcpy_P(&color, &image[imgRow * 12 + col], sizeof(uint32_t));
      if (color > 0)
      {
        uint8_t ledIndex = pgm_read_byte(&mainMatrixScheme[row * 12 + col]);
        if (ledIndex < 144)
        {
          leds[ledIndex] = CRGB(color);
        }
      }
    }
  }
}

static void processLetters(LetterAnimState &st, const uint32_t *const letters[], uint8_t totalLetters)
{
  uint32_t currentMillis = millis();
  if (currentMillis - st.prevMillis < 120)
    return;
  st.prevMillis = currentMillis;

  const int8_t H = 12;
  const int8_t GAP = 3;

  switch (st.phase)
  {
  case 0:
    if (st.step <= H)
    {
      drawShifted(letters[0], H - st.step, false);
      st.step++;
    }
    else
    {
      st.phase = 1;
      st.step = 0;
      st.currentLetter = 0;
    }
    break;

  case 1:
    if (st.currentLetter < totalLetters - 1)
    {
      if (st.step <= (H + GAP))
      {
        drawShifted(letters[st.currentLetter], -st.step, false);
        int8_t nextOffset = (H + GAP) - st.step;
        if (nextOffset <= H)
        {
          drawShifted(letters[st.currentLetter + 1], nextOffset, true);
        }
        st.step++;
      }
      else
      {
        st.step = 0;
        st.currentLetter++;
      }
    }
    else
    {
      st.phase = 2;
      st.step = 0;
    }
    break;

  case 2:
    if (st.step < 12)
    {
      drawShifted(letters[totalLetters - 1], -(st.step), false);
      st.step++;
    }
    else
    {
      st.phase = 3;
    }
    break;

  case 3:
    if (st.step <= H)
    {
      drawShifted(letters[totalLetters - 1], -st.step, false);
      st.step++;
    }
    else
    {
      st.phase = 0;
      st.step = 0;
      st.currentLetter = 0;
    }
    break;
  }

  FastLED.show();
}

void shiftLeft(uint8_t a)
{
  static uint16_t shift = 0;
  static uint8_t lastA = 255;
  static uint32_t lastTime = 0;

  if (lastA != a)
  {
    shift = 0;
    lastA = a;
  }

  if (millis() - lastTime < 100)
    return;
  lastTime = millis();

  for (uint8_t row = 0; row < 12; row++)
  {
    for (uint8_t col = 0; col < 12; col++)
    {
      uint16_t index = (col + shift) % 12;
      uint32_t color;
      memcpy_P(&color, signalsImage[a] + (row * 12 + index), sizeof(uint32_t));
      leds[XY(col, row)] = CRGB(color);
    }
  }

  FastLED.show();

  shift = (shift + 1) % 12;
}

static void drawAnimation(const uint32_t *const *images, uint32_t numImages, uint32_t interval, bool isNewAnim)
{
  static uint32_t previousMillis = 0;
  static uint8_t currentImageIndex = 0;
  uint32_t currentMillis = millis();

  if (isNewAnim)
  {
    currentImageIndex = 0;
    previousMillis = currentMillis;
    drawPicture(mainMatrixScheme, images[0]);
    FastLED.show();
    currentImageIndex = 1;
    return;
  }

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    if (currentImageIndex >= numImages)
      currentImageIndex = 0;
    drawPicture(mainMatrixScheme, images[currentImageIndex]);
    FastLED.show();
    currentImageIndex = (currentImageIndex + 1) % numImages;
  }
}