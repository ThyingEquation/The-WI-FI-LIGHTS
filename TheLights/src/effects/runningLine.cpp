#include <FastLED_NeoMatrix.h>

#include "colors.h"
#include "effects.h"
#include "settings.h"

/*
  Эта группа эффектов для любого размера матриц

  В библиотеке Adafruit_GFX_library (-> TheLights -> .pio -> libdeps -> Adafruit_GFX_library) заменить файл glcdfont.c
  на одноименный из репозитория

  Настраиваемые параметры (RunningLineDelays):
    1) Скорость бегущей строки
    2) Отступ текста от верхней части матрицы
    3) TEXT_POS позиция текста для режима "Бегущая строка". NEO_MATRIX_LEFT - Текст снаружи; NEO_MATRIX_RIGHT - Текст
  внутри

    P.S. Ваша гирлянда висит на окне, при этом первый светодиод находится в правом нижнем углу, в таком случае если
  выбрать "Текст внутри" то бегущая строка будет двигаться справа налево и будет виден внутри помещения. Соответственно
  "Текст снаружи" наоборот

*/

namespace StripControl {}

namespace {
    enum class RunningLineDelays : uint16_t { RUNNING_LINE_DELAY = 250U, TOP_MARGIN = 3U };

    String utf8rus(String source);

    const char *text[] = {"С наступающим Новым годом !!!",
                          "С Новым годом !!!",
                          "Happy New Year !!!",
                          "С Рождеством !!!",
                          "Merry Christmas !!!",
                          "Здесь могла быть ваша реклама",
                          "Привет, я умная гирлянда",
                          "ВВЕДИТЕ ТЕКСТ"};
    const char *poem[] = {
            "На берегу пустынных волн Стоял он, дум великих полн, И вдаль глядел. Пред ним широко Река неслася; бедный "
            "чёлн По ней стремился одиноко. По мшистым, топким берегам Чернели избы здесь и там, Приют убогого "
            "чухонца; И "
            "лес, неведомый лучам В тумане спрятанного солнца, Кругом шумел.\n\n",

            "И думал он: Отсель грозить мы будем шведу, Здесь будет город заложен На зло надменному соседу. Природой "
            "здесь "
            "нам суждено В Европу прорубить окно, Ногою твердой стать при море. Сюда по новым им волнам Все флаги в "
            "гости "
            "будут к нам, И запируем на просторе.\n\n",

            "Прошло сто лет, и юный град, Полнощных стран краса и диво, Из тьмы лесов, из топи блат Вознесся пышно, "
            "горделиво; Где прежде финский рыболов, Печальный пасынок природы, Один у низких берегов Бросал в "
            "неведомые "
            "воды Свой ветхой невод, ныне там По оживленным берегам Громады стройные теснятся Дворцов и башен; корабли "
            "Толпой со всех концов земли К богатым пристаням стремятся;\n\n",

            "В гранит оделася Нева; Мосты повисли над водами; Темно-зелеными садами Ее покрылись острова, И перед "
            "младшею "
            "столицей Померкла старая Москва, Как перед новою царицей Порфироносная вдова.\n\n",

            "Люблю тебя, Петра творенье, Люблю твой строгий, стройный вид, Невы державное теченье, Береговой ее "
            "гранит, "
            "Твоих оград узор чугунный, Твоих задумчивых ночей Прозрачный сумрак, блеск безлунный, Когда я в комнате "
            "моей "
            "Пишу, читаю без лампады, И ясны спящие громады Пустынных улиц, и светла Адмиралтейская игла, И, не пуская "
            "тьму ночную На золотые небеса, Одна заря сменить другую Спешит, дав ночи полчаса.\n\n",

            "Люблю зимы твоей жестокой Недвижный воздух и мороз, Бег санок вдоль Невы широкой, Девичьи лица ярче роз, "
            "И "
            "блеск, и шум, и говор балов, А в час пирушки холостой Шипенье пенистых бокалов И пунша пламень "
            "голубой.\n\n",

            "Люблю воинственную живость Потешных Марсовых полей, Пехотных ратей и коней Однообразную красивость, В их "
            "стройно зыблемом строю Лоскутья сих знамен победных, Сиянье шапок этих медных, Насквозь простреленных в "
            "бою.\n\n",

            "Люблю, военная столица, Твоей твердыни дым и гром, Когда полнощная царица Дарует сына в царской дом, Или "
            "победу над врагом Россия снова торжествует, Или, взломав свой синий лед, Нева к морям его несет И, чуя "
            "вешни "
            "дни, ликует.\n\n",

            "Красуйся, град Петров, и стой Неколебимо как Россия, Да умирится же с тобой И побежденная стихия; Вражду "
            "и "
            "плен старинный свой Пусть волны финские забудут И тщетной злобою не будут Тревожить вечный сон Петра! "
            "Была "
            "ужасная пора, Об ней свежо воспоминанье… Об ней, друзья мои, для вас Начну свое повествованье. Печален "
            "будет "
            "мой рассказ . . . ."};

