#pragma once
#include <vector>
#include <stdint.h>
#include "I2SManager.h"

namespace audio
{
    struct Microphone
    {
        static std::vector<int16_t> recordSeconds(int seconds, int sampleRate = 16000)
        {
            std::vector<int16_t> samples;
            if (!I2SManager::instance().beginRx(sampleRate, 16))
                return samples;

            const int bufferLen = 64;
            int16_t sBuffer[bufferLen];
            const int totalSamples = sampleRate * seconds;
            int samplesRead = 0;

            while (samplesRead < totalSamples)
            {
                size_t bytesIn = 0;
                esp_err_t result = i2s_read(I2S_NUM_0, &sBuffer, bufferLen * sizeof(int16_t), &bytesIn, portMAX_DELAY);
                if (result == ESP_OK && bytesIn > 0)
                {
                    int inBuf = bytesIn / sizeof(int16_t);
                    for (int i = 0; i < inBuf && samplesRead < totalSamples; ++i)
                    {
                        samples.push_back(sBuffer[i]);
                        ++samplesRead;
                    }
                }
            }

            I2SManager::instance().endRx();
            return samples;
        }
    };
}
