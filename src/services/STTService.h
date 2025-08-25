#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <vector>
#include <stdint.h>
#include "../utils/Result.h"
#include "../utils/Logger.h"
#include "../../include/Settings.h"

namespace services
{
    struct STTOptions
    {
        const char *model = settings::kWhisperModel;
        uint32_t timeoutMs = settings::kHttpTimeoutMs;
    };

    struct STTService
    {
        static utils::Result<String> transcribe(const std::vector<uint8_t> &wavData, const char *apiKey, const STTOptions &opt = {})
        {
            String boundary = "----WebKitFormBoundary7MA4YWkTrZu0gW";
            std::vector<uint8_t> payload;
            auto addStr = [&](const String &s)
            { payload.insert(payload.end(), (const uint8_t *)s.c_str(), (const uint8_t *)s.c_str() + s.length()); };
            addStr("--" + boundary + "\r\n");
            addStr("Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n");
            addStr("Content-Type: audio/wav\r\n\r\n");
            payload.insert(payload.end(), wavData.begin(), wavData.end());
            addStr("\r\n--" + boundary + "\r\n");
            addStr("Content-Disposition: form-data; name=\"model\"\r\n\r\n");
            addStr(String(opt.model) + "\r\n");
            addStr("--" + boundary + "--\r\n");

            WiFiClientSecure client;
            client.setInsecure();
            HTTPClient http;
            http.begin(client, "https://api.openai.com/v1/audio/transcriptions");
            http.addHeader("Authorization", String("Bearer ") + apiKey);
            http.addHeader("Content-Type", String("multipart/form-data; boundary=") + boundary);
            http.setTimeout(opt.timeoutMs);
            int code = http.POST(payload.data(), payload.size());
            if (code == HTTP_CODE_OK)
            {
                String response = http.getString();
                JsonDocument doc;
                auto err = deserializeJson(doc, response);
                if (err)
                    return utils::Result<String>::Err(String("JSON Parse Error: ") + err.c_str());
                if (doc["text"].is<String>())
                    return utils::Result<String>::Ok(doc["text"].as<String>());
                return utils::Result<String>::Err("No 'text' field in API response");
            }
            String errMsg = String("STT HTTP ") + code + " - " + http.errorToString(code);
            return utils::Result<String>::Err(errMsg);
        }
    };
}
