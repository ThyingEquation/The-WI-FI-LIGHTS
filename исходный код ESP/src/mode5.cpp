#include <mode5.h>


// W1 заливка змейкой быстрая, W2 заливка змейкой медленно (обычная скорость), W3 заливка экрана полностью 
// W4 заливка экрана змейкой секционная "хамелеон" (разные цвета), W5 заливка экрана секционная (разные цвета)

//srand( time(0) );
int chameleon [] = {3, 20, 5, 10, 5, 1, 3, 6, 2, 8, 7, 2, 15, 8, 3, 7, 4, 10, 2, 15, 6, 8, 2, 4}; // длина массива 24
// int chameleon [] = {3, 20, 5, 10, 5, 1, 3, 6, 2, 8, 7, 2, 15, 8, 3, 7, 4, 10, 2, 15, 6, 8, 2, 4,
//3, 1, 10, 13, 5, 20, 1, 14, 7, 9, 3, 11, 2, 1}; // длина массива 38 для 16х16
// SIZE = sizeof(chameleon) / sizeof(*chameleon); 
unsigned char il = 0;
unsigned char cha = 0;
unsigned char randcol = 0;

const uint32_t colorsPixel[] = { strip.Color(93, 138, 168),   // Военно-воздушный синий    // Английский алфавит. Буква А
                            strip.Color(227, 38, 54),    // Ализариновый красный
                            strip.Color(255, 191, 0),    // Янтарный
                            strip.Color(153, 102, 204),  // Аметистовый
                            strip.Color(242, 243, 244),  // Матовый белый                           
                            strip.Color(141, 182, 0),    // Яблочно-зелёный                         
                            strip.Color(0, 255, 255),    // Цвет морской волны
                            strip.Color(75, 83, 32),     // Армейский зелёный
                            strip.Color(254, 111, 94),   // Горький розовый  
                            strip.Color(0, 0, 255),      // Синий
                            strip.Color(204, 0, 0),      // Красный Университета Бостона
                            strip.Color(181, 166, 66),   // Латунный
                            strip.Color(102, 255, 0),    // Ярко-зелёный
                            strip.Color(8, 232, 222),    // Ярко-бирюзовый
                            strip.Color(205, 0, 205),    // Ярко-фиолетовый      
                            strip.Color(144, 0, 32),     // Бургундский   
                            strip.Color(233, 116, 81),   // Сиена жжёная
                            strip.Color(119, 41, 83),    // Баклажановый
                            strip.Color(196, 30, 58),    // Тёмно-красный, кардинал
                            strip.Color(172, 225, 175),  // Селадоновый
                            strip.Color(222, 49, 99),    // Светлая вишня
                            strip.Color(42, 82, 190),    // Лазурно-синий
                            strip.Color(127, 255, 0),    // Салатовый цвет, шартрез
                            strip.Color(251, 236, 93),   // 	Кукурузный
                            strip.Color(220, 20, 60),    // Тёмно-красный, кровавый, Малиновый
                            strip.Color(0, 255, 255),    // Циан, васильковый
                            strip.Color(189, 183, 107),  // Тёмный хаки, хаки
                            strip.Color(3, 192, 60),     // Тёмный пастельно-зелёный
                            strip.Color(255, 218, 185),  // Тёмно-персиковый
                            strip.Color(231, 84, 128),   // Тёмно-розовый
                            strip.Color(0, 51, 153),     // Тёмно-Пороховая синь
                            strip.Color(233, 150, 122),  // Тёмная лососина
                            strip.Color(86, 3, 25),      // Тёмно-алый
                            strip.Color(23, 114, 69),    // Тёмный весенне-зелёный
                            strip.Color(255, 168, 18),   // Тёмно-мандариновый
                            strip.Color(66, 49, 137),    // Глубокий фиолетовый цвет
                            strip.Color(255, 20, 147),   // Глубокий розовый цвет
                            strip.Color(80, 200, 120),   // Изумрудный                                                               // Буква E
                            strip.Color(153, 0, 102),    // Баклажановый
                            strip.Color(178, 34, 34),    // Кирпичный
                            strip.Color(255, 0, 255),    // Фуксия
                            strip.Color(255, 215, 0),    // Золотой
                            strip.Color(223, 115, 255),  // Гелиотроп  
                            strip.Color(0, 255, 0),      // Зелёный
                            strip.Color(0, 125, 255),    // Синий Градуса
                            strip.Color(173, 255, 47),   // Жёлто-зелёный                                                           // Буква H
                            strip.Color(252, 15, 192),   // Ярко-розовый цвет
                            strip.Color(75, 0, 130),     // Индиго                                                                   // Буква I
                            strip.Color(255, 79, 0),     // Международный (сигнальный) оранжевый
                            strip.Color(205, 92, 92),    // Индийский красный, каштановый
                            strip.Color(0, 168, 107),    // Нефритовый                                                               // Буква J                                                                // Буква K
                            strip.Color(58, 117, 196),   // Синий Кляйна
                            strip.Color(230, 230, 250),  // Лаванда, лавандово-синий                                                // Буква L
                            strip.Color(204, 255, 0),    // Лайм
                            strip.Color(253, 233, 16),   // Лимонный
                            strip.Color(200, 162, 200),  // Сиреневый
                            strip.Color(250, 240, 230),  // Зелёная лужайка
                            strip.Color(255, 0, 255),    // Маджента, фуксия                                                          // Буква M
                            strip.Color(11, 218, 81),    // Малахитовая зелень (малахитовый)
                            strip.Color(128, 0, 0),      // Коричнево-малиновый
                            strip.Color(152, 255, 152),  // Зелёная мята (мятный)
                            strip.Color(153, 51, 102),   // Розовато-лиловый
                            strip.Color(0, 51, 102),     // Полуночно-синий
                            strip.Color(173, 223, 173),  // Зелёный мох
                            strip.Color(153, 122, 141),  // Розовый Маунтбэттена
                            strip.Color(255, 219, 88),   // Горчичный                                                           // Буква N
                            strip.Color(0, 0, 128),      // Тёмно-синий
                            strip.Color(204, 119, 34),   // Охра                                                                     // Буква O
                            strip.Color(107, 142, 35),   // Нежно-оливковый
                            strip.Color(255, 165, 0),    // Оранжевый
                            strip.Color(218, 112, 214),  // Орхидея
                            strip.Color(253, 245, 230),  // Старое кружево
                            strip.Color(175, 238, 238),  // Бледно-синий                                                           // Буква P
                            strip.Color(152, 118, 84),   // 	Бледно-коричневыйй
                            strip.Color(175, 64, 53),    // Бледно-карминный, ср. Кармин
                            strip.Color(221, 173, 175),  // Бледно-каштановый
                            strip.Color(249, 132, 229),  // Бледно-пурпурный
                            strip.Color(250, 218, 221),  // Бледно-розовый
                            strip.Color(219, 112, 147),  // Лиловый
                            strip.Color(218, 189, 171),  // Бледно-песочный цвет
                            strip.Color(240, 220, 130),  // Палевый
                            strip.Color(199, 252, 236),  // Панг
                            strip.Color(255, 239, 213),  // Побег папайи
                            strip.Color(119, 221, 119),  // Пастельно-зелёный
                            strip.Color(255, 209, 220),  // Пастельно-розовый
                            strip.Color(209, 226, 49),   // Грушевый
                            strip.Color(204, 20, 255),   // Барвинок
                            strip.Color(102, 0, 255),    // Персидский синий
                            strip.Color(1, 121, 111),    // 	Зелёная сосна
                            strip.Color(255, 192, 203),  // Розовый
                            strip.Color(255, 153, 102),  // Оранжево-розовый
                            strip.Color(102, 0, 102),    // Сливовый
                            strip.Color(204, 136, 153),  // Красновато-коричневый, пюсовый (устар.), тёмно-коричневый, блошиный
                            strip.Color(255, 117, 24),   // Тыква
                            strip.Color(128, 0, 128),    // Фиолетовый (оттенок)                                                                 // Буква R
                            strip.Color(255, 0, 0),      // Красный
                            strip.Color(199, 21, 133),   // Фиолетово-красный
                            strip.Color(65, 105, 225),   // Ярко-синий («королевский синий»)
                            strip.Color(255, 153, 0),    // Сигнальный оранжевый                                                    // Буква S 
                            strip.Color(8, 37, 103),     // Сапфировый
                            strip.Color(210, 134, 200),  // Салутовый
                            strip.Color(255, 55, 105),   // Лососёвый
                            strip.Color(146, 0, 10),     // Сангрия
                            strip.Color(255, 36, 0),     // Ярко-красный, алый
                            strip.Color(255, 216, 0),    // Цвет жёлтого школьного автобуса
                            strip.Color(46, 139, 87),    // Зелёное море
                            strip.Color(255, 245, 238),  //  Цвет морской ракушки
                            strip.Color(0, 255, 127),    // Зелёная весна
                            strip.Color(70, 130, 180),   // Синяя сталь
                            strip.Color(172, 183, 142),  // Болотный
                            strip.Color(210, 180, 140),  // Цвет загара                                                             // Буква T
                            strip.Color(205, 87, 0),     // Краснобуро-оранжевый
                            strip.Color(255, 204, 0),    // Мандариновый
                            strip.Color(208, 240, 192),  // Зелёный чай
                            strip.Color(0, 128, 128),    // Сине-зелёный
                            strip.Color(216, 191, 216),  // Чертополох
                            strip.Color(48, 213, 200),   // Бирюзовый, ср. бирюза
                            strip.Color(255, 99, 71),    // Томатный                                                        // Буква U
                            strip.Color(91, 146, 229),   // Цвет флага ООН
                            strip.Color(221, 72, 20),    //Оранжевый Ubuntu
                            strip.Color(243, 229, 171),  // Ванильный                                                               // Буква V
                            strip.Color(139, 0, 255),    // Фиолетовый
                            strip.Color(64, 130, 109),   // Зелень Гинье
                            strip.Color(255, 255, 255),  // Белый
                            strip.Color(201, 160, 220),  // Глициния
                            strip.Color(114, 47, 55),    // Вино                                    
                            strip.Color(255, 255, 0),    // Жёлтый                                                                  // Буква Y
                            strip.Color(235, 194, 175),  // Циннвальдит                                                             // Буква Z
                            strip.Color(0, 20, 168) };   // Кобальтовая синь


