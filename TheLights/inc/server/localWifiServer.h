#pragma once

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace LocalWifiServer {
    void initAccessPointServer();
    void checkAccessPointServer();

    inline WiFiUDP Udp;
}

namespace SettingsWifiServer {
    void settingsServer();
}