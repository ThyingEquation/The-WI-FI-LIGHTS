**Новогодняя гирлянда**
=====================

**The christmas lights**
=====================

<p align="center">
  <img src="images/hello.gif" alt="animated" />
</p>

`Описание проекта`
-----------------------------------

**Это моя попытка создать новогоднюю оконную гирлянду на основе светодиодов WS2812B, управляемых с помощью приложения для операционной системы Android (телефоны, планшеты и декстопы). Среда разработки IDE — VS Code + расширение PlatformIO (среда Ардуино для микроконтроллеров ESP).**

**Некоторые эффекты для гирлянды были придуманы и разработаны мной полностью, а также некоторые разработаны мной, но вдохновлены эффектами из других проектов. Также некоторые эффекты взяты мной полностью без изменений или с модификацией/доработкой. Источники:**

* Проекты alexgyver.ru;
* Проекты wokwi.com;
* Примеры из библиотек Adafruit NeoPixel, Adafruit NeoMatrix и FastLED;
* Проекты других авторов, чьи данные, к сожалению, не сохранились.

**Вы можете повторить мой проект и собрать свою собственную новогоднюю гирлянду. 
Андроид-приложение ([скачать из RuStore](https://www.rustore.ru/catalog/app/BARANOV.K.D.LED_Girlianda)) предназначено для управления по сети Wi-Fi микроконтроллером ESP8266, который в свою очередь управляет новогодней гирляндой (матрицей из светодиодов). Доступно 56 различных эффектов, включая красочные радужные переливы, бегущую строку, анимацию и многое другое. ВНИМАНИЕ: некоторые эффекты запускаются и отключаются с задержкой от 1 до 5 секунд, во избежание зависания не нужно много раз нажимать на включение или отключение режима, но если прошло более 10 секунд, то это неисправность.**

**Почти для всех эффектов достаточно изменить размер матрицы в main.h для корректной работы, так как изначально проект отлаживался на матрице 16х16. Анимации созданы сугубо под размер 12х13.**

**Гирлянда в виде матрицы из 12х13 светодиодов идеально подойдёт для большинства двустворчатых окон с размером оконного проема 150х150 см (обычно как раз 120 мм в ширину и 130 мм в высоту занимает стеклопакет). Если у вас окно другого размера или вы хотите повесить гирлянду на балкон, вы можете изменить размеры матрицы в исходном коде в соответствии с вашими потребностями.**

<p align="center">
  <img src="images/spots.gif" alt="animated" />
</p>

`Прошивка микроконтроллера ESP`
-----------------------------------

Для начала необходимо прошить модуль ***ESP-12E/12F*** или отладочную плату на его основе ***NodeMCU***.

Прошить можно прямиком из IDE или через специальную программу:

1.	Из репозитория скачайте [***ESP8266Flasher***](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/main/ESP8266Flasher.exe) и скачайте прошивку ***.bin*** для матрицы [12х13, текст снаружи](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/main/Firmware/firmware.bin). Матрица 12х13 подойдет для стандартных двустворчатых окон, при расположении начала ленты в правом нижнем углу окна и с последующим движением ленты вверх, потом вниз и т.д. (зигзаг), текст будет выводиться так, что будет читаемым со стороны улицы. 
2. Соберите схему, изображенную ниже, для прошивки модуля ***ESP-12E/12F*** через переходник ***UART -> USB***  или просто подключите через ***USB*** к ***ПК*** отладочную плату ***NodeMCU***.

<p align="center">
<img src="images/program1.png" width="500" />
</p>

3. Запустите ***ESP8266Flasher*** и во вкладке ***config*** в первой строке выберите файл прошивки ***.bin***, вернитесь на вкладку ***operation*** и нажмите кнопку ***flash*** и дождитесь прошивки. 

<p align="center">
<img src="images/program2.png" width="500" />
</p>

Если все прошло удачно, программа уведомит Вас в левом нижнем углу о удачной прошивке.

<p align="center">
<img src="images/program3.png" width="500" />
</p>

4. После прошивки необходимо перезагрузить модуль ***ESP-12E/12F*** или нажать на кнопку ***RST*** на отлажочной плате ***NodeMCU***.
 
`Сборка схемы`
-----------------------------------
    
   Соберите [схему](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/main/Гирлянада-Э3.jpg) на основе отладочной платы NodeMCU или [схему](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/main/Гирлянада_ESP12EF.pdf) на основе модуля ***ESP-12E/12F***. Для схемы на основе модуля ***ESP-12E/12F*** возможно изготовить [печатную плату](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/main/LUT_board.pdf). Плата специально изготавливалась под ЛУТ для одностороннего стеклотекстолита. ВАЖНО! Прошивать модуль можно и в собранной схеме. В качестве светодиодов только WS2812B, можно купить в виде ленты с шагом 10 см для простоты монтажа на окно.
   
 Технические параметры схемы: 
   + напряжение питания 5 В от блока питания (220 В -> 5 В); (подойдет вообще любой блок питания, главное — не превысить допустимое потребление тока. Лучше всего взять блок с запасом 20% по току)
   + ток потребления схемы (матрица отключена, только отладочная плата NodeMCU) ≈ 0,250 А;
   + ток потребления 156 (12х13) светодиодов белого цвета (самый прожорливый цвет) при обычной яркости (50 из 255 возможных, в прошивке в качестве стандартной) + схема в сборе ≈ 1,110 А;
   + ток потребления 156 (12х13) светодиодов белого цвета при максимальной яркости (255) + схема в сборе ≈ 2,5 А;
     
```diff
-ВНИМАНИЕ!!! Увеличение яркости более 50 значительно увеличивает потребление тока и нагрев светодиодов!!!
```

`Подключение и работа`
-----------------------------------

После прошивки микроконтроллера при подаче питания будет создана сеть Wi-Fi с автоматически сгенерированным названием сети по типу `ESP-67B17` и паролем `11111111`. 
   Скачайте и установите из RuStore [приложение](https://apps.rustore.ru/app/BARANOV.K.D.LED_Girlianda). Подключитесь к создаваемой гирляндой сети и управляйте различными режимами. Для удобства дальнейшего использования можно настроить имя сети WI-FI во вкладке «НАСТРОЙКИ». После смены имени сети на ваше сеть с новым именем теперь будет запрашивать пароль, тот же пароль, что и был, который равен `11111111` (8 единиц). Настройка завершена. Из приложения пароль изменить невозможно, только через исходный код. Возможна также смена стартового режима, то есть при подаче питания на гирлянду будет запускаться выбранный режим. 

   
   Возможны зависания (по сети Wi-Fi подключается, но на команды не реагирует), в случае зависания просто перезагрузить. Если гирлянда работала, но потом перестала работать, проверьте, есть ли сеть Wi-Fi, создаваемая гирляндой, если сеть есть и возможно подключиться, то произошла утечка памяти и контроллер сам перезагрузился (при условии, если не выбран стартовый режим, если режим выбран, то после перезагрузки запустится стартовый режим и вы не заметите произошедшей утечки). Утечка памяти мной отлавливается и предположительно возникает из-за работы двух библиотек ESP8266WiFi и Adafruit_NeoMatrix, для версии гирлянды 3.1 данная проблема не устранена, но возникает намного реже, чем было в ранних версиях. Она выявлена на некоторых анимациях и режиме «Метеорный поток». Остальные возникшие проблемы могут быть связаны с некорректной сборкой самой схемы. 
Не оставляйте гирлянду без присмотра, отключайте на ночь.
