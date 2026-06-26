#include <ESP8266WebServer.h>
#include <string>

#include "commandsHandler.h"
#include "localWifiServer.h"

#include "effects.h"
#include "settings.h"

namespace LocalWifiServer {
    void initAccessPointServer() {
        (void) WiFi.mode(WIFI_AP);
        delay(1U);
        const IPAddress localIp(Settings::parameters.localIpVal[0], Settings::parameters.localIpVal[1],
                                Settings::parameters.localIpVal[2], Settings::parameters.localIpVal[3]);
        const IPAddress gateway(Settings::parameters.gatewayVal[0], Settings::parameters.gatewayVal[1],
                                Settings::parameters.gatewayVal[2], Settings::parameters.gatewayVal[3]);
        const IPAddress subnet(Settings::parameters.subnetVal[0], Settings::parameters.subnetVal[1],
                               Settings::parameters.subnetVal[2], Settings::parameters.subnetVal[3]);
        delay(1U);
        (void) WiFi.softAPConfig(localIp, gateway, subnet);
        (void) WiFi.softAP(&Settings::parameters.localNetworkSsid[0], &Settings::parameters.localNetworkPassword[0]);
        delay(1U);
        (void) Udp.begin(1653U);
    }

    void initStationServer() {
        (void) WiFi.mode(WIFI_STA);
        (void) WiFi.begin(&Settings::parameters.stationNetworkSsid[0], &Settings::parameters.stationNetworkPassword[0]);

        uint8_t attempts = 0U;
        while (WiFi.status() != WL_CONNECTED && attempts < 20U) {
            delay(500U);
            for (uint32_t i = 0U; i < attempts; ++i) {
                StripControl::leds[i] = CHSV(96U, 255U, 80U);
            }
            StripControl::show();
            attempts++;
        }

        delay(10U);
        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
        StripControl::show();
        delay(10U);

        if (WiFi.status() != WL_CONNECTED) {
            fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CHSV(0U, 255U, 80U));
            StripControl::show();
            delay(2000U);
            EspClass::restart();
        }

        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CHSV(96U, 255U, 80U));
        StripControl::show();
        delay(2000U);
        fill_solid(&StripControl::leds[0], static_cast<int>(StripControl::MATRIX_LEDS), CRGB::Black);
        StripControl::show();

        (void) Udp.begin(1653U);
    }

    void checkServer() {
        if (Udp.parsePacket()) {
            char incomingPacket[256U] = {};
            if (const auto len = static_cast<int32_t>(Udp.read(&incomingPacket[0], 254U)); len > 0) {
                incomingPacket[len] = '\0';
            }

            const auto command = std::string_view(incomingPacket);

            if (command == "DISCOVER") {
                const String reply =
                        String("IAM_HERE|") + Settings::parameters.localNetworkSsid + "|" +
                        (WiFi.getMode() == WIFI_STA ? WiFi.localIP().toString() : WiFi.softAPIP().toString());
                (void) Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
                (void) Udp.write(reply.c_str());
                (void) Udp.endPacket();
                return;
            }

            if (constexpr std::array<std::string_view, 4U> commands = {"connectionState?", "settings?", "canvas?",
                                                                       "command?"};
                command.find(commands[0U]) == 0U) {
                CommandsHandler::handleConnectionState(command.substr(commands[0U].length()));
            } else if (command.find(commands[1U]) == 0U) {
                CommandsHandler::handleSettings(command.substr(commands[1U].length()));
            } else if (command.find(commands[2U]) == 0U) {
                CommandsHandler::handleCanvas(command.substr(commands[2U].length()));
            } else if (command.find(commands[3U]) == 0U) {
                CommandsHandler::handleMainCommand(command.substr(commands[3U].length()));
            } else {
                (void) Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
                (void) Udp.write("Error main");
                (void) Udp.endPacket();
            }
        }
    }
} // namespace LocalWifiServer

namespace SettingsServer {
    ESP8266WebServer server(80);

