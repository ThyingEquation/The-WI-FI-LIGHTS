#include "matrixMovie.h"

static void matrixFallingLines();

void matrixMovie(uint8_t subMode) {
  switch (subMode) {
    case 1:
      matrixFallingLines();
      break;
  }
}

void matrixFallingLines() {
  static int currentCol[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 6};
  static int currentRow[11] = {7, 8, 5, 6, 13, 2, 5, 11, 8, 7, 11};
  static int lineLength[11] = {9, 6, 5, 7, 8, 9, 7, 5, 7, 8, 5};

  auto drawLine = [](int col, int row, int length, bool clear) {
    for (int j = 0; j < length; ++j) {
      int rowIndex = row - j;
      if (rowIndex >= 0 && rowIndex < mHeight) {
        int pixelIndex = XY(col, rowIndex);
        if (clear) {
          leds[pixelIndex] = CRGB::Black;
        } else {
          int brightness = (j == length - 1) ? 255 : 5 + (5 * j);
          leds[pixelIndex] = hsv2rgb_spectrum(CHSV(90, 255, brightness));
        }
      }
    }
  };

  for (int i = 0; i < 11; i++) {
    drawLine(currentCol[i], currentRow[i], lineLength[i], false);
  }
  FastLED.show();
  delay(120);

  for (int i = 0; i < 11; i++) {
    drawLine(currentCol[i], currentRow[i], lineLength[i], true);
  }
  FastLED.show();

  for (int i = 0; i < 11; i++) {
    if (currentRow[i] > 0) {
      --currentRow[i];
    } else {
      currentCol[i] = rand() % mWidth;
      currentRow[i] = mHeight + 8 + rand() % (mHeight + 3);
      lineLength[i] = 5 + rand() % 5;
    }
  }
}