/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "audio_i2c.hpp"

void AudioI2c::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    _wire->beginTransmission(addr);
    _wire->write(reg);
    for (int i = 0; i < length; i++) {
        _wire->write(*(buffer + i));
    }
    _wire->endTransmission();
#if AUDIO_I2C_DEBUG
    Serial.print("Write bytes: [");
    Serial.print(addr);
    Serial.print(", ");
    Serial.print(reg);
    Serial.print(", ");
    for (int i = 0; i < length; i++) {
        Serial.print(buffer[i]);
        if (i < length - 1) {
            Serial.print(", ");
        }
    }
    Serial.println("]");
#else
#endif
}

void AudioI2c::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(addr, length);
    for (int i = 0; i < length; i++) {
        buffer[index++] = _wire->read();
    }
#if AUDIO_I2C_DEBUG
    Serial.print("Read bytes: [");
    Serial.print(addr);
    Serial.print(", ");
    Serial.print(reg);
    Serial.print(", ");
    for (int i = 0; i < length; i++) {
        Serial.print(buffer[i]);
        if (i < length - 1) {
            Serial.print(", ");
        }
    }
    Serial.println("]");
#else
#endif
}

bool AudioI2c::begin(TwoWire *wire, uint8_t sda, uint8_t scl, uint32_t speed, uint8_t addr)
{
    _wire  = wire;
    _sda   = sda;
    _scl   = scl;
    _addr  = addr;
    _speed = speed;
    _wire->begin(_sda, _scl, _speed);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

void AudioI2c::setMICStatus(audio_mic_t status)
{
    uint8_t reg = MICROPHONE_STATUS;
    writeBytes(_addr, reg, (uint8_t *)&status, 1);
}

void AudioI2c::setHPMode(audio_hpmode_t mode)
{
    uint8_t reg = HEADPHONE_MODE;
    writeBytes(_addr, reg, (uint8_t *)&mode, 1);
}

void AudioI2c::setRGBBrightness(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    uint8_t reg = RGB_LED_BRIGHTNESS;
    writeBytes(_addr, reg, (uint8_t *)&brightness, 1);
}

void AudioI2c::setRGBLED(uint8_t num, uint32_t color)
{
    if (num > 2) num = 2;
    color       = ((color & 0xFF) << 16) | (color & 0xFF00) | ((color >> 16) & 0xFF);
    uint8_t reg = RGB_LED + num * 3;
    writeBytes(_addr, reg, (uint8_t *)&color, 3);
}

uint8_t AudioI2c::setI2CAddress(uint8_t newAddr)
{
    newAddr     = constrain(newAddr, I2C_ADDR_MIN, I2C_ADDR_MAX);
    uint8_t reg = I2C_ADDRESS;
    writeBytes(_addr, reg, (uint8_t *)&newAddr, 1);
    _addr = newAddr;
    delay(20);
    return _addr;
}

void AudioI2c::setFlashWriteBack()
{
    uint8_t data = 1;
    uint8_t reg  = FLASH_WRITE;
    writeBytes(_addr, reg, (uint8_t *)&data, 1);
    delay(20);
}

audio_mic_t AudioI2c::getMICStatus()
{
    uint8_t data;
    uint8_t reg = MICROPHONE_STATUS;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return (audio_mic_t)data;
}

audio_hpmode_t AudioI2c::getHPMode()
{
    uint8_t data;
    uint8_t reg = HEADPHONE_MODE;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return (audio_hpmode_t)data;
}

uint8_t AudioI2c::getHPInsertStatus()
{
    uint8_t data;
    uint8_t reg = HEADPHONE_INSERT_STATUS;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return data;
}

uint8_t AudioI2c::getRGBBrightness()
{
    uint8_t data;
    uint8_t reg = RGB_LED_BRIGHTNESS;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return data;
}

uint32_t AudioI2c::getRGBLED(uint8_t num)
{
    uint8_t rgb[3] = {0};
    uint8_t reg    = RGB_LED + num * 3;
    readBytes(_addr, reg, rgb, 3);
    return (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
}

uint8_t AudioI2c::getFirmwareVersion()
{
    uint8_t version;
    uint8_t reg = FIRMWARE_VERSION;
    readBytes(_addr, reg, (uint8_t *)&version, 1);
    return version;
}

uint8_t AudioI2c::getI2CAddress()
{
    uint8_t I2CAddress;
    uint8_t reg = I2C_ADDRESS;
    readBytes(_addr, reg, (uint8_t *)&I2CAddress, 1);
    return I2CAddress;
}