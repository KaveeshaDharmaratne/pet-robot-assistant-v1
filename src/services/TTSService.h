#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <AudioTools.h>
#include <AudioTools/AudioCodecs/CodecMP3Helix.h>
#include "../audio/I2SManager.h"
#include "../../include/Settings.h"

namespace services
{
    struct TTSOptions
    {
        const char *voiceId = settings::kElevenVoiceId;
        int sampleRate = 16000;
        uint32_t timeoutMs = settings::kHttpTimeoutMs;
    };

    struct TTSService
    {
        static void speak(const String &text, const char *apiKey, const TTSOptions &opt = {})
        {
            audio::I2SManager::instance().beginTx(opt.sampleRate, 2, 16);
            WiFiClientSecure client;
            client.setInsecure();
            HTTPClient http;
            String url = String("https://api.elevenlabs.io/v1/text-to-speech/") + opt.voiceId + "/stream";
            http.begin(client, url);
            http.addHeader("xi-api-key", apiKey);
            http.addHeader("Content-Type", "application/json");
            String payload = String("{\"text\":\"") + text + "\", \"model_id\":\"eleven_monolingual_v1\", \"voice_settings\":{\"speed\":0.7,\"stability\":0.7,\"similarity_boost\":0.75}}";
            int code = http.POST(payload);
            if (code == HTTP_CODE_OK)
            {
                audio_tools::MP3DecoderHelix decoder;
                auto &i2s = audio::I2SManager::instance().txStream();
                audio_tools::EncodedAudioStream stream(&i2s, &decoder);
                stream.begin(audio_tools::AudioInfo(opt.sampleRate, 2, 16));
                audio_tools::StreamCopy copier;
                copier.begin(stream, *http.getStreamPtr());
                while (copier.copy())
                {
                    delay(1);
                }
                copier.end();
                stream.end();
            }
            audio::I2SManager::instance().endTx();
        }
    };
}