    void handleRoot() {
        String html = "<html><head><meta charset='UTF-8'><title>Настройки Гирлянды</title>";
        html += "<style>body { font-family: sans-serif; margin: 20px; } input { margin-bottom: 10px; width: 100%; "
                "padding: 8px; } input[type='radio'] { width: auto; vertical-align: middle; margin-bottom: 0; padding: "
                "0; }</style>";
        html += "</head><body>";
        html += "<h2>Настройки Гирлянды</h2>";

        html += "<form action='/save' method='POST'>";

        html += "Имя гирлянды (текущее: <b>" + static_cast<String>(Settings::parameters.localNetworkSsid) +
                "</b>):<br>";
        html += "<input type='text' name='g_name' placeholder='Введите новое имя'><br><br>";

        html += "Пароль гирлянды (текущее: <b>" + static_cast<String>(Settings::parameters.localNetworkPassword) +
                "</b>):<br>";
        html += "<input type='text' name='g_pass' placeholder='Введите новый пароль (мин. 8 символов)'><br><br>";

        html += "Имя вашей Wi-Fi сети (текущее: <b>" + static_cast<String>(Settings::parameters.stationNetworkSsid) +
                "</b>):<br>";
        html += "<input type='text' name='wifi_ssid' placeholder='Введите новое имя'><br><br>";

        html += "Пароль вашей Wi-Fi сети (текущий: <b>" +
                static_cast<String>(Settings::parameters.stationNetworkPassword) + "</b>):<br>";
        html += "<input type='text' name='wifi_pass' placeholder='Введите новый пароль (мин. 8 символов)'><br><br>";

        html += "Режим работы:<br>";
        html += "<label><input type='radio' name='workMode' value='1'" +
                String(Settings::parameters.workMode == 1U ? " checked" : "") +
                "> Работа через андроид приложение + точка доступа (AP)</label><br>";
        html += "<label><input type='radio' name='workMode' value='2'" +
                String(Settings::parameters.workMode == 2U ? " checked" : "") +
                "> Работа через андроид приложение + сторонняя сеть (STA)</label><br>";
        html += "<label><input type='radio' name='workMode' value='3'" +
                String(Settings::parameters.workMode == 3U ? " checked" : "") +
                "> Работа через упрощенную веб-страницу + точка доступа</label><br><br>";

        html += "<input type='submit' value='Сохранить и перезагрузить' style='background: #4CAF50; color: white; "
                "border: none; cursor: pointer;'>";
        html += "</form></body></html>";

        server.send(200, "text/html", html);
    }

    void handleSettings() {
        if (server.hasArg("g_name") && server.arg("g_name") != "") {
            const String &gName = server.arg("g_name");
            (void) strncpy(&Settings::parameters.localNetworkSsid[0], gName.c_str(), 31U);
            Settings::parameters.localNetworkSsid[31] = '\0';
        }
        if (server.hasArg("g_pass") && server.arg("g_pass") != "") {
            const String &gPass = server.arg("g_pass");
            if (gPass.length() >= 8U) {
                (void) strncpy(&Settings::parameters.localNetworkPassword[0], gPass.c_str(), 63U);
                Settings::parameters.localNetworkPassword[63] = '\0';
            }
        }
        if (server.hasArg("wifi_ssid") && server.arg("wifi_ssid") != "") {
            const String &wSsid = server.arg("wifi_ssid");
            (void) strncpy(&Settings::parameters.stationNetworkSsid[0], wSsid.c_str(), 31U);
            Settings::parameters.stationNetworkSsid[31] = '\0';
        }
        if (server.hasArg("wifi_pass") && server.arg("wifi_pass") != "") {
            const String &wPass = server.arg("wifi_pass");
            if (wPass.length() >= 8U) {
                (void) strncpy(&Settings::parameters.stationNetworkPassword[0], wPass.c_str(), 63U);
                Settings::parameters.stationNetworkPassword[63] = '\0';
            }
        }

        if (server.hasArg("workMode")) {
            Settings::parameters.workMode = static_cast<uint32_t>(server.arg("workMode").toInt());
        }

        const String msg = "<html><head><meta charset='UTF-8'></head><body><h3>Настройки сохранены!</h3><p>ESP8266 "
                           "перезагружается...</p></body></html>";
        server.send(200, "text/html", msg);

        Settings::save();
        delay(2000U);
        EspClass::restart();
    }

    void checkServer() {
        (void) WiFi.mode(WIFI_AP);
        delay(1U);
        const IPAddress localIp(Settings::parameters.localIpVal[0], Settings::parameters.localIpVal[1],
                                Settings::parameters.localIpVal[2], Settings::parameters.localIpVal[3]);
        const IPAddress gateway(Settings::parameters.gatewayVal[0], Settings::parameters.gatewayVal[1],
                                Settings::parameters.gatewayVal[2], Settings::parameters.gatewayVal[3]);
        const IPAddress subnet(Settings::parameters.subnetVal[0], Settings::parameters.subnetVal[1],
                               Settings::parameters.subnetVal[2], Settings::parameters.subnetVal[3]);
        delay(1U);
        (void) WiFi.softAPConfig(localIp, gateway, subnet);
        (void) WiFi.softAP(String("Settings.") + Settings::parameters.localNetworkSsid, "11111111");

        server.on("/", &handleRoot);
        server.on("/save", &handleSettings);

        server.begin();

        while (true) {
            server.handleClient();
            delay(1U);
        }
    }
} // namespace SettingsServer


