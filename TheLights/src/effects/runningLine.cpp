#include "effects.h"
#include "colors.h"
#include "lightsSettings.h"

#include <FastLED_NeoMatrix.h>

/*
  Эта группа эффектов для любого размера матриц

  В библиотеке Adafruit_GFX_library (-> TheLights -> .pio -> libdeps -> Adafruit_GFX_library) заменить файл glcdfont.c на одноименный
  из репозитория

  Настраиваемые параметры (runningLineSettings):
    1) Скорость бегущей строки
    2) Отступ текста от верхней части матрицы
    3) TEXT_POS позиция текста для режима "Бегущая строка". NEO_MATRIX_LEFT - Текст снаружи; NEO_MATRIX_RIGHT - Текст внутри

    P.S. Ваша гирлянда висит на окне, при этом первый светодиод находится в правом нижнем углу, в таком случае если выбрать "Текст внутри"
  то бегущая строка будет двигаться справа налево и будет виден внутри помещения. Соответсвенно "Текст снаружи" наоборот

*/

enum runningLineSettings
{
  RUNNING_LINE_DELAY = 250,
  TOP_MARGIN = 3,
  MATRIX_CONFIG = NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG
};

static String utf8rus(String source);

static const char *text[] = {"С наступающим Новым годом !!!", "С Новым годом !!!", "Happy New Year !!!", "С Рождеством !!!", "Merry Christmas !!!", "Здесь могла быть ваша реклама", "Привет, я умная гирлянда", "ВВЕДИТЕ ТЕКСТ"};
static const char *poem[] = {
    "На берегу пустынных волн Стоял он, дум великих полн, И вдаль глядел. Пред ним широко Река неслася; бедный чёлн По ней стремился одиноко. По мшистым, топким берегам Чернели избы здесь и там, Приют убогого чухонца; И лес, неведомый лучам В тумане спрятанного солнца, Кругом шумел.\n\n",

    "И думал он: Отсель грозить мы будем шведу, Здесь будет город заложен На зло надменному соседу. Природой здесь нам суждено В Европу прорубить окно, Ногою твердой стать при море. Сюда по новым им волнам Все флаги в гости будут к нам, И запируем на просторе.\n\n",

    "Прошло сто лет, и юный град, Полнощных стран краса и диво, Из тьмы лесов, из топи блат Вознесся пышно, горделиво; Где прежде финский рыболов, Печальный пасынок природы, Один у низких берегов Бросал в неведомые воды Свой ветхой невод, ныне там По оживленным берегам Громады стройные теснятся Дворцов и башен; корабли Толпой со всех концов земли К богатым пристаням стремятся;\n\n",

    "В гранит оделася Нева; Мосты повисли над водами; Темно-зелеными садами Ее покрылись острова, И перед младшею столицей Померкла старая Москва, Как перед новою царицей Порфироносная вдова.\n\n",

    "Люблю тебя, Петра творенье, Люблю твой строгий, стройный вид, Невы державное теченье, Береговой ее гранит, Твоих оград узор чугунный, Твоих задумчивых ночей Прозрачный сумрак, блеск безлунный, Когда я в комнате моей Пишу, читаю без лампады, И ясны спящие громады Пустынных улиц, и светла Адмиралтейская игла, И, не пуская тьму ночную На золотые небеса, Одна заря сменить другую Спешит, дав ночи полчаса.\n\n",

    "Люблю зимы твоей жестокой Недвижный воздух и мороз, Бег санок вдоль Невы широкой, Девичьи лица ярче роз, И блеск, и шум, и говор балов, А в час пирушки холостой Шипенье пенистых бокалов И пунша пламень голубой.\n\n",

    "Люблю воинственную живость Потешных Марсовых полей, Пехотных ратей и коней Однообразную красивость, В их стройно зыблемом строю Лоскутья сих знамен победных, Сиянье шапок этих медных, Насквозь простреленных в бою.\n\n",

    "Люблю, военная столица, Твоей твердыни дым и гром, Когда полнощная царица Дарует сына в царской дом, Или победу над врагом Россия снова торжествует, Или, взломав свой синий лед, Нева к морям его несет И, чуя вешни дни, ликует.\n\n",

    "Красуйся, град Петров, и стой Неколебимо как Россия, Да умирится же с тобой И побежденная стихия; Вражду и плен старинный свой Пусть волны финские забудут И тщетной злобою не будут Тревожить вечный сон Петра! Была ужасная пора, Об ней свежо воспоминанье… Об ней, друзья мои, для вас Начну свое повествованье. Печален будет мой рассказ . . . ."};

