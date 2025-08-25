#pragma once
#include <WiFi.h>
#include "../utils/Logger.h"

namespace net
{
    class WiFiManager
    {
    public:
        static void connect(const char *ssid, const char *pass, uint32_t timeoutMs = 20000)
        {
            ::WiFi.begin(ssid, pass);
            uint32_t start = millis();
            while (::WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs)
            {
                delay(500);
                utils::Logger::info("Connecting to WiFi...");
            }
            if (::WiFi.status() == WL_CONNECTED)
            {
                utils::Logger::info("WiFi Connected: %s", ::WiFi.localIP().toString().c_str());
            }
            else
            {
                utils::Logger::warn("WiFi connect timeout");
            }
        }
    };
}
