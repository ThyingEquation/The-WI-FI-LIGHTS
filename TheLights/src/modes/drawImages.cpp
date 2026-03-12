#include "modes.h"
#include "image.h"

/*
  Эта группа эффектов только для матрицы 12х13

  Настраиваемые параметры: нет
*/

void drawImages(uint8_t subMode)
{
  const uint32_t *images[] = {pacman1, pacman2, pacman3, pacman4, pacman5,
                         mushroom, amogus, cup, pineapple, alien,
                         hummer, cat, teaCup, dino, hammerAndSickle,
                         apple, bird, rabbit, question, goldenKey,
                         star, sun, pepe};

  if (subMode == 0)
  {
    for (uint16_t i = 0; i < MATRIX_LEDS; i++)
    {
      strip.setPixelColor(i, 0x000000);
    }
    strip.show();
  }
  else if (subMode > 0 && subMode < 24)
  {
    drawPicture(mainMatrixScheme, images[subMode - 1]);
    strip.show();
  }
}