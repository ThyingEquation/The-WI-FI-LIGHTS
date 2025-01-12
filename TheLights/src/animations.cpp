#include "animations.h"
#include "pictures.h"

static void japaneseLettersAnimation(void);
static void koreanLettersAnimation(void);
static void leftShiftAnimation(uint8_t a);
static void displayAnimation(const int *images[], int numImages,
                             unsigned long interval);
static void resetLettersAnimations();

static uint8_t currentStepJp = 0;
static uint8_t currentPhaseJp = 0;
static uint8_t currentLetterJp = 0;
static uint8_t currentStepKr = 0;
static uint8_t currentPhaseKr = 0;
static uint8_t currentLetterKr = 0;

static uint8_t ArrDown[156];
static uint8_t ArrUp[156];

uint8_t mainScheme[] = {
    144, 143, 118, 117, 92,  91,  66,  65,  40,  39,  14,  13,  145, 142, 119,
    116, 93,  90,  67,  64,  41,  38,  15,  12,  146, 141, 120, 115, 94,  89,
    68,  63,  42,  37,  16,  11,  147, 140, 121, 114, 95,  88,  69,  62,  43,
    36,  17,  10,  148, 139, 122, 113, 96,  87,  70,  61,  44,  35,  18,  9,
    149, 138, 123, 112, 97,  86,  71,  60,  45,  34,  19,  8,   150, 137, 124,
    111, 98,  85,  72,  59,  46,  33,  20,  7,   151, 136, 125, 110, 99,  84,
    73,  58,  47,  32,  21,  6,   152, 135, 126, 109, 100, 83,  74,  57,  48,
    31,  22,  5,   153, 134, 127, 108, 101, 82,  75,  56,  49,  30,  23,  4,
    154, 133, 128, 107, 102, 81,  76,  55,  50,  29,  24,  3,   155, 132, 129,
    106, 103, 80,  77,  54,  51,  28,  25,  2,   156, 131, 130, 105, 104, 79,
    78,  53,  52,  27,  26,  1};

void animations(uint8_t subMode) {
  const int *heartImages[] = {heart1, heart2, heart3, heart4};
  const int *smileImages[] = {smile1, smile2, smile3, smile4};
  const int *jumpingManImages[] = {jumpingMan1, jumpingMan2};
  const int *fireballImages[] = {fireball1, fireball2, fireball3, fireball4};
  const int *explosionImages[] = {
      explosion1,  explosion2,  explosion3,  explosion4,  explosion5,
      explosion6,  explosion7,  explosion8,  explosion9,  explosion10,
      explosion11, explosion12, explosion13, explosion14, explosion15,
      explosion16, explosion17, explosion18};

  switch (subMode) {
    case 1:
      displayAnimation(heartImages, 4, 1500);
      break;

    case 2:
      displayAnimation(smileImages, 4, 800);
      break;

    case 3:
      displayAnimation(jumpingManImages, 2, 1000);
      break;

    case 4:
      displayAnimation(fireballImages, 4, 700);
      break;

    case 5:
      displayAnimation(explosionImages, 18, 250);
      break;

    case 6:
      japaneseLettersAnimation();
      break;

    case 7:
      koreanLettersAnimation();
      break;

    case 8:
      leftShiftAnimation(0);
      break;

    case 9:
      leftShiftAnimation(1);
      break;

    case 10:
      leftShiftAnimation(2);
      break;

    case 11:
      leftShiftAnimation(3);
      break;

    case 12:
      leftShiftAnimation(4);
      break;

    case 13:
      leftShiftAnimation(5);
      break;

    case 14:
      leftShiftAnimation(6);
      break;

    case 99:
      resetLettersAnimations();
      break;

      // case 6:
      //   displayAnimation(pacmanImages, 5, 1500);
      //   break;

      // case 9:
      //   displayAnimation(diffImages, 9, 1500);
      //   break;

    default:
      break;
  }
}

void mainFunAnime(uint8_t p1[], const int p2[]) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((p1[i] - 1) <= 155) {
      strip.setPixelColor(p1[i] - 1, pgm_read_dword(&(p2[i])));
    }
  }
}

static void shiftArrUp(uint8_t upCount) {
  for (int i = 0; i < NUM_LEDS - mWidth; i++) {
    ArrUp[i] = ArrUp[i + mWidth];
  }
  for (int j = 0; j < mWidth; j++) {
    ArrUp[NUM_LEDS - mWidth + j] = mainScheme[upCount * mWidth + j];
  }
}

static void shiftArrDown() {
  for (int i = 0; i < NUM_LEDS - mWidth; i++) {
    ArrDown[i] = ArrDown[i + mWidth];
  }
  for (int j = 0; j < mWidth; j++) {
    ArrDown[NUM_LEDS - mWidth + j] = 157;
  }
}

