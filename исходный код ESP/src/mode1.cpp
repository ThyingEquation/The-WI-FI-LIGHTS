#include <mode1.h>

unsigned char x = 0;
unsigned char y = 0;
unsigned char z = 0;

//extern String *pCommand = &command;

String lCommand = "";

// пробегаем со 156 светодиода до 1 + перебор цветов в 
// mode1_colors и с настройкой яркости (при нажатии на 10 секунд включается выбранная 
// яркость) в mode1_bright

void mode1(String *pCommand) {
//String pCommand
  lCommand = *pCommand;

  String sortString; 
  
  if (lCommand.indexOf("/A") != -1) {
    if (lCommand.indexOf("/257") != -1) {
      chooseColor(lCommand);
      for (int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, strip.Color(x, y, z));
      }
      strip.show();
      delay(150);
      lCommand = "";
    }

    else {
      sortString = lCommand; 
      sortString.remove(0,7);
      sortString.remove(3);
      if (sortString.indexOf("/") != -1) {
         sortString.remove(2);
      }
      int q = sortString.toInt();
      q--;
      chooseColor(lCommand);
      strip.setPixelColor(q, strip.Color(x, y, z));
      strip.show();
      delay(150); 
      lCommand = "";           
    }    
  } 

  else if (lCommand.indexOf("/B") != -1) {
    if (lCommand.indexOf("/257") != -1) {
        for (int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      delay(150);
      lCommand = "";
    }

    else {
      sortString = lCommand; 
      sortString.remove(0,7);
      sortString.remove(3);
      if (sortString.indexOf("/") != -1) {
         sortString.remove(2);
      }
      int q = sortString.toInt();
      q--; 
      chooseColor(lCommand);
      strip.setPixelColor(q, strip.Color(0, 0, 0));
      strip.show();
      delay(150);  
      lCommand = "";          
    }    
  }

  lCommand = "";

}

//////////////////////////////////////
// управление яркостью

void brightness(String *pCommand) {

  int i = 0;

  lCommand = *pCommand;

  if (lCommand.indexOf("/M100") != -1) {
    strip.setBrightness(255);
    strip.show();
    delay(50);
        while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

    else if (lCommand.indexOf("/M90") != -1) {
    strip.setBrightness(230);
    strip.show();
    delay(50);
        while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

    else if (lCommand.indexOf("/M80") != -1) {
    strip.setBrightness(204);
    strip.show();
    delay(50);
        while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }
  
  else if (lCommand.indexOf("/M70") != -1) {
    strip.setBrightness(179);
    strip.show();
    delay(50);
        while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

   else if (lCommand.indexOf("/M60") != -1) {
    strip.setBrightness(153);
    strip.show();
    delay(50);
        while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

   else if (lCommand.indexOf("/M50") != -1) {
    strip.setBrightness(128);
    strip.show();
    delay(50);
        while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

   else if (lCommand.indexOf("/M40") != -1) {
    strip.setBrightness(102);
    strip.show();
    delay(50);
        while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

    else if (lCommand.indexOf("/M30") != -1) {
    strip.setBrightness(77);
    strip.show();
    delay(50);
    while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

    else if (lCommand.indexOf("/M20") != -1) {
    strip.setBrightness(51);
    strip.show();
    delay(50);
    while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

    else if (lCommand.indexOf("/M10") != -1) {
    strip.setBrightness(26);
    strip.show();
    delay(50);
    while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

    else if (lCommand.indexOf("/M0") != -1) {
    strip.setBrightness(1);
    strip.show();
    delay(50);
     while (i < 11){
       delay (1000);
       i++;
    }
    i = 0;
    strip.setBrightness(51);
    strip.show();
    delay(50);
  }

}

///////////////////////////////////////////////////
// выбор цветов 

unsigned char chooseColor(String lCommand) {

 //lCommand = *pCommand;

  if (lCommand.indexOf("/C") != -1) {  //белый
    x = 255;
    y = 255;
    z = 255;
    return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/D") != -1) {  //синий
    x = 0;
    y = 0;
    z = 255;
     return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/E") != -1) {  //лаймовый
    x = 0;
    y = 255;
    z = 0;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/F") != -1) {  //желтый
    x = 255;
    y = 255;
    z = 0;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/G") != -1) {  //оранжевый
    x = 255;
    y = 165;
    z = 0;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/H") != -1) {  //красный
    x = 255;
    y = 0;
    z = 0;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/I") != -1) {  //малиновый
    x = 255;
    y = 20;
    z = 147;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/J") != -1) {  //фиолетово-баклажанный
    x = 148;
    y = 0;
    z = 211;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/K") != -1) {  //весенне-зеленый
    x = 0;
    y = 255;
    z = 127;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/L") != -1) {  //золотой
    x = 255;
    y = 215;
    z = 0;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/N") != -1) {  //морозное небо
    x = 0;
    y = 191;
    z = 255;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/O") != -1) {  //аквамариновый
    x = 127;
    y = 255;
    z = 212;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/P") != -1) {  //индиго
    x = 75;
    y = 0;
    z = 130;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/Q") != -1) {  //светлорозовый
    x = 255;
    y = 182;
    z = 193;
        return x;
    return y;
    return z;
  }

  else if (lCommand.indexOf("/R") != -1) {  //зеленый
    x = 0;
    y = 128;
    z = 0;
        return x;
    return y;
    return z;
  }

  return 0;
}