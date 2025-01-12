#include "weather.h"

static void weatherEffects(uint8_t pieceCount, uint8_t speed, int *currentCol,
                           int *currentRow, int color);

void weather(uint8_t subMode) {
  static int currentCols[][22] = {
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11},
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11, 9, 4, 2},
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11},
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 10, 11, 9, 4, 2, 0, 2, 5}};

  static int currentRows[][22] = {
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14},
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14, 4, 9, 12, 3},
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14},
      {1, 3, 2, 12, 6, 9, 2, 7, 11, 1, 10, 5, 8, 0, 14, 4, 9, 12, 3, 3, 6, 9}};

  static const int lengths[] = {15, 19, 15, 22};
  static const int delays[] = {180, 95, 60, 30};
  static const int colors[] = {0xf2f3f4, 0xf2f3f4, 0x007dff, 0x0014a8};

  int index = subMode - 1;
  weatherEffects(lengths[index], delays[index], currentCols[index],
                 currentRows[index], colors[index]);
}

void weatherEffects(uint8_t pieceCount, uint8_t speed, int *currentCol,
                    int *currentRow, int color) {
  auto drawPixels = [&](bool clear) {
    for (int i = 0; i < pieceCount; i++) {
      if ((currentRow[i] >= 0) && (currentRow[i] < mHeight)) {
        int pixelIndex = XY(currentCol[i], currentRow[i]);
        strip.setPixelColor(pixelIndex, clear ? 0x000000 : color);
      }
    }
    strip.show();
  };

  drawPixels(false);
  delay(speed);

  drawPixels(true);

  for (int i = 0; i < pieceCount; i++) {
    if (currentRow[i] > 0) {
      --currentRow[i];
    } else {
      currentCol[i] = rand() % mWidth;
      currentRow[i] = mHeight + rand() % 5;
    }
  }
}