#include <canvas.h>

unsigned char x = 0;
unsigned char y = 0;
unsigned char z = 0;

void canvas(String choosenMode) {
  String sortString;

  if (choosenMode.indexOf("A") != -1) {
    if (choosenMode.indexOf("257") != -1) {
      chooseColor(choosenMode);
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(x, y, z));
      }
      strip.show();
      delay(150);
      choosenMode = "";
    }

    else {
      sortString = choosenMode;
      sortString.remove(0, 2);
      int ledNum = sortString.toInt();
      ledNum--;
      chooseColor(choosenMode);
      strip.setPixelColor(ledNum, strip.Color(x, y, z));
      strip.show();
      delay(150);
      choosenMode = "";
    }
  }

  else if (choosenMode.indexOf("B") != -1) {
    if (choosenMode.indexOf("257") != -1) {
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      delay(150);
      choosenMode = "";
    }

    else {
      sortString = choosenMode;
      sortString.remove(0, 2);
      int ledNum = sortString.toInt();
      ledNum--;
      chooseColor(choosenMode);
      strip.setPixelColor(ledNum, strip.Color(0, 0, 0));
      strip.show();
      delay(150);
      choosenMode = "";
    }
  }

  choosenMode = "";
}

unsigned char chooseColor(String choosenMode) {
  if (choosenMode.indexOf("C") != -1) {  // белый
    x = 255;
    y = 255;
    z = 255;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("D") != -1) {  // синий
    x = 0;
    y = 0;
    z = 255;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("E") != -1) {  // лаймовый
    x = 0;
    y = 255;
    z = 0;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("F") != -1) {  // желтый
    x = 255;
    y = 255;
    z = 0;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("G") != -1) {  // оранжевый
    x = 255;
    y = 165;
    z = 0;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("H") != -1) {  // красный
    x = 255;
    y = 0;
    z = 0;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("I") != -1) {  // малиновый
    x = 255;
    y = 20;
    z = 147;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("J") != -1) {  // фиолетово-баклажанный
    x = 148;
    y = 0;
    z = 211;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("K") != -1) {  // весенне-зеленый
    x = 0;
    y = 255;
    z = 127;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("L") != -1) {  // золотой
    x = 255;
    y = 215;
    z = 0;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("N") != -1) {  // морозное небо
    x = 0;
    y = 191;
    z = 255;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("O") != -1) {  // аквамариновый
    x = 127;
    y = 255;
    z = 212;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("P") != -1) {  // индиго
    x = 75;
    y = 0;
    z = 130;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("Q") != -1) {  // светлорозовый
    x = 255;
    y = 182;
    z = 193;
    return x;
    return y;
    return z;
  }

  else if (choosenMode.indexOf("R") != -1) {  // зеленый
    x = 0;
    y = 128;
    z = 0;
    return x;
    return y;
    return z;
  }

  return 0;
}