namespace WebControlServer {

    ESP8266WebServer server(80);

    void prepareEffectChange() {
        CommandsHandler::sendVirtualCommand();
        Effects::state.isScreenClearEnable = true;
        Effects::state.isAllModesEnable = false;
    }

    void handleRoot() {
        String html = "";
        (void) html.reserve(6144U);

        html += "<html><head><meta charset='UTF-8'><title>Управление Гирляндой</title>";
        html += "<style>body { font-family: sans-serif; margin: 20px; background: #ffffff; color: #333; }";
        html += "input, select { margin-bottom: 20px; width: 100%; padding: 15px; box-sizing: border-box; }";
        html += ".block { background: #f9f9f9; padding: 15px; border-radius: 8px; margin-bottom: 20px; border: 1px "
                "solid #d3d3d3; }";
        html += ".btn { background: #4CAF50; color: white; border: none; cursor: pointer; padding: 15px; "
                "border-radius: 4px; font-weight: bold; width: 100%; }";
        html += ".btn-blue { background: #2196F3; }";
        html += ".btn-orange { background: #FF9800; }";
        html += "</style></head><meta name='viewport' content='width=device-width, initial-scale=1.0'><body>";

        html += "<h2>Упрощенное управление Гирляндой</h2>";

        // === ЭФФЕКТЫ ===
        html += "<div class='block'><h3>Выбор эффекта</h3>";
        html += "<form action='/set_effect' method='POST'>";
        html += "<select name='effect'>";

        html += "<optgroup>";
        html += "<option value='11_0'>Все эффекты</option>";
        html += "</optgroup>";

        html += "<optgroup label='Цветные эффекты'>";
        html += "<option value='0_0'>Цветные пятна</option>";
        html += "<option value='0_1'>Световой шум</option>";
        html += "<option value='0_2'>Диагональные волны</option>";
        html += "<option value='0_3'>Крутящаяся радуга</option>";
        html += "<option value='0_4'>Радужная рябь</option>";
        html += "<option value='0_5'>Радуга волной</option>";
        html += "<option value='0_6'>Радуга змейкой</option>";
        html += "</optgroup>";

        html += "<optgroup label='Бегущие огни'>";
        html += "<option value='1_0'>Медленный огонек</option>";
        html += "<option value='1_1'>Быстрый огонек</option>";
        html += "<option value='1_2'>Цветной огонек</option>";
        html += "<option value='1_3'>Цветная змейка</option>";
        html += "<option value='1_4'>Бегущие огоньки #1</option>";
        html += "<option value='1_5'>Бегущие огоньки #2</option>";
        html += "<option value='1_6'>Бегущие огоньки #3</option>";
        html += "</optgroup>";

        html += "<optgroup label='Прыгающие огоньки'>";
        html += "<option value='2_0'>Хаос</option>";
        html += "<option value='2_1'>Дрейфующая линия</option>";
        html += "<option value='2_2'>Прыгающие круги</option>";
        html += "<option value='2_3'>Прыгающий квадрат</option>";
        html += "<option value='2_4'>Прыгающие точки</option>";
        html += "</optgroup>";

        html += "<optgroup label='Мерцающие огни'>";
        html += "<option value='3_0'>Летящие огни</option>";
        html += "<option value='3_1'>Конфетти</option>";
        html += "<option value='3_2'>Мерцающие огни</option>";
        html += "</optgroup>";

        html += "<optgroup label='Водные эффекты'>";
        html += "<option value='4_0'>Лагуна</option>";
        html += "<option value='4_1'>Бассейн</option>";
        html += "</optgroup>";

        html += "<optgroup label='Погодные эффекты'>";
        html += "<option value='5_0'>Снегопад</option>";
        html += "<option value='5_1'>Метель</option>";
        html += "<option value='5_2'>Дождь</option>";
        html += "<option value='5_3'>Ливень</option>";
        html += "</optgroup>";

        html += "<optgroup label='Эффекты в виде игр'>";
        html += "<option value='6_0'>Игра змейка</option>";
        html += "<option value='6_1'>Тетрис</option>";
        html += "<option value='6_2'>Арканоид</option>";
        html += "<option value='6_3'>Космические корабли</option>";
        html += "<option value='6_4'>Эффект из к/ф матрица</option>";
        html += "</optgroup>";

        html += "<optgroup label='Космос'>";
        html += "<option value='7_0'>Звездное небо</option>";
        html += "<option value='7_1'>Созвездия</option>";
        html += "<option value='7_2'>Пульсирующая звезда</option>";
        html += "<option value='7_3'>Затменные звезды</option>";
        html += "<option value='7_4'>Метеоритный дождь</option>";
        html += "<option value='7_5'>Спиральная туманность</option>";
        html += "<option value='7_6'>Спиральная галактика</option>";
        html += "<option value='7_7'>Фазы луны</option>";
        html += "<option value='7_8'>Юпитер</option>";
        html += "<option value='7_9'>Черная дыра</option>";
        html += "<option value='7_10'>Северное сияние</option>";
        html += "<option value='7_11'>Магнитные волны</option>";
        html += "<option value='7_12'>Интерференция лучей</option>";
        html += "</optgroup>";

        html += "<optgroup label='Режим рисования'>";
        html += "<option value='8_0'>Полная заливка</option>";
        html += "<option value='8_1'>Зеркальная заливка</option>";
        html += "<option value='8_2'>Заливка линиями</option>";
        html += "<option value='8_3'>Попиксельная линиями</option>";
        html += "<option value='8_4'>Быстрая заливка змейкой</option>";
        html += "<option value='8_5'>Медленная заливка змейкой</option>";
        html += "<option value='8_6'>Цветная заливка змейкой</option>";
        html += "<option value='8_7'>Цветное дыхание</option>";
        html += "<option value='8_254'>Все картинки подряд</option>";
        html += "<option value='8_255'>Все картинки беспорядочно</option>";
        html += "</optgroup>";

        html += "<optgroup label='Бегущая строка (Текст)'>";
        html += "<option value='9_0'>С наступающим Новым годом</option>";
        html += "<option value='9_1'>С Новым годом</option>";
        html += "<option value='9_2'>Happy New Year</option>";
        html += "<option value='9_3'>С Рождеством</option>";
        html += "<option value='9_4'>Merry Christmas</option>";
        html += "<option value='9_5'>Здесь могла быть ваша реклама</option>";
        html += "<option value='9_6'>Привет, я умная гирлянда</option>";
        html += "<option value='9_7'>ВВЕДИТЕ ТЕКСТ</option>";
        html += "<option value='9_8'>Поэма 'Медный всадник'</option>";
        html += "</optgroup>";

        html += "<optgroup label='Анимации'>";
        html += "<option value='10_0'>Сердце</option>";
        html += "<option value='10_1'>Смайлик</option>";
        html += "<option value='10_2'>Прыгающий человечек</option>";
        html += "<option value='10_3'>Файербол</option>";
        html += "<option value='10_4'>Взрыв</option>";
        html += "<option value='10_5'>'С НОВЫМ ГОДОМ' на японском</option>";
        html += "<option value='10_6'>Приветствие на корейском</option>";
        html += "<option value='10_7'>Цифровой сигнал</option>";
        html += "<option value='10_8'>Синусоида</option>";
        html += "<option value='10_9'>Цветные синусоиды</option>";
        html += "<option value='10_10'>Цветные линии #1</option>";
        html += "<option value='10_11'>Цветные линии #2</option>";
        html += "<option value='10_12'>Цветные линии #3</option>";
        html += "<option value='10_13'>Цветные линии #4</option>";
        html += "</optgroup>";

        html += "</select>";
        html += "<input type='submit' class='btn btn-blue' value='Включить'></form>";

        html += "<form action='/reset_effect' method='POST'>";
        html += "<input type='submit' class='btn btn-blue' value='Отключить эффект'></form></div>";

        // === НАСТРОЙКИ ===
        html += "<div class='block'><h3>Настройки</h3>";

        html += "<form action='/set_startup' method='POST'>";
        html += "<input type='submit' class='btn btn-orange' value='Сделать текущий эффект стартовым'></form>";

        html += "<form action='/remove_startup' method='POST'>";
        html += "<input type='submit' class='btn btn-orange' value='Удалить стартовый эффект'></form>";

        html += "<form action='/save' method='POST'>";

        html += "Яркость (0-255):<br>";
        html += "<input type='number' name='bright' min='0' max='255' value='" +
                String(Settings::parameters.globalBrightness) + "'><br>";

        html += "Задержка 'Все эффекты' (мс):<br>";
        html += "<input type='number' name='delay' min='1000' value='" + String(Settings::parameters.allModeDelay) +
                "'><br>";

        html += "Порядок смены эффектов:<br>";
        html += "<select name='order'>";
        html += "<option value='1'" + String(Settings::parameters.allModesWorkType == 1U ? " selected" : "") +
                ">По порядку</option>";
        html += "<option value='0'" + String(Settings::parameters.allModesWorkType == 0U ? " selected" : "") +
                ">Случайный порядок</option>";
        html += "</select><br>";

        html += "Авто-отключение Wi-Fi (через 3 мин):<br>";
        html += "<select name='wifi_off'>";
        html += "<option value='0'" + String(!Settings::parameters.isWifiAutoOffEnable ? " selected" : "") +
                ">Выключено (Wi-Fi всегда работает)</option>";
        html += "<option value='1'" + String(Settings::parameters.isWifiAutoOffEnable ? " selected" : "") +
                ">Включено</option>";
        html += "</select><br><br>";

        html += "<input type='submit' class='btn' value='Сохранить и перезагрузить'></form></div>";

        html += "</body></html>";
        server.send(200, "text/html", html);
    }