static void initArrLetters() {
  for (int i = 0; i < NUM_LEDS; i++) {
    ArrUp[i] = 157;
    ArrDown[i] = mainScheme[i];
  }
}

void japaneseLettersAnimation() {
  const int *jpL[] = {jpLetter1, jpLetter2, jpLetter3, jpLetter4, jpLetter5,
                      jpLetter6, jpLetter7, jpLetter8, jpLetter9, jpLetter10};

  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

  if (currentMillis - previousMillis >= 210) {
    previousMillis = currentMillis;

    switch (currentPhaseJp) {
      case 0:
        if (currentStepJp < 13) {
          strip.clear();
          shiftArrUp(currentStepJp);
          mainFunAnime(ArrUp, jpLetter1);
          strip.show();
          currentStepJp++;
        } else {
          currentPhaseJp = 1;
          currentStepJp = 0;
          currentLetterJp = 0;
          initArrLetters();
        }
        break;

      case 1:
        if (currentLetterJp < 9) {
          if (currentStepJp < 16) {
            strip.clear();
            if (currentStepJp < 13) {
              shiftArrDown();
              mainFunAnime(ArrDown, jpL[currentLetterJp]);
            }
            if (currentStepJp >= 3) {
              shiftArrUp(currentStepJp - 3);
              mainFunAnime(ArrUp, jpL[currentLetterJp + 1]);
            }
            strip.show();
            currentStepJp++;
          } else {
            currentStepJp = 0;
            currentLetterJp++;
            initArrLetters();
          }
        } else {
          currentPhaseJp = 2;
          currentStepJp = 0;
          initArrLetters();
        }
        break;

      case 2:
        if (currentStepJp < 13) {
          strip.clear();
          shiftArrDown();
          mainFunAnime(ArrDown, jpLetter10);
          strip.show();
          currentStepJp++;
        } else {
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

void koreanLettersAnimation() {
  const int *krL[] = {krLetter1, krLetter2, krLetter3, krLetter4, krLetter5};

  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

  if (currentMillis - previousMillis >= 210) {
    previousMillis = currentMillis;

    switch (currentPhaseKr) {
      case 0:
        if (currentStepKr < 13) {
          strip.clear();
          shiftArrUp(currentStepKr);
          mainFunAnime(ArrUp, krLetter1);
          strip.show();
          currentStepKr++;
        } else {
          currentPhaseKr = 1;
          currentStepKr = 0;
          currentLetterKr = 0;
          initArrLetters();
        }
        break;

      case 1:
        if (currentLetterKr < 4) {
          if (currentStepKr < 16) {
            strip.clear();
            if (currentStepKr < 13) {
              shiftArrDown();
              mainFunAnime(ArrDown, krL[currentLetterKr]);
            }
            if (currentStepKr >= 3) {
              shiftArrUp(currentStepKr - 3);
              mainFunAnime(ArrUp, krL[currentLetterKr + 1]);
            }
            strip.show();
            currentStepKr++;
          } else {
            currentStepKr = 0;
            currentLetterKr++;
            initArrLetters();
          }
        } else {
          currentPhaseKr = 2;
          currentStepKr = 0;
          initArrLetters();
        }
        break;

      case 2:
        if (currentStepKr < 13) {
          strip.clear();
          shiftArrDown();
          mainFunAnime(ArrDown, krLetter5);
          strip.show();
          currentStepKr++;
        } else {
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

void leftShiftAnimation(uint8_t a) {
  const int *signalsImages[] = {signal1, signal2, signal3, signal4,
                                signal5, signal6, signal7};

  static int shift = 0;

  for (int row = 0; row < 13; row++) {
    for (int col = 0; col < 12; col++) {
      int index = (col + shift) % 12;
      long color = pgm_read_dword(&(signalsImages[a][row * 12 + index]));
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
      strip.setPixelColor(XY(col, row), strip.Color(r, g, b));
    }
  }
  strip.show();
  delay(100);

  shift = (shift + 1) % 12;
}

void displayAnimation(const int *images[], int numImages,
                      unsigned long interval) {
  static unsigned long previousMillisJP = 0;
  static uint8_t currentImageIndex = 0;

  static bool initialized = false;

  unsigned long currentMillis = millis();

  if (!initialized || (currentMillis - previousMillisJP >= interval)) {
    initialized = true;
    previousMillisJP = currentMillis;
    mainFunAnime(mainScheme, images[currentImageIndex]);
    strip.show();
    currentImageIndex = (currentImageIndex + 1) % numImages;
  }
}

void resetLettersAnimations() {
  initArrLetters();

  currentStepJp = 0;
  currentPhaseJp = 0;
  currentLetterJp = 0;

  currentStepKr = 0;
  currentPhaseKr = 0;
  currentLetterKr = 0;
}
