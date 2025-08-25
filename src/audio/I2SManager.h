#pragma once
#include <AudioTools.h>
#include <driver/i2s.h>
#include "../devices/Pins.h"

namespace audio
{
    class I2SManager
    {
    public:
        static I2SManager &instance()
        {
            static I2SManager s;
            return s;
        }

        audio_tools::I2SStream &txStream() { return i2s_; }

        void beginTx(int sampleRate = 16000, int channels = 2, int bits = 16)
        {
            auto cfg = i2s_.defaultConfig(TX_MODE);
            cfg.sample_rate = sampleRate;
            cfg.channels = channels;
            cfg.bits_per_sample = bits;
            cfg.pin_bck = PIN_I2S_BCLK;
            cfg.pin_ws = PIN_I2S_WS;
            cfg.pin_data = PIN_I2S_DOUT;
            cfg.use_apll = false;
            cfg.i2s_format = I2S_STD_FORMAT;
            cfg.buffer_count = 6;
            cfg.buffer_size = 256;
            i2s_.begin(cfg);
        }

        void endTx() { i2s_.end(); }

        bool beginRx(int sampleRate = 16000, int bits = 16)
        {
            endTx();
            const i2s_config_t i2s_config = {
                .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
                .sample_rate = sampleRate,
                .bits_per_sample = i2s_bits_per_sample_t(bits),
                .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
                .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
                .intr_alloc_flags = 0,
                .dma_buf_count = 8,
                .dma_buf_len = 64,
                .use_apll = false};
            const i2s_pin_config_t pin_config = {
                .bck_io_num = PIN_I2S_BCLK,
                .ws_io_num = PIN_I2S_WS,
                .data_out_num = -1,
                .data_in_num = PIN_I2S_DIN};
            if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK)
                return false;
            if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK)
            {
                i2s_driver_uninstall(I2S_NUM_0);
                return false;
            }
            if (i2s_start(I2S_NUM_0) != ESP_OK)
            {
                i2s_driver_uninstall(I2S_NUM_0);
                return false;
            }
            delay(100);
            return true;
        }

        void endRx()
        {
            i2s_stop(I2S_NUM_0);
            i2s_driver_uninstall(I2S_NUM_0);
        }

    private:
        audio_tools::I2SStream i2s_;
    };
}