    void handleEnableEffect() {
        prepareEffectChange();

        if (server.hasArg("effect")) {
            const String effVal = server.arg("effect");
            if (const int32_t separatorIdx = effVal.indexOf('_'); separatorIdx != -1) {
                const auto group =
                        static_cast<uint32_t>(effVal.substring(0U, static_cast<uint32_t>(separatorIdx)).toInt());
                const auto submode =
                        static_cast<uint32_t>(effVal.substring(static_cast<uint32_t>(separatorIdx + 1)).toInt());
                Effects::state.effectsGroup = group;
                Effects::state.effectSubmode = submode;
            }
        }
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
    }

    void handleDisableEffect() {
        prepareEffectChange();
        Effects::state.effectsGroup = Effects::EFFECT_DISABLED;
        Effects::state.effectSubmode = Effects::EFFECT_DISABLED;
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
    }

    void handleSetStartingEffect() {
        Settings::parameters.startingEffectsGroup = Effects::state.effectsGroup;
        Settings::parameters.startingEffectSubmode = Effects::state.effectSubmode;
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
    }

    void handleRemoveStartingEffect() {
        Settings::parameters.startingEffectsGroup = Effects::EFFECT_DISABLED;
        Settings::parameters.startingEffectSubmode = Effects::EFFECT_DISABLED;
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
    }

