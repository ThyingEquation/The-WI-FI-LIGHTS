# The-WI-FI-LIGHTS [Гирлянда (WI-FI матрица)]

<img src="https://user-images.githubusercontent.com/125156746/226203992-f6c08d09-5ff4-4911-9943-f8e58fb74a75.jpg" width="800" />

Вы сможете собрать свою собственную новогоднюю гирлянду. 
Приложение предназначено для управления по сети WI-FI микроконтроллером ESP8266, который в свою очередь управляет лентой или готовым модулем (гибкой матрицей) из адресных светодиодов.  Приложение содержит 37 различных эффектов для гирлянды. Матрица 16х16 светодиодов подойдет для большинства окон. 


                                                                      == ПРОШИВКА МИКРОКОНТРОЛЛЕРА ==

     
Для начала необходимо прошить микроконтроллер ESP8266 или отладочную плату на его основе NodeMCU (инструкция для windows). 
1.	По ссылке или из репозитория скачайте ESP8266Flasher (https://disk.yandex.ru/d/dSQwCgCfOg9xrA) и скачайте файл прошивки matrix16x16.bin
2. Соберите схему для прошивки модуля ESP8266 через переходник UART -> USB  или просто подключите через USB к ПК плату ESP-12E/12F.

<img src="https://user-images.githubusercontent.com/125156746/226195939-0472280e-2450-4c82-9016-2b1e9b78b645.png" width="800" />
(источник https://www.drive2.ru/l/508484546501542169)

3. Запустите ESP8266Flasher и во вкладке config в первой строке выберите файл прошивки matrix16x16.bin, вернитесь на вкладку operation и нажмите кнопку flash и дождитесь прошивки 

<img src="https://user-images.githubusercontent.com/125156746/226195994-9c8d60ad-c5fb-43d2-9177-56a1115e9586.png" width="800" />

Если все прошло удачно, программа уведомит Вас в левом нижнем углу о удачной прошивке 

<img src="https://user-images.githubusercontent.com/125156746/226196050-bc388f15-ff63-42d2-99ae-3203220fb458.png" width="800" />

4. После прошивки необходимо отключить/перезагрузить микроконтроллер 
    
                                                                             == СБОРКА СХЕМЫ ==
    
    Соберите схему представленную в Гирлянада-Э3.jpg, на данный момент печатная плата еще не готова). ВАЖНО! Прошивать микроконтроллер можно и в собранной схеме
    Техничческие параметры схемы: 
    - напряжение питания 5В от блока питания (220 В -> 5 В);
    - ток потребления схемы (матрица отключена) 0,250 А; 
    - ток потребления 256 светодиодов белого цвета при максимальной яркости 4,286 А;
    
    =Подключение и работа=
    
      Скачайте и установите из RuStore (https://apps.rustore.ru/app/BARANOV.K.D.LED_Girlianda) или репохитория app-release.apk, включите точку доступа WI-FI и в настройках выберите имя сети "Lights", пароль "11111111" и отключите общий доступ к WI-FI. ВАЖНО! не меняйте имя и проль сети, так как микроконтроллер не сможет подключиться к другой сети.
      
      ![Screenshot_20230319-211702_Settings](https://user-images.githubusercontent.com/125156746/226198616-f73ad739-1dc2-4ac3-9591-a82e93ecd8d6.jpg)

      
      Активируйте точку доступа и подайте питание на схему и дождитесь подключения микроконтроллера. После подключения микроконтроллер будет указан в подключенных устройствах
    
    ![Screenshot_20230319-212006_Settings](https://user-images.githubusercontent.com/125156746/226198621-fb0548bf-c296-4e4e-95f7-4dee6688d87c.jpg)

![Screenshot_20230319-212018_Settings](https://user-images.githubusercontent.com/125156746/226198626-efe953dc-0f3c-4670-9a23-ccfdd77cbc66.jpg)

Запишите IP адрес подключенного микроконтроллера, в данном случае это 192.168.43.233, далее запустите приложение "Гирлянда" 
     
      После ввода IP адреса, нажмите на желтую дискету и адрес сохраниться, больше его вводить не нужно.

После успешного сохранения, нажмите на кнопку "Подключение", после удачного обмена командами с микроконтроллером, появится кнопка "Далее", поздравляю, схема собрана верно, все работает!!!

![Screenshot_20230320-212554](https://user-images.githubusercontent.com/125156746/226709222-e9dbd47f-c5a8-41ae-aba5-396d0da255db.jpg)


    =ЗАКЛЮЧЕНИЕ=
    
    Хочу выразить благодарность Neco Ru за уроки по работе ESP и андроид. 
    Эффекты для ленты написаны самим/взяты из примеров библиотек, а так же взяты с сайта https://wokwi.com/, так же выражаю им благодарность.
    
    Буду рад предложениям по улучшению проекта, информации о найденых ошибках и при наличии матриц 8х8, 12х12 и тп. обкатку моего кода у добровольцев. Для связи пишите на почту TyingEquation@yandex.by
    
    === Найденные ошибки на данный момент ===
    
    - в режиме холст, при увеличении яркости (для цветов "белый", "морозное небо" и "светло-розовый") на любое значение зависает полностью микроконтроллер 
    - ошибки отображения интерфейса на планшетах

