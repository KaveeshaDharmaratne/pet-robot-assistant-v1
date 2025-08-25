#pragma once
#include <vector>
#include <stdint.h>

namespace audio
{
    struct WavEncoder
    {
        static std::vector<uint8_t> mono16_16k(const std::vector<int16_t> &pcm)
        {
            const uint32_t dataSize = pcm.size() * sizeof(int16_t);
            const uint32_t fileSize = 44 + dataSize;
            std::vector<uint8_t> wav;
            wav.reserve(fileSize);
            uint8_t hdr[44] = {
                'R', 'I', 'F', 'F',
                (uint8_t)(fileSize - 8), (uint8_t)((fileSize - 8) >> 8), (uint8_t)((fileSize - 8) >> 16), (uint8_t)((fileSize - 8) >> 24),
                'W', 'A', 'V', 'E',
                'f', 'm', 't', ' ',
                16, 0, 0, 0,
                1, 0,
                1, 0,
                0x80, 0x3E, 0, 0,
                0x00, 0x7D, 0, 0,
                2, 0,
                16, 0,
                'd', 'a', 't', 'a',
                (uint8_t)(dataSize), (uint8_t)(dataSize >> 8), (uint8_t)(dataSize >> 16), (uint8_t)(dataSize >> 24)};
            wav.insert(wav.end(), hdr, hdr + 44);
            wav.insert(wav.end(), (const uint8_t *)pcm.data(), (const uint8_t *)pcm.data() + dataSize);
            return wav;
        }
    };
}