void mode5(String choosenMode) {
  if (choosenMode.indexOf("/W1") != -1) { /////////////////////////////////////////////////// 256 
    if (l <= 156) {
      strip.setPixelColor(l, colorsPixel[col]);
      strip.show();
      delay(6);
      l++;
    } else {
      if (++col >= 128) { col = 0; }
      l = 0;
    }
  }

  if (choosenMode.indexOf("/W2") != -1) { ////////////////////////////////////////////////////// 256 
    if (l <= 156) {
      strip.setPixelColor(l, colorsPixel[col]);
      strip.show();
      delay(25);
      l++;
    } else {
      if (++col >= 128) { col = 0; }
      l = 0;
    }
  }


  if (choosenMode.indexOf("/W3") != -1) {
    matrix.fillScreen(colorsMatrix[col]);
    matrix.show();
    delay(5000);
    if (++col >= 128) { col = 0; }
  }

  if (choosenMode.indexOf("/W4") != -1) { ////////////////////////////////////////////////////// 256 + массив

      cha = 0;
       il = 0; 

     for (int s = 0; s <= 23; s++) { ///////////////////////////////////////////////////// массив моего окна 23

      for (int g5 = il; g5 <= (il + chameleon[cha]); g5++) {

      strip.setPixelColor(g5, colorsPixel[randcol]);
      strip.show();
      delay(70);
     }
     il = (il + chameleon[cha]) + 1;
     cha++; 
     randcol = random(128);

  }

  }

  if (choosenMode.indexOf("/W5") != -1) { //////////////////////////////////////////////////////////////// 256
   for (int i = 0; i <= 104; i++) { ///////////////////////////////////////////// для моего окна тут 104 или 128 для 256 светодиодов
      strip.setPixelColor(i, colorsPixel[col]);
    }
     strip.show();
    delay(1000);
    for (int i = 104; i <= 156; i++) { ///////////////////////////////////////////// для моего окна тут 156 ..256 - 128
      strip.setPixelColor(i, colorsPixel[col + 1]);
    }
     strip.show();
    delay(1000);
    if (++col >= 128) { col = 0; }
  }

  if (choosenMode.indexOf("/W6") != -1) {
  SmokeBallsRoutine();
  FastLED.show();
  delay(40);
  }

}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// слайды
#define NUM_LEDS kMatrixWidth * kMatrixHeight
#define WAVES_AMOUNT 12 /////////////////////////////////////////////////////////// kMatrixWidth ...............16
uint16_t pos5[WAVES_AMOUNT];
float sSpeed[WAVES_AMOUNT];
uint8_t maxMin[WAVES_AMOUNT];
float speedfactor;
uint8_t waveColors[WAVES_AMOUNT];
uint16_t reg[WAVES_AMOUNT];
bool loadingFlag5 = true;
#define speed (125)
#define scale (10)
#define Clr (0)
#define run (1) // 0-linear 1-beatsin
#define SubPix (1)

