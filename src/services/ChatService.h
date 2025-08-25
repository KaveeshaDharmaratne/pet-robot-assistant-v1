#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "../utils/Result.h"
#include "../../include/Settings.h"
#include "../../include/Prompts.h"

namespace services
{
    struct ChatOptions
    {
        const char *model = settings::kChatModel;
        uint32_t timeoutMs = settings::kHttpTimeoutMs;
    };

    struct ChatService
    {
        static utils::Result<String> complete(const String &user, const char *apiKey, const ChatOptions &opt = {})
        {
            WiFiClientSecure client;
            client.setInsecure();
            HTTPClient http;
            http.begin(client, "https://api.openai.com/v1/chat/completions");
            http.addHeader("Authorization", String("Bearer ") + apiKey);
            http.addHeader("Content-Type", "application/json");
            String payload = String("{\"model\":\"") + opt.model + "\", \"messages\":[{\"role\":\"system\", \"content\":\"" + String(prompts::kSystem) + "\"}, {\"role\":\"user\", \"content\":\"" + user + "\"}]}";
            http.setTimeout(opt.timeoutMs);
            int code = http.POST(payload);
            if (code == HTTP_CODE_OK)
            {
                JsonDocument doc;
                auto e = deserializeJson(doc, http.getString());
                if (e)
                    return utils::Result<String>::Err(String("JSON Parse Error: ") + e.c_str());
                String out = doc["choices"][0]["message"]["content"].as<String>();
                return utils::Result<String>::Ok(out);
            }
            return utils::Result<String>::Err(String("Chat HTTP ") + code + " - " + http.errorToString(code));
        }
    };
}