const int32_t textLength[] = {183, 100, 98, 98, 114, 173, 137, 85}; // множитель 6
const int32_t poemLength[] = {1668, 1530, 3366, 2418, 1260, 1278, 1356, 1356, 2052};

static FastLED_NeoMatrix matrix = FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_CONFIG);

void initRunningLine()
{
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setTextColor(pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(0, 128)])));
}

void drawRunningLine(uint8_t subMode)
{
  static int32_t x = MATRIX_WIDTH;
  static uint32_t lastTime = 0;
  static uint8_t lastSubmode = 255;

  static int32_t poemX = MATRIX_WIDTH;
  static uint8_t currentPoem = 0;
  static uint32_t poemLastTime = 0;
  static String cachedPoem = "";
  static uint8_t cachedPoemIndex = 255;

  if (subMode > 8 || subMode != lastSubmode)
  {
    x = MATRIX_WIDTH;
    lastSubmode = subMode;

    poemX = MATRIX_WIDTH;
    currentPoem = 0;

    matrix.setTextColor(pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(0, 128)])));

    if (subMode > 8)
      return;
  }

  if (subMode == 8)
  {
    if (millis() - poemLastTime < RUNNING_LINE_DELAY)
      return;

    poemLastTime = millis();

    fill_solid(leds, MATRIX_LEDS, CRGB::Black);
    matrix.fillScreen(0);

    matrix.setCursor(poemX, TOP_MARGIN);

    if (cachedPoemIndex != currentPoem)
    {
      cachedPoem = utf8rus(poem[currentPoem]);
      cachedPoemIndex = currentPoem;
    }
    matrix.print(cachedPoem);

    poemX--;

    if (poemX < -poemLength[currentPoem] - 30)
    {
      poemX = MATRIX_WIDTH;
      currentPoem = (currentPoem + 1) % (sizeof(poem) / sizeof(poem[0]));
    }

    stripShow();
    return;
  }

  if (millis() - lastTime < RUNNING_LINE_DELAY)
  {
    return;
  }

  lastTime = millis();

  matrix.fillScreen(0);

  matrix.setCursor(x, TOP_MARGIN);
  matrix.print(utf8rus(text[subMode]));

  x--;

  if (x < -textLength[subMode] - 10)
  {
    x = MATRIX_WIDTH;
    matrix.setTextColor(pgm_read_dword(&(mainColors[ESP8266TrueRandom.random(0, 128)])));
  }
  stripShow();
}

String utf8rus(String source)
{
  int i, k;
  String target;
  unsigned char n;
  char m[2] = {'0', '\0'};
  k = source.length();
  i = 0;
  while (i < k)
  {
    n = source[i];
    i++;
    if (n >= 0xC0)
    {
      switch (n)
      {
      case 0xD0:
      {
        n = source[i];
        i++;
        if (n == 0x81)
        {
          n = 0xA8;
          break;
        }
        if (n >= 0x90 && n <= 0xBF)
          n = n + 0x2F;
        break;
      }
      case 0xD1:
      {
        n = source[i];
        i++;
        if (n == 0x91)
        {
          n = 0xB7;
          break;
        }
        if (n >= 0x80 && n <= 0x8F)
          n = n + 0x6F;
        break;
      }
      }
    }
    m[0] = n;
    target = target + String(m);
  }
  return target;
}