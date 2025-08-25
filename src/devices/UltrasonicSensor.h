#pragma once
#include <HCSR04.h>
#include "Pins.h"

namespace devices
{
    class UltrasonicSensor
    {
    public:
        static UltraSonicDistanceSensor &instance()
        {
            static UltraSonicDistanceSensor sensor(PIN_TRIG, PIN_ECHO);
            return sensor;
        }
        static float distanceCm() { return instance().measureDistanceCm(); }
        static bool personDetected(float d, float thresholdCm = 20.0f)
        {
            return (d > 0) && (d < thresholdCm);
        }
    };
}
