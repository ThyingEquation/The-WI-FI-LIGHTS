#include <string>

#include "localWifiServer.h"
#include "commandsHandler.h"
#include "settings.h"


namespace LocalWifiServer {
    void initLightServer() {
        (void)WiFi.mode(WIFI_AP);
        delay(50U);

        const IPAddress localIp(Settings::settings.localIpVal[0], Settings::settings.localIpVal[1],
                                Settings::settings.localIpVal[2], Settings::settings.localIpVal[3]);
        const IPAddress gateway(Settings::settings.gatewayVal[0], Settings::settings.gatewayVal[1],
                                Settings::settings.gatewayVal[2], Settings::settings.gatewayVal[3]);
        const IPAddress subnet(Settings::settings.subnetVal[0], Settings::settings.subnetVal[1],
                               Settings::settings.subnetVal[2], Settings::settings.subnetVal[3]);
        delay(50U);

        (void)WiFi.softAPConfig(localIp, gateway, subnet);
        delay(50U);
        (void)WiFi.softAP(&Settings::settings.ssid[0], &Settings::settings.password[0]);
        delay(50U);

        (void)Udp.begin(1653U);
    }

    void checkLightServer() {
        if (Udp.parsePacket()) {
            char incomingPacket[256U];
            if (const int len = Udp.read(incomingPacket, 255U); len > 0) {
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
                    (void)Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
                    (void)Udp.write("Error main");
                    (void)Udp.endPacket();
                }
        }
    }
}