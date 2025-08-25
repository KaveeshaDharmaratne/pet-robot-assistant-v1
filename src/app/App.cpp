#include "App.h"
#include <Arduino.h>
#include "../devices/UltrasonicSensor.h"
#include "../audio/Microphone.h"
#include "../audio/WavEncoder.h"
#include "../services/STTService.h"
#include "../services/ChatService.h"
#include "../services/TTSService.h"
#include "../net/WiFiManager.h"
#include "../utils/Logger.h"
#include "../utils/Stopwatch.h"
#include "../../include/Settings.h"
#include "../../config.example.h"

using namespace app;

void App::setup()
{
    Serial.begin(115200);
    // Mic power control removed: mic is always powered by the board
    net::WiFiManager::connect(WIFI_SSID, WIFI_PASS);
    stateStartMs_ = millis();
    utils::Logger::info("Setup complete. Ready for voice recording test.");
}

void App::loopTick()
{
    float distance = devices::UltrasonicSensor::distanceCm();
    if (devices::UltrasonicSensor::personDetected(distance, settings::kDetectThresholdCm))
    {
        switch (state_)
        {
        case ConvState::Wait:
            break;
        case ConvState::Countdown:
            if (millis() - stateStartMs_ >= 1000)
            {
                utils::Logger::info("Countdown: %d", countdown_);
                countdown_--;
                stateStartMs_ = millis();
            }
            if (countdown_ <= 0)
            {
                utils::Logger::info("Recording now! Speak for %d seconds...", settings::kRecordSeconds);
                toState(ConvState::Record);
            }
            break;
        case ConvState::Record:
        {
            auto pcm = audio::Microphone::recordSeconds(settings::kRecordSeconds);
            if (pcm.empty())
            {
                utils::Logger::warn("No audio captured");
                toState(ConvState::Next);
                break;
            }
            auto wav = audio::WavEncoder::mono16_16k(pcm);
            auto stt = services::STTService::transcribe(wav, OPENAI_API_KEY);
            userInput_ = stt.ok ? stt.value : String("");
            utils::Logger::info("Transcribed text: %s", userInput_.c_str());
            toState(ConvState::Process);
            break;
        }
        case ConvState::Process:
        {
            if (userInput_.length() > 0)
            {
                utils::Logger::info("Getting AI response...");
                auto ai = services::ChatService::complete(userInput_, OPENAI_API_KEY);
                aiResponse_ = ai.ok ? ai.value : String("Sorry, I didn't understand that.");
                utils::Logger::info("AI Response: %s", aiResponse_.c_str());
                utils::Logger::info("Playing TTS response...");
                services::TTSService::speak(aiResponse_, ELEVENLABS_API_KEY);
            }
            else
            {
                utils::Logger::warn("No speech detected or transcription failed.");
            }
            toState(ConvState::Next);
            break;
        }
        case ConvState::Next:
            if (millis() - stateStartMs_ > 1000)
            {
                countdown_ = settings::kCountdownSeconds;
                toState(ConvState::Countdown);
            }
            break;
        }
    }
    else if ((distance <= 0 || distance >= 100))
    {
        utils::Logger::info("No person detected. Waiting...");
    }
}

void App::toState(ConvState s)
{
    state_ = s;
    stateStartMs_ = millis();
}
