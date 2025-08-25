#pragma once
#include <WString.h>

namespace app
{
    enum class ConvState
    {
        Wait,
        Countdown,
        Record,
        Process,
        Next
    };

    class App
    {
    public:
        void setup();
        void loopTick();

    private:
        ConvState state_ = ConvState::Countdown;
        unsigned long stateStartMs_ = 0;
        int countdown_ = 1;
        String userInput_;
        String aiResponse_;

        void toState(ConvState s);
    };
}
