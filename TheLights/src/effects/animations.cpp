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

static void drawAnimation(const uint32_t *images[], uint32_t numImages, uint32_t interval);
static void shiftLeft(uint8_t a);
static void displayJapaneseLetters(void);
static void displayKoreanLetters(void);
static void resetLettersAnimation();

static uint8_t currentStepJp = 0;
static uint8_t currentPhaseJp = 0;
static uint8_t currentLetterJp = 0;
static uint8_t currentStepKr = 0;
static uint8_t currentPhaseKr = 0;
static uint8_t currentLetterKr = 0;

static uint8_t downArray[144];
static uint8_t upArray[144];

uint8_t mainMatrixScheme[] = {
    133, 132, 109, 108, 85, 84, 61, 60, 37, 36, 13, 12,
    134, 131, 110, 107, 86, 83, 62, 59, 38, 35, 14, 11,
    135, 130, 111, 106, 87, 82, 63, 58, 39, 34, 15, 10,
    136, 129, 112, 105, 88, 81, 64, 57, 40, 33, 16, 9,
    137, 128, 113, 104, 89, 80, 65, 56, 41, 32, 17, 8,
    138, 127, 114, 103, 90, 79, 66, 55, 42, 31, 18, 7,
    139, 126, 115, 102, 91, 78, 67, 54, 43, 30, 19, 6, 
    140, 125, 116, 101, 92, 77, 68, 53, 44, 29, 20, 5,
    141, 124, 117, 100, 93, 76, 69, 52, 45, 28, 21, 4,
    142, 123, 118, 99,  94, 75, 70, 51, 46, 27, 22, 3,
    143, 122, 119, 98,  95, 74, 71, 50, 47, 26, 23, 2,
    144, 121, 120, 97,  96, 73, 72, 49, 48, 25, 24, 1};

void drawAnimations(uint8_t subMode)
{
  const uint32_t *heartImage[] = {heart_1, heart_2, heart_3, heart_4};
  const uint32_t *smileImage[] = {smile_1, smile_2, smile_3, smile_4};
  const uint32_t *jumpingManImage[] = {jumpingMan_1, jumpingMan_2};
  const uint32_t *fireballImage[] = {fireball_1, fireball_2, fireball_3, fireball_4};
  const uint32_t *explosionImage[] = {explosion_1, explosion_2, explosion_3, explosion_4, explosion_5,
                                      explosion_6, explosion_7, explosion_8, explosion_9, explosion_10, explosion_11, explosion_12,
                                      explosion_13, explosion_14, explosion_15, explosion_16, explosion_17, explosion_18};

  switch (subMode)
  {
  case 1:
    drawAnimation(heartImage, 4, HEART_IMAGE);
    break;

  case 2:
    drawAnimation(smileImage, 4, SMILE_IMAGE);
    break;

  case 3:
    drawAnimation(jumpingManImage, 2, JUMPING_MAN_IMAGE);
    break;

  case 4:
    drawAnimation(fireballImage, 4, FIREBALL_IMAGE);
    break;

  case 5:
    drawAnimation(explosionImage, 18, EXPLOSION_IMAGE);
    break;

  case 6:
    displayJapaneseLetters();
    break;

  case 7:
    displayKoreanLetters();
    break;

  case 8:
    shiftLeft(0);
    break;

  case 9:
    shiftLeft(1);
    break;

  case 10:
    shiftLeft(2);
    break;

  case 11:
    shiftLeft(3);
    break;

  case 12:
    shiftLeft(4);
    break;

  case 13:
    shiftLeft(5);
    break;

  case 14:
    shiftLeft(6);
    break;

  case 255:
    resetLettersAnimation();
    break;

  default:
    break;
  }
}

void drawPicture(uint8_t p1[], const uint32_t p2[])
{
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    if ((p1[i] - 1) <= 143)
    {
      uint32_t rawColor = pgm_read_dword_near(&p2[i]);
      leds[p1[i] - 1] = CRGB(rawColor);
    }
  }
}

static void shiftArrayUp(uint8_t upCount)
{
  for (uint16_t i = 0; i < MATRIX_LEDS - MATRIX_WIDTH; i++)
  {
    upArray[i] = upArray[i + MATRIX_WIDTH];
  }
  for (uint8_t j = 0; j < MATRIX_WIDTH; j++)
  {
    upArray[MATRIX_LEDS - MATRIX_WIDTH + j] = mainMatrixScheme[upCount * MATRIX_WIDTH + j];
  }
}

static void shiftArrayDown()
{
  for (uint16_t i = 0; i < MATRIX_LEDS - MATRIX_WIDTH; i++)
  {
    downArray[i] = downArray[i + MATRIX_WIDTH];
  }
  for (uint8_t j = 0; j < MATRIX_WIDTH; j++)
  {
    downArray[MATRIX_LEDS - MATRIX_WIDTH + j] = 145;
  }
}

static void initLettersArray()
{
  for (uint16_t i = 0; i < MATRIX_LEDS; i++)
  {
    upArray[i] = 145;
    downArray[i] = mainMatrixScheme[i];
  }
}

