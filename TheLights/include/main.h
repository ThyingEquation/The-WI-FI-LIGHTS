/*
==BARANOV KIRILL==
ver 3.1 - 2024
*/

#ifndef MAIN_H
#define MAIN_H
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <LittleFS.h>
#include <math.h>

#include "FS.h"
#include "anime.h"
#include "canvas.h"
#include "colorfulSpots.h"
#include "colorfulWaves.h"
#include "draw.h"
#include "extra.h"
#include "flashLights.h"
#include "rainbows.h"
#include "runningLights.h"
#include "runningLine.h"
#include "snow.h"
#include "space.h"
#include "water.h"

#define FASTLED_ALLOW_INTERRUPTS 0 FASTLED_USING_NAMESPACE
#define TEXT_POS \
  NEO_MATRIX_LEFT  // NEO_MATRIX_LEFT - outside === NEO_MATRIX_RIGHT - inside
#define mWidth 12
#define mHeight 13
#define NUM_LEDS mWidth* mHeight
#define BRIGHTNESS 50
#define PIN 4

extern Adafruit_NeoPixel strip;
extern Adafruit_NeoMatrix matrix;
extern CRGB* const leds;

extern uint8_t choosenModeD2;

bool loadConfig();
bool saveConfig();

void settingsProcessing(void);
void mainProcessing(void);
void paintingProcessing(void);
void argIndexCheck(String argu);
void checkMode(uint8_t num);
void allMods();

