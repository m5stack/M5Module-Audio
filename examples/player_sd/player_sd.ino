/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 *
 * @Hardwares: M5Core/M5Core2/M5CoreS3 + Module Audio
 * @Dependent Library:
 * M5Unified: https://github.com/m5stack/M5Unified
 * Module Audio: https://github.com/m5stack/M5Module-Audio
 *
 * @Note: If use CoreS3, please switch to the B Pins.
 * @Note: Please place hello.wav in the root directory of the SD card.
 */

#include "M5Unified.h"
#include "M5Module_Audio.h"
#include <SPI.h>
#include <SD.h>

M5ModuleAudio device;

void setup()
{
    delay(1000);
    auto cfg            = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    if (!device.begin(Wire)) {
        while (1) {
            Serial.println("Module Audio not found");
            delay(1000);
        }
    }
    device.setHPMode(AUDIO_HPMODE_NATIONAL);
    device.setMICStatus(AUDIO_MIC_OPEN);
    device.setRGBBrightness(100);
    Serial.printf("getHPMode:%d\n", device.getHPMode());
    Serial.printf("getMICStatus:%d\n", device.getMICStatus());
    for (int i = 0; i <= 2; i++) {
        device.setRGBLED(i, 0xFFFFFF);
    }
    Serial.println("Read Reg ES8388 :");
    device.setMicAdcVolume(100);
    device.setSpeakerVolume(40);
    device.setSpeakerOutput(DAC_OUTPUT_OUT1);
    device.setBitsSample(ES_MODULE_ADC, BIT_LENGTH_16BITS);
    device.setSampleRate(SAMPLE_RATE_44K);
    uint8_t *reg = device.readAllReg();
    if (reg != nullptr) {
        for (uint8_t i = 0; i < 53; i++) {
            Serial.printf("Reg-%02d = 0x%02x\r\n", i, reg[i]);
        }
    }
    // The SPI_CS pin on the M5Core, M5Core2, and M5CoreS3 is GPIO4
    if (SD.begin(GPIO_NUM_4)) {
        Serial.println("SD card initialized successfully");
    } else {
        while (1) {
            Serial.println("Failed to initialize SD card. Please check the SD card and try again.");
            delay(1000);
        }
    }
}

void loop()
{
    device.play(SD, "/hello.wav");
}