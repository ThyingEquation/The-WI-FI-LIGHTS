**WI-FI Lumières De Noël**
=====================

**WI-FI Christmas lights**
=====================

**Вы сможете собрать свою собственную новогоднюю гирлянду. 
Андроид приложение предназначено для управления по сети WI-FI модулем ESP-12E/12F, который в свою очередь управляет лентой или готовой гибкой матрицей из адресных светодиодов. Приложение содержит 37 различных эффектов для гирлянды. Матрица 16х16, состоящая из ленты с шагом светодиодов 10 см, подойдет для большинства окон.**


<p align="center">
<img src="https://user-images.githubusercontent.com/125156746/226203992-f6c08d09-5ff4-4911-9943-f8e58fb74a75.jpg" width="500" />
</p>

`Прошивка микроконтроллера ESP`
-----------------------------------

Для начала необходимо прошить модуль ***ESP-12E/12F*** или отладочную плату на его основе ***NodeMCU***.
1.	По ссылке или из репозитория скачайте [***ESP8266Flasher***](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/ESP8266Flasher.exe) и скачайте файл прошивки ***.bin*** для матрицы [12х13, текст внутри](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/Прошивки/12x13%20text%20inside.bin), [12х13, текст снаружи](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/Прошивки/12x13%20text%20outside.bin), [16х16, текст внутри](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/Прошивки/16x16%20text%20inside.bin), [16х16, текст снаружи](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/Прошивки/16x16%20text%20outside.bin). Матрица 12х13 подойдет для стандартных двустворчатых окон, при расположении начала ленты в правом нижнем углу и с последующим движением ленты вверх, текст будет выводиться в зависимости от выбранной прошивки. Внутри: для чтения из помещения; снаружи: отраженный для чтения с улиы. 
2. Соберите схему, изображенную ниже, для прошивки модуля ***ESP-12E/12F*** через переходник ***UART -> USB***  или просто подключите через ***USB*** к ***ПК*** отладочную плату ***NodeMCU***.

<p align="center">
<img src="https://user-images.githubusercontent.com/125156746/226195939-0472280e-2450-4c82-9016-2b1e9b78b645.png" width="500" />
</p>

3. Запустите ***ESP8266Flasher*** и во вкладке ***config*** в первой строке выберите файл прошивки ***.bin***, вернитесь на вкладку ***operation*** и нажмите кнопку ***flash*** и дождитесь прошивки. 

<p align="center">
<img src="https://user-images.githubusercontent.com/125156746/226195994-9c8d60ad-c5fb-43d2-9177-56a1115e9586.png" width="500" />
</p>

Если все прошло удачно, программа уведомит Вас в левом нижнем углу о удачной прошивке.

<p align="center">
<img src="https://user-images.githubusercontent.com/125156746/226196050-bc388f15-ff63-42d2-99ae-3203220fb458.png" width="500" />
</p>

4. После прошивки необходимо перезагрузить модуль ***ESP-12E/12F*** или нажать на кнопку ***RST*** на отлажочной плате ***NodeMCU***.
 
`Сборка схемы`
-----------------------------------
    
   Соберите [схему](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/Гирлянада-Э3.jpg) на основе отладочной платы NodeMCU или [схему](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/Гирлянада_ESP12EF.pdf) на основе модуля ***ESP-12E/12F***. Для схемы на основе модуля ***ESP-12E/12F*** возможно изготовить [печатную плату](https://github.com/ThyingEquation/The-WI-FI-LIGHTS/blob/ver2/лут_плата.pdf). Плата специально изготавливалась под ЛУТ для одностороннего стеклотекстолита. ВАЖНО! Прошивать модуль можно и в собранной схеме
   
   Техничческие параметры схемы: 
   + напряжение питания 5 В от блока питания (220 В -> 5 В);
   + ток потребления схемы (матрица отключена, только отладочная плата NodeMCU) 0,250 А; 
   + ток потребления 256 светодиодов белого цвета при максимальной яркости + NodeMCU 4,286 А;
    
`Подключение и работа`
-----------------------------------
    
   Скачайте и установите из RuStore [приложение](https://apps.rustore.ru/app/BARANOV.K.D.LED_Girlianda). При первом запуске приложения, необходимо обязательно настроить имя сети WI-FI во вкладке "НАСТРОЙКИ" (изначально сеть будет называться по типу `ESP-67B17`). После смены имени сети на ваше, сеть с новым именем теперь будет запрашивать пароль, который равен `11111111` (8 единиц). Настройка завершена. 