const int mainColors[] PROGMEM = {
    0x5d8aa8,  // Военно-воздушный синий    // Английский алфавит. Буква А
    0xe32636,  // Ализариновый красный
    0xffbf00,  // Янтарный
    0x9966cc,  // Аметистовый
    0xf2f3f4,  // Матовый белый
    0x8db600,  // Яблочно-зелёный
    0x00ffff,  // Цвет морской волны
    0x4b5320,  // Армейский зелёный
    0xfeb75e,  // Горький розовый
    0x0000ff,  // Синий
    0xcc0000,  // Красный Университета Бостона
    0xb5a642,  // Латунный
    0x66ff00,  // Ярко-зелёный
    0x08e8de,  // Ярко-бирюзовый
    0xcd00cd,  // Ярко-фиолетовый
    0x900020,  // Бургундский
    0xe97451,  // Сиена жжёная
    0x772953,  // Баклажановый
    0xc41e3a,  // Тёмно-красный, кардинал
    0xacd7af,  // Селадоновый
    0xde3163,  // Светлая вишня
    0x2a52be,  // Лазурно-синий
    0x7fff00,  // Салатовый цвет, шартрез
    0xfb935d,  // Кукурузный
    0xdc143c,  // Тёмно-красный, кровавый, Малиновый
    0x00ffff,  // Циан, васильковый
    0xbdb76b,  // Тёмный хаки, хаки
    0x03c03c,  // Тёмный пастельно-зелёный
    0xffdab9,  // Тёмно-персиковый
    0xe75480,  // Тёмно-розовый
    0x003399,  // Тёмно-Пороховая синь
    0xe9967a,  // Тёмная лососина
    0x560319,  // Тёмно-алый
    0x177245,  // Тёмный весенне-зелёный
    0xffa812,  // Тёмно-мандариновый
    0x423189,  // Глубокий фиолетовый цвет
    0xff1493,  // Глубокий розовый цвет
    0x50c878,  // Изумрудный // Буква E
    0x990066,  // Баклажановый
    0xb22222,  // Кирпичный
    0xff00ff,  // Фуксия
    0xffd700,  // Золотой
    0xdf73ff,  // Гелиотроп
    0x00ff00,  // Зелёный
    0x007dff,  // Синий Градуса
    0xadff2f,  // Жёлто-зелёный // Буква H
    0xfc0fc0,  // Ярко-розовый цвет
    0x4b0082,  // Индиго // Буква I
    0xff4f00,  // Международный (сигнальный) оранжевый
    0xcd5c5c,  // Индийский красный, каштановый
    0x00a86b,  // Нефритовый // Буква J // Буква K
    0x3a75c4,  // Синий Кляйна
    0xe6e6fa,  // Лаванда, лавандово-синий // Буква L
    0xccff00,  // Лайм
    0xfdff10,  // Лимонный
    0xc8a2c8,  // Сиреневый
    0xf0f8ec,  // Зелёная лужайка
    0xff00ff,  // Маджента, фуксия // Буква M
    0x0bda51,  // Малахитовая зелень (малахитовый)
    0x800000,  // Коричнево-малиновый
    0x98ff98,  // Зелёная мята (мятный)
    0x993366,  // Розовато-лиловый
    0x003366,  // Полуночно-синий
    0xadffad,  // Зелёный мох
    0x997a8d,  // Розовый Маунтбэттена
    0xffdb58,  // Горчичный // Буква N
    0x000080,  // Тёмно-синий
    0xcc7722,  // Охра // Буква O
    0x6b8e23,  // Нежно-оливковый
    0xffa500,  // Оранжевый
    0xda70d6,  // Орхидея
    0xfdf5e6,  // Старое кружево
    0xafeeee,  // Бледно-синий // Буква P
    0x987654,  // Бледно-коричневыйй
    0xaf4035,  // Бледно-карминный, ср. Кармин
    0xd5adb1,  // Бледно-каштановый
    0xf984e5,  // Бледно-пурпурный
    0xfadadd,  // Бледно-розовый
    0xdb7093,  // Лиловый
    0xdbbead,  // Бледно-песочный цвет
    0xf0dc82,  // Палевый
    0xc7fcec,  // Панг
    0xffefd5,  // Побег папайи
    0x77dd77,  // Пастельно-зелёный
    0xffd1dc,  // Пастельно-розовый
    0xd1e231,  // Грушевый
    0xcc14ff,  // Барвинок
    0x6600ff,  // Персидский синий
    0x01796f,  // Зелёная сосна
    0xffc0cb,  // Розовый
    0xff9966,  // Оранжево-розовый
    0x660066,  // Сливовый
    0xcc8899,  // Красновато-коричневый, пюсовый (устар.),
               // тёмно-коричневый, блошиный
    0xff7518,  // Тыква
    0x800080,  // Фиолетовый (оттенок) // Буква R
    0xff0000,  // Красный
    0xc71585,  // Фиолетово-красный
    0x4169e1,  // Ярко-синий («королевский синий»)
    0xff9900,  // Сигнальный оранжевый // Буква S
    0x082567,  // Сапфировый
    0xd286c8,  // Салутовый
    0xff3769,  // Лососёвый
    0x92000a,  // Сангрия
    0xff2400,  // Ярко-красный, алый
    0xffd800,  // Цвет жёлтого школьного автобуса
    0x2e8b57,  // Зелёное море
    0xfff5ee,  // Цвет морской ракушки
    0x00ff7f,  // Зелёная весна
    0x4682b4,  // Синяя сталь
    0xacb78e,  // Болотный
    0xd2b48c,  // Цвет загара // Буква T
    0xcd5700,  // Краснобуро-оранжевый
    0xffcc00,  // Мандариновый
    0xd0f0c0,  // Зелёный чай
    0x008080,  // Сине-зелёный
    0xd8bfd8,  // Чертополох
    0x30d5c8,  // Бирюзовый, ср. бирюза
    0xff6347,  // Томатный // Буква U
    0x5b92e5,  // Цвет флага ООН
    0xdd4814,  // Оранжевый Ubuntu
    0xf3e5ab,  // Ванильный // Буква V
    0x8b00ff,  // Фиолетовый
    0x40826d,  // Зелень Гинье
    0xffffff,  // Белый
    0xc9a0dd,  // Глициния
    0x722f37,  // Вино
    0xffff00,  // Жёлтый // Буква Y
    0xebc2af,  // Циннвальдит // Буква Z
    0x0014a8   // Кобальтовая синь
};

#endif
