#pragma once

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace LocalWifiServer {
    void initAccessPointServer();
    void initStationServer();
    void checkServer();

    inline WiFiUDP Udp;
}

namespace SettingsServer {
    void checkServer();
}

namespace WebControlServer {
    void init();
    void checkServer();
}