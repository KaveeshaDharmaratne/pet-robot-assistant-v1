#pragma once
#include <Arduino.h>

namespace utils
{
    struct Logger
    {
        template <typename... Args>
        static void info(const char *fmt, Args... args)
        {
            Serial.printf("[INFO %lu] ", millis());
            Serial.printf(fmt, args...);
            Serial.println();
        }
        template <typename... Args>
        static void warn(const char *fmt, Args... args)
        {
            Serial.printf("[WARN %lu] ", millis());
            Serial.printf(fmt, args...);
            Serial.println();
        }
        template <typename... Args>
        static void error(const char *fmt, Args... args)
        {
            Serial.printf("[ERR  %lu] ", millis());
            Serial.printf(fmt, args...);
            Serial.println();
        }
    };
}
