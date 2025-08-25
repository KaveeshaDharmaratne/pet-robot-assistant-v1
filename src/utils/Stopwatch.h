#pragma once
#include <Arduino.h>

namespace utils
{
    class Stopwatch
    {
    public:
        Stopwatch() : startMs_(millis()) {}
        void reset() { startMs_ = millis(); }
        unsigned long elapsed() const { return millis() - startMs_; }
        bool elapsedAtLeast(unsigned long ms) const { return elapsed() >= ms; }

    private:
        unsigned long startMs_;
    };
}