uint16_t XY5 (uint8_t x, uint8_t y) {
  return (y * kMatrixHeight + x);
}

uint32_t getPixColor(uint32_t thisSegm)
{
  uint32_t thisPixel = thisSegm;
  if (thisPixel > NUM_LEDS - 1) return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b);
}

uint32_t getPixColorXY5(uint8_t x, uint8_t y)
{
  return getPixColor(XY5(x, y));
}

void drawPixelXYF5(float x, float y, CRGB color)
{
  // if (x < 0 || y < 0 || x > ((float)kMatrixWidth - 1) || y > ((float)kMatrixHeight - 1)) return;
  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
#define WU_WEIGHT(a,b) ((uint8_t) (((a)*(b)+(a)+(b))>>8))
  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy),
                   WU_WEIGHT(ix, yy), WU_WEIGHT(xx, yy)
                  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XY5(xn, yn)];
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    leds[XY5(xn, yn)] = clr;
  }
}

void shift() {
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = kMatrixHeight; y > 0; y -= speedfactor) {
      leds[XY5(x, y)] = getPixColorXY5(x, y - 1);
    }
  }
}
void clearing() {
  for (byte i = 0; i < kMatrixWidth; i++) {
    leds[XY5(i, 0)] = 0;
  }
}

static float fmap(const float x, const float in_min, const float in_max, const float out_min, const float out_max) {
  return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

float randomf(float min, float max)
{
  return fmap((float)random16(4095), 0.0, 4095.0, min, max);
}


void SmokeBallsRoutine() {
  if (loadingFlag5) {
    loadingFlag5 = false;
    randomSeed(millis());
    for (byte j = 0; j < WAVES_AMOUNT; j++) {
      reg[j] =  random((kMatrixWidth * 10) - ((kMatrixWidth / 3) * 20));
      sSpeed[j] = randomf(5., (float)(16 * kMatrixWidth)); //random(50, 16 * kMatrixWidth) / random(1, 10);
      maxMin[j] = random((kMatrixWidth / 4) * 10, (kMatrixWidth / 2) * 20);
      waveColors[j] = random(0, 9) * 28;
      pos5[j] = reg[j];
    }
  }
  speedfactor = fmap(speed, 1., 255., .02, .1);
  shift();
  if (Clr)
    clearing();
  fadeToBlackBy (leds, NUM_LEDS, 10);
  blur2d(leds, kMatrixWidth, kMatrixHeight, 20);
  for (byte j = 0; j < map(scale, 1, 16, 2, WAVES_AMOUNT); j++) {
    waveColors[j]++;
    if (run)
      pos5[j] = beatsin16((uint8_t)(sSpeed[j] * (speedfactor * .5)), reg[j], maxMin[j] + reg[j], waveColors[j] * 256, waveColors[j] * 8);
    else
      pos5[j] += (sSpeed[j] * (speedfactor * .5));
    if (SubPix)
      drawPixelXYF5((float)pos5[j] / 10., 0.05, ColorFromPalette(PartyColors_p, waveColors[j]));
    else
      leds[XY5(pos5[j] / 10, 0)] = ColorFromPalette(PartyColors_p, waveColors[j]);
  }
  EVERY_N_SECONDS(20) {
    for (byte j = 0; j < map(scale, 1, 16, 2, WAVES_AMOUNT); j++) {
      //reg[j] += random(-20,20);
      waveColors[j] += 28;
    }
  }
}