static void displayJapaneseLetters()
{
  const uint32_t *jpLetters[] = {jpLetter_1, jpLetter_2, jpLetter_3, jpLetter_4, jpLetter_5,
                                 jpLetter_6, jpLetter_7, jpLetter_8, jpLetter_9, jpLetter_10};

  uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;

  if (currentMillis - previousMillis >= 210)
  {
    previousMillis = currentMillis;

    switch (currentPhaseJp)
    {
    case 0:
      if (currentStepJp < 12)
      {
        FastLED.clear();
        shiftArrayUp(currentStepJp);
        drawPicture(upArray, jpLetter_1);
        FastLED.show();
        currentStepJp++;
      }
      else
      {
        currentPhaseJp = 1;
        currentStepJp = 0;
        currentLetterJp = 0;
        initLettersArray();
      }
      break;

    case 1:
      if (currentLetterJp < 9)
      {
        if (currentStepJp < 15)
        {
          FastLED.clear();
          if (currentStepJp < 12)
          {
            shiftArrayDown();
            drawPicture(downArray, jpLetters[currentLetterJp]);
          }
          if (currentStepJp >= 3)
          {
            shiftArrayUp(currentStepJp - 3);
            drawPicture(upArray, jpLetters[currentLetterJp + 1]);
          }
          FastLED.show();

          currentStepJp++;
        }
        else
        {
          currentStepJp = 0;
          currentLetterJp++;
          initLettersArray();
        }
      }
      else
      {
        currentPhaseJp = 2;
        currentStepJp = 0;
        initLettersArray();
      }
      break;

    case 2:
      if (currentStepJp < 12)
      {
        FastLED.clear();
        shiftArrayDown();
        drawPicture(downArray, jpLetter_10);
        FastLED.show();
        currentStepJp++;
      }
      else
      {
        currentPhaseJp = 3;
        currentStepJp = 0;
      }
      break;

    case 3:
      delay(2000);
      currentPhaseJp = 0;
      break;
    }
  }
}

void displayKoreanLetters()
{
  const uint32_t *krLetters[] = {krLetter_1, krLetter_2, krLetter_3, krLetter_4, krLetter_5};

  uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;

  if (currentMillis - previousMillis >= 210)
  {
    previousMillis = currentMillis;

    switch (currentPhaseKr)
    {
    case 0:
      if (currentStepKr < 12)
      {
        FastLED.clear();
        shiftArrayUp(currentStepKr);
        drawPicture(upArray, krLetter_1);
        FastLED.show();
        currentStepKr++;
      }
      else
      {
        currentPhaseKr = 1;
        currentStepKr = 0;
        currentLetterKr = 0;
        initLettersArray();
      }
      break;

    case 1:
      if (currentLetterKr < 4)
      {
        if (currentStepKr < 15)
        {
          FastLED.clear();
          if (currentStepKr < 12)
          {
            shiftArrayDown();
            drawPicture(downArray, krLetters[currentLetterKr]);
          }
          if (currentStepKr >= 3)
          {
            shiftArrayUp(currentStepKr - 3);
            drawPicture(upArray, krLetters[currentLetterKr + 1]);
          }
          FastLED.show();
          currentStepKr++;
        }
        else
        {
          currentStepKr = 0;
          currentLetterKr++;
          initLettersArray();
        }
      }
      else
      {
        currentPhaseKr = 2;
        currentStepKr = 0;
        initLettersArray();
      }
      break;

    case 2:
      if (currentStepKr < 12)
      {
        FastLED.clear();
        shiftArrayDown();
        drawPicture(downArray, krLetter_5);
        FastLED.show();
        currentStepKr++;
      }
      else
      {
        currentPhaseKr = 3;
        currentStepKr = 0;
      }
      break;

    case 3:
      delay(2000);
      currentPhaseKr = 0;
      break;
    }
  }
}

void shiftLeft(uint8_t a)
{
  const uint32_t *signalsImage[] = {signal_1, signal_2, signal_3, signal_4, signal_5, signal_6, signal_7};

  static uint16_t shift = 0;
  static uint32_t lastTime = 0;

  if (millis() - lastTime < 100) {
    return;
  }
  lastTime = millis();

  for (uint8_t row = 0; row < 12; row++)
  {
    for (uint8_t col = 0; col < 12; col++)
    {
      uint16_t index = (col + shift) % 12;
      uint32_t color = pgm_read_dword_near(&(signalsImage[a][row * 12 + index]));
      leds[XY(col, row)] = CRGB(color);
    }
  }

  FastLED.show();
  
  shift = (shift + 1) % 12;
}

void drawAnimation(const uint32_t *images[], uint32_t numImages, uint32_t interval)
{
  static uint32_t previousMillisJP = 0;
  static uint8_t currentImageIndex = 0;

  static bool initialized = false;

  uint32_t currentMillis = millis();

  if (!initialized || (currentMillis - previousMillisJP >= interval))
  {
    initialized = true;
    previousMillisJP = currentMillis;
    drawPicture(mainMatrixScheme, images[currentImageIndex]);
    FastLED.show();
    currentImageIndex = (currentImageIndex + 1) % numImages;
  }
}

void resetLettersAnimation()
{
  initLettersArray();

  currentStepJp = 0;
  currentPhaseJp = 0;
  currentLetterJp = 0;
  currentStepKr = 0;
  currentPhaseKr = 0;
  currentLetterKr = 0;
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