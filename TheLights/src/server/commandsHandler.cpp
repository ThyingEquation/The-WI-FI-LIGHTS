#include "commandsHandler.h"

#include "effects.h"
#include "localWifiServer.h"
#include "settings.h"

namespace Effects {
    EffectsState state;
}

namespace {
    bool isCommandReceived = false;
}

namespace CommandsHandler{
    void handleConnectionState(const std::string_view command) {
        if (command == "check") {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("imHere");
            (void) LocalWifiServer::Udp.endPacket();
        }
    }

    void handleSettings(std::string_view command) {
        constexpr std::array<std::string_view, 8> commands = {
            "ssid=",         "startingEffect=", "brightness=", "restart",
            "allModesTime=", "allModes=",       "wifiOff",     "wifiAutoOff="};

        bool isFailed = false;

        if (command.rfind(commands[0U]) == 0U) {
            command.remove_prefix(commands[0U].length());
            const size_t len = std::min(command.size(), static_cast<size_t>(32));
            (void)command.copy(&Settings::settings.localNetworkSsid[0], len);
            Settings::settings.localNetworkSsid[len] = '\0';
        } else if (command.rfind(commands[1U]) == 0U) {
            command.remove_prefix(commands[1U].length());
            if (const size_t separatorPos = command.find('-'); separatorPos != std::string_view::npos) {
                uint32_t groupTemp = 0U;
                uint32_t submodeTemp = 0U;
                if (sscanf(std::string(command.substr(0U, separatorPos)).c_str(), "%u", &groupTemp) != 1 ||
                    sscanf(std::string(command.substr(separatorPos + 1U)).c_str(), "%u", &submodeTemp) != 1) {
                    isFailed = true;
                    } else if (groupTemp > 255U || submodeTemp > 255U) {
                        isFailed = true;
                    } else {
                        Settings::settings.startingEffectsGroup = static_cast<uint8_t>(groupTemp);
                        Settings::settings.startingEffectSubmode = static_cast<uint8_t>(submodeTemp);
                    }
            }
        } else if (command.rfind(commands[2U]) == 0U) {
            command.remove_prefix(commands[2U].length());
            uint32_t val = 0U;
            if (sscanf(std::string(command).c_str(), "%u", &val) != 1) {
                isFailed = true;
            } else if (val > 255U) {
                isFailed = true;
            } else {
                Settings::settings.globalBrightness = static_cast<uint8_t>(val);
            }
        } else if (command.rfind(commands[3U]) == 0U) {
            Settings::saveSettings();
            EspClass::restart();
        } else if (command.rfind(commands[4U]) == 0U) {
            command.remove_prefix(commands[4U].length());
            uint32_t allModeDelayTemp = 0U;
            if (sscanf(std::string(command).c_str(), "%u", &allModeDelayTemp) != 1) {
                isFailed = true;
            } else if (allModeDelayTemp > 3600U) {
                isFailed = true;
            } else {
                Settings::settings.allModeDelay = allModeDelayTemp * 1000U;
            }
        } else if (command.rfind(commands[5U]) == 0U) {
            command.remove_prefix(commands[5U].length());
            uint32_t allModesWorkTypeTemp = 0U;
            if (sscanf(std::string(command).c_str(), "%u", &allModesWorkTypeTemp) != 1) {
                isFailed = true;
            } else if (allModesWorkTypeTemp > 1U) {
                isFailed = true;
            } else {
                Settings::settings.allModesWorkType = static_cast<uint8_t>(allModesWorkTypeTemp);
            }
        } else if (command.rfind(commands[6U]) == 0U) {
            Effects::state.isWifiActive = false;
            (void)WiFi.softAPdisconnect(true);
        } else if (command.rfind(commands[7U]) == 0U) {
            command.remove_prefix(commands[7U].length());
            bool isWifiAutoOffEnableTemp = false;
            if (sscanf(std::string(command).c_str(), "%u", &isWifiAutoOffEnableTemp) != 1) {
                isFailed = true;
            } else if (isWifiAutoOffEnableTemp) {
                isFailed = true;
            } else {
                Settings::settings.isWifiAutoOffEnable = isWifiAutoOffEnableTemp;
            }
        } else {
            isFailed = true;
        }

        if (isFailed) {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("Error settings");
            (void) LocalWifiServer::Udp.endPacket();
        } else {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("Setting ok");
            (void) LocalWifiServer::Udp.endPacket();
        }
    }

