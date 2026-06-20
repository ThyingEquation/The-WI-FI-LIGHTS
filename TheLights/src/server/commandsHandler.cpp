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

namespace CommandsHandler {
    void handleConnectionState(const std::string_view command) {
        if (command == "check") {
            (void) LocalWifiServer::Udp.beginPacket(LocalWifiServer::Udp.remoteIP(), LocalWifiServer::Udp.remotePort());
            (void) LocalWifiServer::Udp.write("imHere");
            (void) LocalWifiServer::Udp.endPacket();
        }
    }

    void handleSettings(std::string_view command) {
        constexpr std::array<std::string_view, 12U> commands = {
                "ssidAp=", "passwordAp=",   "ssidSta=",  "passwordSta=", "startingEffect=", "brightness=",
                "restart", "allModesTime=", "allModes=", "wifiOff",      "wifiAutoOff=",    "allModesDataMask="};

        bool isFailed = false;

        if (command.rfind(commands[0U]) == 0U) {
            command.remove_prefix(commands[0U].length());
            const size_t len = std::min(command.size(), static_cast<size_t>(32));
            (void) command.copy(&Settings::parameters.localNetworkSsid[0], len);
            Settings::parameters.localNetworkSsid[len] = '\0';
        } else if (command.rfind(commands[1U]) == 0U) {
            command.remove_prefix(commands[1U].length());
            const size_t len = std::min(command.size(), static_cast<size_t>(64));
            (void) command.copy(&Settings::parameters.localNetworkPassword[0], len);
            Settings::parameters.localNetworkPassword[len] = '\0';
        } else if (command.rfind(commands[2U]) == 0U) {
            command.remove_prefix(commands[2U].length());
            const size_t len = std::min(command.size(), static_cast<size_t>(32));
            (void) command.copy(&Settings::parameters.stationNetworkSsid[0], len);
            Settings::parameters.stationNetworkSsid[len] = '\0';
        } else if (command.rfind(commands[3U]) == 0U) {
            command.remove_prefix(commands[3U].length());
            const size_t len = std::min(command.size(), static_cast<size_t>(64));
            (void) command.copy(&Settings::parameters.stationNetworkPassword[0], len);
            Settings::parameters.stationNetworkPassword[len] = '\0';
        } else if (command.rfind(commands[4U]) == 0U) {
            command.remove_prefix(commands[4U].length());
            if (const size_t separatorPos = command.find('-'); separatorPos != std::string_view::npos) {
                uint32_t groupTemp = 0U;
                uint32_t submodeTemp = 0U;
                if (sscanf(std::string(command.substr(0U, separatorPos)).c_str(), "%u", &groupTemp) != 1 ||
                    sscanf(std::string(command.substr(separatorPos + 1U)).c_str(), "%u", &submodeTemp) != 1) {
                    isFailed = true;
                } else if (groupTemp > Effects::EFFECT_DISABLED || submodeTemp > Effects::EFFECT_DISABLED) {
                    isFailed = true;
                } else {
                    Settings::parameters.startingEffectsGroup = groupTemp;
                    Settings::parameters.startingEffectSubmode = submodeTemp;
                }
            }
        } else if (command.rfind(commands[5U]) == 0U) {
            command.remove_prefix(commands[5U].length());
            uint32_t val = 0U;
            if (sscanf(std::string(command).c_str(), "%u", &val) != 1) {
                isFailed = true;
            } else if (val > Effects::EFFECT_DISABLED) {
                isFailed = true;
            } else {
                Settings::parameters.globalBrightness = val;
            }
        } else if (command.rfind(commands[6U]) == 0U) {
            Settings::save();
            EspClass::restart();
        } else if (command.rfind(commands[7U]) == 0U) {
            command.remove_prefix(commands[7U].length());
            uint32_t allModeDelayTemp = 0U;
            if (sscanf(std::string(command).c_str(), "%u", &allModeDelayTemp) != 1) {
                isFailed = true;
            } else if (allModeDelayTemp > 3600U) {
                isFailed = true;
            } else {
                Settings::parameters.allModeDelay = allModeDelayTemp * 1000U;
            }
        } else if (command.rfind(commands[8U]) == 0U) {
            command.remove_prefix(commands[8U].length());
            uint32_t allModesWorkTypeTemp = 0U;
            if (sscanf(std::string(command).c_str(), "%u", &allModesWorkTypeTemp) != 1) {
                isFailed = true;
            } else if (allModesWorkTypeTemp > 1U) {
                isFailed = true;
            } else {
                Settings::parameters.allModesWorkType = allModesWorkTypeTemp;
            }
        } else if (command.rfind(commands[9U]) == 0U) {
            Effects::state.isWifiActive = false;
            if (Settings::parameters.workMode == static_cast<uint32_t>(Settings::AnimationsDelays::ANDROID_STA)) {
                (void) WiFi.disconnect(true);
            } else {
                (void) WiFi.softAPdisconnect(true);
            }
        } else if (command.rfind(commands[10U]) == 0U) {
            command.remove_prefix(commands[10U].length());
            bool isWifiAutoOffEnableTemp = false;
            if (sscanf(std::string(command).c_str(), "%u", &isWifiAutoOffEnableTemp) != 1) {
                isFailed = true;
            } else if (isWifiAutoOffEnableTemp) {
                Settings::parameters.isWifiAutoOffEnable = true;
            } else if (!isWifiAutoOffEnableTemp) {
                Settings::parameters.isWifiAutoOffEnable = false;
            } else {
                isFailed = true;
            }
        } else if (command.rfind(commands[11U]) == 0U) {
            command.remove_prefix(commands[11U].length());
            uint8_t tempMask[Settings::EFFECTS_BITMASK_SIZE] = {0U};
            for (size_t i = 0U; i < Settings::EFFECTS_BITMASK_SIZE; ++i) {
                unsigned int byteVal = 0U;
                std::string hexByte(command.substr(i * 2U, 2U));
                if (sscanf(hexByte.c_str(), "%x", &byteVal) != 1) {
                    isFailed = true;
                    break;
                }
                tempMask[i] = static_cast<uint8_t>(byteVal);
            }
            if (!isFailed) {
                (void)std::copy(std::begin(tempMask), std::end(tempMask),
                          std::begin(Settings::parameters.enabledEffectsMask));
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
            Effects::state.effectsGroup = Effects::EFFECT_DISABLED;
        } else if (command.find('-') != std::string_view::npos) {
            if (const size_t separatorPos = command.find('-'); separatorPos != std::string_view::npos) {
                uint32_t groupTemp = 0U;
                uint32_t submodeTemp = 0U;
                if (sscanf(std::string(command.substr(0U, separatorPos)).c_str(), "%u", &groupTemp) != 1 ||
                    sscanf(std::string(command.substr(separatorPos + 1U)).c_str(), "%u", &submodeTemp) != 1) {
                    isFailed = true;
                } else if (groupTemp > Effects::EFFECT_DISABLED || submodeTemp > Effects::EFFECT_DISABLED) {
                    isFailed = true;
                } else {
                    Effects::state.effectsGroup = static_cast<uint8_t>(groupTemp);
                    Effects::state.effectSubmode = static_cast<uint8_t>(submodeTemp);
                    if (Effects::state.effectsGroup == 10U) {
                        Effects::drawAnimations(Effects::EFFECT_DISABLED);
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
} // namespace CommandsHandler

namespace Effects {
    bool checkCommandReceived() {
        bool returnValue = false;
        if (isCommandReceived) {
            isCommandReceived = false;
            returnValue = true;
        } else {
            returnValue = false;
        }
        return returnValue;
    }
} // namespace Effects
