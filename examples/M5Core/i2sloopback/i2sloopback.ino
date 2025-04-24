/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 *
 * @Hardwares: M5Core Basic v2.7+ Module Audio
 * @Dependent Library:
 * M5Unified@^0.2.5: https://github.com/m5stack/M5Unified
 * Module Audio:
 */

#include "M5Unified.h"
#include "audio_i2c.hpp"
#include "es8388.hpp"
#include "driver/i2s.h"
AudioI2c device;

#define SYS_I2C_SDA_PIN  21
#define SYS_I2C_SCL_PIN  22
#define SYS_I2S_MCLK_PIN 0
#define SYS_I2S_SCLK_PIN 13
#define SYS_I2S_LRCK_PIN 12
#define SYS_I2S_DOUT_PIN 15
#define SYS_I2S_DIN_PIN  34
#define SYS_SPI_MISO_PIN 19
#define SYS_SPI_MOSI_PIN 23
#define SYS_SPI_CLK_PIN  18
#define SYS_SPI_CS_PIN   4

ES8388 es8388(&Wire, SYS_I2C_SDA_PIN, SYS_I2C_SCL_PIN);

uint16_t rxbuf[256], txbuf[256];
size_t readsize = 0;
byte error, address;
const uint32_t color[]  = {0xFF0000, 0xFF0000, 0xFF0000, 0x00FF00, 0xFFFF00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF};
i2s_config_t i2s_config = {.mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
                           .sample_rate          = 44100,
                           .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
                           .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
                           .communication_format = I2S_COMM_FORMAT_STAND_I2S,
                           .intr_alloc_flags     = 0,
                           .dma_buf_count        = 8,
                           .dma_buf_len          = 512,
                           .use_apll             = false,
                           .tx_desc_auto_clear   = true,
                           .fixed_mclk           = 0};

i2s_pin_config_t pin_config = {
    .mck_io_num   = SYS_I2S_MCLK_PIN,
    .bck_io_num   = SYS_I2S_SCLK_PIN,
    .ws_io_num    = SYS_I2S_LRCK_PIN,
    .data_out_num = SYS_I2S_DOUT_PIN,
    .data_in_num  = SYS_I2S_DIN_PIN,
};

void setup()
{
    M5.begin();
    Serial.begin(115200);
    for (int address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("Found I2C device at address 0x");
            Serial.print(address, HEX);
            Serial.print("   \n");
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            Serial.println(address, HEX);
        }
    }
    device.begin(&Wire, SYS_I2C_SCL_PIN, SYS_I2C_SCL_PIN);
    device.setHPMode(AUDIO_HPMODE_NATIONAL);
    device.setMICStatus(AUDIO_MIC_OPEN);
    device.setRGBBrightness(100);
    Serial.printf("getHPMode:%d\n", device.getHPMode());
    Serial.printf("getMICStatus:%d\n", device.getMICStatus());
    for (int i = 0; i <= 2; i++) {
        device.setRGBLED(i, color[i + 3]);
        // Output the hexadecimal value of the current color
        Serial.printf("Set RGB to %06X\n", (unsigned int)color[i + 3]);
        Serial.printf("get RGB to %06X\n", device.getRGBLED(i));
    }
    Serial.println("Read Reg ES8388 : ");
    if (!es8388.init()) Serial.println("Init Fail");
    es8388.setADCInput(ADC_INPUT_LINPUT1_RINPUT1);
    es8388.setMicGain(MIC_GAIN_24DB);
    es8388.setADCVolume(100);
    // The volume output should not exceed 40, otherwise there will be noise or current sounds
    es8388.setDACVolume(40);
    es8388.setDACOutput(DAC_OUTPUT_OUT1);
    es8388.setMixSourceSelect(MIXADC, MIXADC);
    es8388.setBitsSample(ES_MODULE_ADC, BIT_LENGTH_16BITS);
    es8388.setSampleRate(SAMPLE_RATE_44K);
    uint8_t *reg;
    for (uint8_t i = 0; i < 53; i++) {
        reg = es8388.readAllReg();
        Serial.printf("Reg-%02d = 0x%02x\r\n", i, reg[i]);
    }
    // i2s
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void loop()
{
    i2s_read(I2S_NUM_0, &rxbuf[0], 256 * 2, &readsize, 1000);
    for (int i = 0; i < 256; i++) {
        // direct transfer too txbuff
        txbuf[i] = rxbuf[i];
        // txbuf[i] = 0; //mute
    }
    // play received buffer
    i2s_write(I2S_NUM_0, &txbuf[0], 256 * 2, &readsize, 1000);
}