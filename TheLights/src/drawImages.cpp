#include "drawImages.h"
#include "image.h"

extern uint8_t mainScheme[];

static void imageDisplay(uint8_t arr[], const int img2[]);

void drawImages(uint8_t subMode) {
  const int *images[] = {pacman1,  pacman2, pacman3, pacman4,   pacman5,
                         mushroom, amogus,  cup,     pineapple, alien,
                         hummer,   cat,     teaCup,  dino,      hammerAndSickle,
                         apple,    bird,    rabbit,  question,  goldenKey,
                         star,     sun,     pepe};

  if (subMode == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, 0x000000);
    }
    strip.show();

  } else if (subMode > 0 && subMode < 24) {
    imageDisplay(mainScheme, images[subMode - 1]);
  }
}

void imageDisplay(uint8_t arr[], const int img2[]) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((arr[i] - 1) <= 155) {
      strip.setPixelColor(arr[i] - 1, pgm_read_dword(&(img2[i])));
    }
  }
  strip.show();
}