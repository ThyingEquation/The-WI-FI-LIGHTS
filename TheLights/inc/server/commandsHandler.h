#pragma once
#include <string>

namespace CommandsHandler{
    void handleConnectionState(std::string_view command);
    void handleSettings(std::string_view command);
    void handleCanvas(std::string_view command);
    void handleMainCommand(std::string_view command);
    void sendVirtualCommand();
}