    const int32_t textLength[] = {183, 100, 98, 98, 114, 173, 137, 85}; // множитель 6
    const int32_t poemLength[] = {1668, 1530, 3366, 2418, 1260, 1278, 1356, 1356, 2052};

    auto matrix = FastLED_NeoMatrix(StripControl::leds, static_cast<uint16_t>(StripControl::MATRIX_WIDTH),
                                    static_cast<uint16_t>(StripControl::MATRIX_HEIGHT),
                                    NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
} // namespace

namespace Effects {
    void initRunningLine() {
        matrix.begin();
        matrix.setTextWrap(false);
        matrix.setTextColor(pgm_read_dword(&(Colors::mainColors[ESP8266TrueRandom.random(0, 128)])));
    }

    void drawRunningLine(const uint32_t subMode) {
        static auto x = static_cast<int32_t>(StripControl::MATRIX_WIDTH);
        static uint32_t lastTime = 0U;
        static uint32_t lastSubmode = 255U;

        static auto poemX = static_cast<int32_t>(StripControl::MATRIX_WIDTH);
        static uint8_t currentPoem = 0U;
        static uint32_t poemLastTime = 0U;
        static String cachedPoem = "";
        static uint32_t cachedPoemIndex = 255U;

        if (subMode > 8U || subMode != lastSubmode) {
            x = static_cast<int32_t>(StripControl::MATRIX_WIDTH);
            lastSubmode = static_cast<uint32_t>(subMode);

            poemX = static_cast<int32_t>(StripControl::MATRIX_WIDTH);
            currentPoem = 0U;

            matrix.setTextColor(pgm_read_dword(&(Colors::mainColors[ESP8266TrueRandom.random(0, 128)])));

            if (subMode > 8U) {
                return;
            }
        }

        if (subMode == 8U) {
            if (millis() - poemLastTime < static_cast<uint32_t>(RunningLineDelays::RUNNING_LINE_DELAY)) {
                return;
            }

            poemLastTime = millis();

            fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
            matrix.fillScreen(0U);

            matrix.setCursor(static_cast<int16_t>(poemX), static_cast<int16_t>(RunningLineDelays::TOP_MARGIN));

            if (cachedPoemIndex != currentPoem) {
                cachedPoem = utf8rus(poem[currentPoem]);
                cachedPoemIndex = currentPoem;
            }
            (void) matrix.print(cachedPoem);

            poemX--;

            if (poemX < -poemLength[currentPoem] - 30) {
                poemX = static_cast<int16_t>(StripControl::MATRIX_WIDTH);
                currentPoem = (currentPoem + 1U) % std::size(poem);
            }

            StripControl::show();
            return;
        }

        if (millis() - lastTime < static_cast<uint32_t>(RunningLineDelays::RUNNING_LINE_DELAY)) {
            return;
        }

        lastTime = millis();

        matrix.fillScreen(0U);

        matrix.setCursor(static_cast<int16_t>(x), static_cast<int16_t>(RunningLineDelays::TOP_MARGIN));
        (void) matrix.print(utf8rus(text[subMode]));

        x--;

        if (x < -textLength[subMode] - 10) {
            x = static_cast<int32_t>(StripControl::MATRIX_WIDTH);
            matrix.setTextColor(pgm_read_dword(&(Colors::mainColors[ESP8266TrueRandom.random(0, 128)])));
        }
        StripControl::show();
    }
} // namespace Effects

namespace {
    String utf8rus(String source) {
        String target;
        char m[2] = {'0', '\0'};
        const auto k = static_cast<int32_t>(source.length());
        int32_t i = 0;
        while (i < k) {
            auto n = static_cast<unsigned char>(source[static_cast<uint32_t>(i)]);
            i++;
            if (n >= 0xC0U) {
                switch (static_cast<uint32_t>(n)) {
                    case 0xD0U: {
                        n = static_cast<unsigned char>(source[static_cast<uint32_t>(i)]);
                        i++;
                        if (n == 0x81U) {
                            n = 0xA8U;
                            break;
                        }
                        if (n >= 0x90U && n <= 0xBFU) {
                            n = n + 0x2FU;
                        }
                        break;
                    }
                    case 0xD1U: {
                        n = static_cast<unsigned char>(source[static_cast<uint32_t>(i)]);
                        i++;
                        if (n == 0x91U) {
                            n = 0xB7U;
                            break;
                        }
                        if (n >= 0x80U && n <= 0x8FU) {
                            n = n + 0x6FU;
                        }
                        break;
                    }
                    default:;
                }
            }
            m[0] = n;
            target = target + String(m);
        }
        return target;
    }
} // namespace
