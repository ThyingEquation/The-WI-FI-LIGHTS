#pragma once

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace LocalWifiServer {
    void initLightServer();
    void checkLightServer();

    inline WiFiUDP Udp;
}
