#include <mode2.h>

 // вывод бегущей строки, чтобы было ОК, нужно в библиотеке Adafruit_GFX_library 
 // gfxfont.h заменить на файл скаченный с arduino.kid матрица 16х16.
 // для точки отсчета брать if (--g < -125), где 125 зависит от длины текста, так например 125 при длине текста 21



 #if MATRIX_SIZE_TYPE == 11
    char textLocation = 5; 
#elif MATRIX_SIZE_TYPE == 12
    char textLocation = 5; ;
#elif MATRIX_SIZE_TYPE == 21
    char textLocation = 3; 
#elif MATRIX_SIZE_TYPE == 22
    char textLocation = 3; 
#else
    char textLocation = 1;
#endif

void mode2(String choosenMode) {

  if (choosenMode.indexOf("/T1") != -1) {

  matrix.fillScreen(matrix.Color(0, 0, 0));
  matrix.setCursor(g, textLocation);  // Отступ сверху
  matrix.print(utf8rus("С НОВЫМ 2023 ГОДОМ!!!"));
  if (--g < -125) {  
    g = matrix.width();
   // if (++pass >= 128) { pass = 0; }  // всего в main для матрицы записано 128 цветов
   pass = random(127);
    matrix.setTextColor(colorsMatrix[pass]);
  }
  matrix.show();  
  delay(250);
   b++;
  }

    if (choosenMode.indexOf("/T2") != -1) {

  matrix.fillScreen(matrix.Color(0, 0, 0));
  matrix.setCursor(g, textLocation);  // Отступ сверху
  matrix.print(utf8rus("ЗДЕСЬ МОГЛА БЫТЬ ВАША РЕКЛАМА"));
  if (--g < -173) {  
    g = matrix.width();
    //if (++pass >= 128) { pass = 0; }  // всего в main для матрицы записано 128 цветов
    pass = random(127);
    matrix.setTextColor(colorsMatrix[pass]);
  }
  matrix.show();  
  delay(250);
   b++;
  }

      if (choosenMode.indexOf("/T3") != -1) {

  matrix.fillScreen(matrix.Color(0, 0, 0));
  matrix.setCursor(g, textLocation);  // Отступ сверху
  matrix.print(utf8rus("ПРИВЕТ, Я УМНАЯ ГИРЛЯНДА"));
  if (--g < -137) {  
    g = matrix.width();
    //if (++pass >= 128) { pass = 0; }  // всего в main для матрицы записано 128 цветов
    pass = random(127);
    matrix.setTextColor(colorsMatrix[pass]);
  }
  matrix.show();  
  delay(250);
   b++;
  }

      if (choosenMode.indexOf("/T4") != -1) {

  matrix.fillScreen(matrix.Color(0, 0, 0));
  matrix.setCursor(g, textLocation);  // Отступ сверху
  matrix.print(utf8rus("ВВЕДИТЕ ТЕКСТ"));
  if (--g < -85) {  
    g = matrix.width();
   // if (++pass >= 128) { pass = 0; }  // всего в main для матрицы записано 128 цветов
    pass = random(127);
    matrix.setTextColor(colorsMatrix[pass]);
  }
  matrix.show();  
  delay(250);
   b++;
  }
}

String utf8rus(String source) {
  int a, k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length();
  a = 0;

  while (a < k) {
    n = source[a];
    a++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0:
          {
            n = source[a];
            a++;
            if (n == 0x81) {
              n = 0xA8;
              break;
            }
            if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
            break;
          }
        case 0xD1:
          {
            n = source[a];
            a++;
            if (n == 0x91) {
              n = 0xB8;
              break;
            }
            if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
            break;
          }
      }
    }
    m[0] = n;
    target = target + String(m);
  }
  return target;
}