    void handleCanvas(std::string_view command) {
        std::string_view modeString;
        isCommandReceived = true;
        bool isFailed = false;

        const size_t modeStart = command.find("mode=");
        const size_t modeEnd = command.find('&', modeStart);

        if (modeStart == std::string_view::npos) {
            isFailed = true; /* return */
        } else {
            modeString = command.substr(modeStart + 5U, modeEnd - (modeStart + 5U));
        }

        const size_t colorStart = command.find("color=");
        uint8_t color = 0U;
        if (colorStart == std::string_view::npos) {
            isFailed = true;
        } else {
            const size_t colorEnd = command.find('&', colorStart);
            const std::string_view colorString = command.substr(colorStart + 6U, colorEnd - (colorStart + 6U));
            uint32_t colorTemp = 0U;
            if (sscanf(std::string(colorString).c_str(), "%u", &colorTemp) != 1) {
                isFailed = true;
            } else if (colorTemp > 20U) {
                isFailed = true;
            } else {
                color = static_cast<uint8_t>(colorTemp);
            }
        }

        const size_t ledStart = command.find("led=");
        uint16_t ledNum = 0U;
        if (ledStart == std::string_view::npos) {
            isFailed = true;
        } else {
            const std::string_view ledString = command.substr(ledStart + 4U);
            uint32_t ledNumTemp = 0U;
            if (sscanf(std::string(ledString).c_str(), "%u", &ledNumTemp) != 1) {
                isFailed = true;
            } else if (ledNumTemp > 257U) {
                isFailed = true;
            } else {
                ledNum = static_cast<uint16_t>(ledNumTemp);
            }
        }

        if (isFailed) {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("Error canvas");
            (void) LocalWifiServer::Udp.endPacket();
        } else {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("Ok");
            (void) LocalWifiServer::Udp.endPacket();

            Effects::drawOnCanvas(modeString, color, ledNum);
        }
    }

    void handleMainCommand(std::string_view command) {
        isCommandReceived = true;
        bool isFailed = false;

        Effects::state.isScreenClearEnable = true;
        Effects::state.isAllModesEnable = false;

        if (command.rfind("stop") == 0U) {
            Effects::state.isScreenClearEnable = true;
            Effects::state.effectsGroup = 255U;
        } else if (command.find('-') != std::string_view::npos) {
            if (const size_t separatorPos = command.find('-'); separatorPos != std::string_view::npos) {
                uint32_t groupTemp = 0U;
                uint32_t submodeTemp = 0U;
                if (sscanf(std::string(command.substr(0U, separatorPos)).c_str(), "%u", &groupTemp) != 1 ||
                    sscanf(std::string(command.substr(separatorPos + 1U)).c_str(), "%u", &submodeTemp) != 1) {
                    isFailed = true;
                    } else if (groupTemp > 255U || submodeTemp > 255U) {
                        isFailed = true;
                    } else {
                        Effects::state.effectsGroup = static_cast<uint8_t>(groupTemp);
                        Effects::state.effectSubmode = static_cast<uint8_t>(submodeTemp);
                        if (Effects::state.effectsGroup == 10U) {
                            //drawAnimations(255U);
                        }
                    }
            }
        } else {
            isFailed = true;
        }

        if (isFailed) {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("Error commands");
            (void) LocalWifiServer::Udp.endPacket();
        } else {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("Ok");
            (void) LocalWifiServer::Udp.endPacket();
        }
    }

    void sendVirtualCommand() { isCommandReceived = true; }
}

namespace Effects {
    bool checkCommandReceived() {
        bool returnValue = false;
        if (isCommandReceived) {
            isCommandReceived = false;
            returnValue = true;
        }
        else {
            returnValue = false;
        }
        return returnValue;
    }
}