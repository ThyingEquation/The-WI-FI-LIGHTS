#include <ESP8266WebServer.h>
#include <string>

#include "commandsHandler.h"
#include "localWifiServer.h"
#include "settings.h"


namespace LocalWifiServer {
    void initAccessPointServer() {
        (void) WiFi.mode(WIFI_AP);
        delay(1U);
        const IPAddress localIp(Settings::settings.localIpVal[0], Settings::settings.localIpVal[1],
                                Settings::settings.localIpVal[2], Settings::settings.localIpVal[3]);
        const IPAddress gateway(Settings::settings.gatewayVal[0], Settings::settings.gatewayVal[1],
                                Settings::settings.gatewayVal[2], Settings::settings.gatewayVal[3]);
        const IPAddress subnet(Settings::settings.subnetVal[0], Settings::settings.subnetVal[1],
                               Settings::settings.subnetVal[2], Settings::settings.subnetVal[3]);
        delay(1U);
        (void) WiFi.softAPConfig(localIp, gateway, subnet);
        (void) WiFi.softAP(&Settings::settings.localNetworkSsid[0], &Settings::settings.localNetworkPassword[0]);
        delay(1U);
        (void) Udp.begin(1653U);
    }

    void checkAccessPointServer() {
        if (Udp.parsePacket()) {
            char incomingPacket[256U] = {};
            if (const uint32_t len = static_cast<uint32_t>(Udp.read(&incomingPacket[0], 255U)); len > 0U) {
                incomingPacket[len] = 0U;
            }

            const auto command = std::string_view(incomingPacket);

            if (constexpr std::array<std::string_view, 4U> commands = {"connectionState?", "settings?", "canvas?",
                                                                       "command?"};
                command.rfind(commands[0U]) == 0U) {
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

namespace SettingsWifiServer {
    ESP8266WebServer server(80);

    void handleRoot() {
        String html = "<html><head><meta charset='UTF-8'><title>Настройки Гирлянды</title>";
        html += "<style>body { font-family: sans-serif; margin: 20px; } input { margin-bottom: 10px; width: 100%; "
                "padding: 8px; }</style>";
        html += "</head><body>";
        html += "<h2>Настройки Гирлянды</h2>";

        html += "<form action='/save' method='POST'>";

        html += "Имя гирлянды (текущее: <b>" + static_cast<String>(Settings::settings.localNetworkSsid) + "</b>):<br>";
        html += "<input type='text' name='g_name' placeholder='Введите новое имя'><br><br>";

        html += "Имя вашей Wi-Fi сети (текущее: <b>" + static_cast<String>(Settings::settings.stationNetworkSsid) +
                "</b>):<br>";
        html += "<input type='text' name='wifi_ssid' placeholder='Введите новое имя'><br><br>";

        html += "Пароль вашей Wi-Fi сети (текущее: <b>" +
                static_cast<String>(Settings::settings.stationNetworkPassword) + "</b>):<br>";
        html += "<input type='text' name='wifi_pass' placeholder='Введите новый пароль'><br><br>";

        html += "Тип работы Wi-Fi сети:<br>";
        html += "<select name='modeWifi'>";
        html += "<option value='wifiAP'" + String(!Settings::settings.isLocalAccessPoint ? " selected" : "") +
                ">Локальная точка доступа</option>";
        html += "<option value='wifiSTA'" + String(Settings::settings.isLocalAccessPoint ? " selected" : "") +
                ">Работа в сторонней сети</option>";
        html += "</select><br><br>";

        html += "Режим работы:<br>";
        html += "<select name='workMode'>";
        html += "<option value='android'" + String(Settings::settings.isAndroidAppWorkMode ? " selected" : "") +
                ">Работа через андроид приложение</option>";
        html += "<option value='web'" + String(!Settings::settings.isAndroidAppWorkMode ? " selected" : "") +
                ">Работа через упрощенную веб-страницу</option>";
        html += "</select><br><br>";

        html += "<input type='submit' value='Сохранить и перезагрузить' style='background: #4CAF50; color: white; "
                "border: none; cursor: pointer;'>";
        html += "</form></body></html>";

        server.send(200, "text/html", html);
    }

    void handleSave() {
        if (server.hasArg("g_name") && server.arg("g_name") != "") {
            const String gName = server.arg("g_name");
            (void)strncpy(&Settings::settings.localNetworkSsid[0], gName.c_str(), 32U);
            Settings::settings.localNetworkSsid[32] = '\0';
        }
        if (server.hasArg("wifi_ssid") && server.arg("wifi_ssid") != "") {
            const String gName = server.arg("wifi_ssid");
            (void)strncpy(&Settings::settings.stationNetworkSsid[0], gName.c_str(), 32U);
            Settings::settings.stationNetworkSsid[32] = '\0';
        }
        if (server.hasArg("wifi_pass") && server.arg("wifi_pass") != "") {
            const String gName = server.arg("wifi_pass");
            (void)strncpy(&Settings::settings.stationNetworkPassword[0], gName.c_str(), 63U);
            Settings::settings.stationNetworkPassword[63] = '\0';
        }

        if (server.hasArg("modeWifi")) {
            Settings::settings.isLocalAccessPoint = (server.arg("modeWifi") == "wifiSTA");
        }

        if (server.hasArg("workMode")) {
            Settings::settings.isAndroidAppWorkMode = (server.arg("workMode") == "android");
        }

        const String msg = "<html><head><meta charset='UTF-8'></head><body><h3>Настройки сохранены!</h3><p>ESP8266 "
                     "перезагружается...</p></body></html>";
        server.send(200, "text/html", msg);

        Settings::saveSettings();
        delay(2000U);
        EspClass::restart();
    }

    void settingsServer() {
        (void) WiFi.mode(WIFI_AP);
        delay(1U);
        const IPAddress localIp(Settings::settings.localIpVal[0], Settings::settings.localIpVal[1],
                                Settings::settings.localIpVal[2], Settings::settings.localIpVal[3]);
        const IPAddress gateway(Settings::settings.gatewayVal[0], Settings::settings.gatewayVal[1],
                                Settings::settings.gatewayVal[2], Settings::settings.gatewayVal[3]);
        const IPAddress subnet(Settings::settings.subnetVal[0], Settings::settings.subnetVal[1],
                               Settings::settings.subnetVal[2], Settings::settings.subnetVal[3]);
        delay(1U);
        (void) WiFi.softAPConfig(localIp, gateway, subnet);
        (void) WiFi.softAP(String("Settings.") + Settings::settings.localNetworkSsid, "11111111");

        server.on("/", &handleRoot);
        server.on("/save", &handleSave);

        server.begin();

        while (true) {
            server.handleClient();
            delay(1U);
        }
    }
} // namespace SettingsWifiServer