    void handleSettings() {
        if (server.hasArg("bright")) {
            Settings::parameters.globalBrightness = static_cast<uint8_t>(server.arg("bright").toInt());
        }
        if (server.hasArg("delay")) {
            Settings::parameters.allModeDelay = static_cast<uint32_t>(server.arg("delay").toInt());
        }
        if (server.hasArg("order")) {
            Settings::parameters.allModesWorkType = static_cast<uint8_t>(server.arg("order").toInt());
        }
        if (server.hasArg("wifi_off")) {
            Settings::parameters.isWifiAutoOffEnable = (server.arg("wifi_off") == "1");
        }

        Settings::save();

        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
        delay(100U);
        EspClass::restart();
    }

    void init() {
        (void) WiFi.mode(WIFI_AP);
        delay(1U);
        const IPAddress localIp(Settings::parameters.localIpVal[0], Settings::parameters.localIpVal[1],
                                Settings::parameters.localIpVal[2], Settings::parameters.localIpVal[3]);
        const IPAddress gateway(Settings::parameters.gatewayVal[0], Settings::parameters.gatewayVal[1],
                                Settings::parameters.gatewayVal[2], Settings::parameters.gatewayVal[3]);
        const IPAddress subnet(Settings::parameters.subnetVal[0], Settings::parameters.subnetVal[1],
                               Settings::parameters.subnetVal[2], Settings::parameters.subnetVal[3]);
        delay(1U);
        (void) WiFi.softAPConfig(localIp, gateway, subnet);
        (void) WiFi.softAP(&Settings::parameters.localNetworkSsid[0], &Settings::parameters.localNetworkPassword[0]);

        server.on("/", HTTP_GET, &handleRoot);
        server.on("/set_effect", HTTP_POST, &handleEnableEffect);
        server.on("/reset_effect", HTTP_POST, &handleDisableEffect);
        server.on("/set_startup", HTTP_POST, &handleSetStartingEffect);
        server.on("/remove_startup", HTTP_POST, &handleRemoveStartingEffect);
        server.on("/save", HTTP_POST, &handleSettings);

        server.begin();
    }

    void checkServer() { server.handleClient(); }
} // namespace WebControlServer
