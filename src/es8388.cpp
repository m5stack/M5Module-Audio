/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "es8388.hpp"

ES8388::ES8388(TwoWire* wire, uint8_t sda, uint8_t scl, uint32_t speed)
{
    _wire  = wire;
    _sda   = sda;
    _scl   = scl;
    _speed = speed;
    _wire->begin(_sda, _scl, _speed);
}

bool ES8388::writeBytes(uint8_t reg, uint8_t data)
{
    _wire->beginTransmission(ES8388_ADDR);
    _wire->write(reg);
    _wire->write(data);
    return _wire->endTransmission() == 0;
#if ES8388_DEBUG
    Serial.print("Write bytes: [");
    Serial.print(ES8388_ADDR);
    Serial.print(", ");
    Serial.print(reg);
    Serial.print(", ");
    Serial.print(data);
    Serial.print(", ");
#else
#endif
}

bool ES8388::readBytes(uint8_t reg, uint8_t& data)
{
    bool ret = false;
    _wire->beginTransmission(ES8388_ADDR);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom((uint16_t)ES8388_ADDR, (uint8_t)1);
    if (_wire->available() >= 1) {
        data = _wire->read();
        ret  = true;
    }
    return ret;
#if AUDIO_I2C_DEBUG
    Serial.print("Read bytes: [");
    Serial.print(ES8388_ADDR);
    Serial.print(", ");
    Serial.print(reg);
    Serial.print(", ");
    Serial.print(data);
    Serial.print(", ");
#else
#endif
}

uint8_t* ES8388::readAllReg()
{
    static uint8_t reg[53];
    for (uint8_t i = 0; i < 53; i++) {
        readBytes(i, reg[i]);
    }
    return reg;
}

bool ES8388::init()
{
    bool res = true;
    /* INITIALIZATION (BASED ON ES8388 USER GUIDE EXAMPLE) */
    // Set Chip to Slave
    res &= writeBytes(ES8388_MASTERMODE, 0x00);
    // Power down DEM and STM
    res &= writeBytes(ES8388_CHIPPOWER, 0xFF);
    // Set same LRCK	Set same LRCK
    res &= writeBytes(ES8388_DACCONTROL21, 0x80);
    // Set Chip to Play&Record Mode
    res &= writeBytes(ES8388_CONTROL1, 0x05);
    // Power Up Analog and Ibias
    res &= writeBytes(ES8388_CONTROL2, 0x40);

    /* ADC setting */
    // Micbias for Record
    res &= writeBytes(ES8388_ADCPOWER, 0x00);
    // Enable Lin1/Rin1 (0x00 0x00) for Lin2/Rin2 (0x50 0x80)
    res &= writeBytes(ES8388_ADCCONTROL2, 0x00);
    res &= writeBytes(ES8388_ADCCONTROL3, 0x00);
    // PGA gain (0x88 - 24db) (0x77 - 21db)
    res &= writeBytes(ES8388_ADCCONTROL1, 0x88);
    // SFI setting (i2s mode/16 bit)
    res &= writeBytes(ES8388_ADCCONTROL4, 0x2C);
    // ADC MCLK/LCRK ratio (256)
    res &= writeBytes(ES8388_ADCCONTROL5, 0x02);
    // set ADC digital volume
    res &= writeBytes(ES8388_ADCCONTROL7, 0x28);
    res &= writeBytes(ES8388_ADCCONTROL8, 0x00);
    res &= writeBytes(ES8388_ADCCONTROL9, 0x00);
    // recommended ALC setting for VOICE refer to ES8388 MANUAL
    res &= writeBytes(ES8388_ADCCONTROL10, 0xeA);
    res &= writeBytes(ES8388_ADCCONTROL11, 0xC0);
    res &= writeBytes(ES8388_ADCCONTROL12, 0x12);
    res &= writeBytes(ES8388_ADCCONTROL13, 0x06);
    res &= writeBytes(ES8388_ADCCONTROL14, 0xC3);

    /* DAC setting */
    // Power Up DAC& enable Lout/Rout
    res &= writeBytes(ES8388_DACPOWER, 0x3F);
    // SFI setting (i2s mode/16 bit)
    res &= writeBytes(ES8388_DACCONTROL1, 0x18);
    // DAC MCLK/LCRK ratio (256)
    res &= writeBytes(ES8388_DACCONTROL2, 0x02);
    // unmute codec
    res &= writeBytes(ES8388_DACCONTROL3, 0x00);
    // set DAC digital volume
    res &= writeBytes(ES8388_DACCONTROL4, 0x05);
    res &= writeBytes(ES8388_DACCONTROL5, 0x05);
    // Setup Mixer
    // (reg[16] 1B mic Amp, 0x09 direct;[reg 17-20] 0x90 DAC, 0x50 Mic Amp)
    res &= writeBytes(ES8388_DACCONTROL16, 0x00);
    res &= writeBytes(ES8388_DACCONTROL17, 0xd0);
    res &= writeBytes(ES8388_DACCONTROL18, 0x38);  //??
    res &= writeBytes(ES8388_DACCONTROL19, 0x38);  //??
    res &= writeBytes(ES8388_DACCONTROL20, 0xd0);
    res &= writeBytes(ES8388_DACCONTROL21, 0x80);
    // set Lout/Rout Volume -45db
    res &= writeBytes(ES8388_DACCONTROL24, 0x12);
    res &= writeBytes(ES8388_DACCONTROL25, 0x12);
    res &= writeBytes(ES8388_DACCONTROL26, 0x00);
    res &= writeBytes(ES8388_DACCONTROL27, 0x00);

    /* Power up DEM and STM */
    res &= writeBytes(ES8388_CHIPPOWER, 0x00);
    /* set up MCLK) */
    return res;
}

bool ES8388::setMicGain(es_mic_gain_t gain)
{
    bool res             = true;
    uint8_t controlValue = (gain << 4) | gain;
    res &= writeBytes(ES8388_ADCCONTROL1, controlValue);
    return res;
}

uint8_t ES8388::getMicGain()
{
    static uint8_t data;
    readBytes(ES8388_ADCCONTROL1, data);
    data = data & 0x0f;
    return data;
}

bool ES8388::setADCInput(es_adc_input_t input)
{
    bool res    = true;
    uint8_t reg = 0;
    res         = readBytes(ES8388_ADCCONTROL2, reg);
    reg         = reg & 0x0f;
    res &= writeBytes(ES8388_ADCCONTROL2, reg | input);
    if (input == ADC_INPUT_LINPUT2_RINPUT2) {
        res &= writeBytes(ES8388_ADCCONTROL3, 0x80);
    }
    return res;
}
bool ES8388::setADCVolume(uint8_t volume)
{
    bool res = true;
    if (volume > 100) volume = 100;
    uint16_t steps = static_cast<uint16_t>(((100 - volume) * 192) / 100);
    uint8_t data   = static_cast<uint8_t>(steps);
    res &= writeBytes(ES8388_ADCCONTROL8, data);
    res &= writeBytes(ES8388_ADCCONTROL9, data);
    return res;
}

bool ES8388::setDACVolume(uint8_t volume)
{
    bool res      = true;
    volume        = (volume > 100) ? 100 : volume;
    uint8_t steps = static_cast<uint8_t>((static_cast<uint16_t>(volume) * 33 + 50) / 100);
    steps         = (steps > 0x21) ? 0x21 : steps;
    res &= writeBytes(ES8388_DACCONTROL24, steps);
    res &= writeBytes(ES8388_DACCONTROL25, steps);
    return res;
}

bool ES8388::setDACOutput(es_dac_output_t output)
{
    bool res = true;
    res &= writeBytes(ES8388_DACPOWER, output);
    return res;
}

// mute Output
bool ES8388::setDACmute(bool mute)
{
    uint8_t _reg;
    readBytes(ES8388_ADCCONTROL1, _reg);
    bool res = true;
    if (mute)
        res &= writeBytes(ES8388_DACCONTROL3, _reg | 0x02);
    else
        res &= writeBytes(ES8388_DACCONTROL3, _reg & ~(0x02));
    return res;
}

bool ES8388::setMixSourceSelect(es_mixsel_t lmixsel, es_mixsel_t rmixsel)
{
    bool res          = true;
    uint8_t left_bits = 0, right_bits = 0;

    switch (lmixsel) {
        case MIXLIN1:
            left_bits = 0b000;
            break;
        case MIXLIN2:
            left_bits = 0b001;
            break;
        case MIXADC:
            left_bits = 0b011;
            break;
        case MIXRES:
            return false;
        default:
            return false;
    }

    switch (rmixsel) {
        case MIXLIN1:
            right_bits = 0b000;
            break;
        case MIXLIN2:
            right_bits = 0b001;
            break;
        case MIXADC:
            right_bits = 0b011;
            break;
        case MIXRES:
            return false;
        default:
            return false;
    }

    uint8_t data = (left_bits << 3) | (right_bits << 0);
    res &= writeBytes(ES8388_DACCONTROL16, data);
    return res;
}

bool ES8388::setBitsSample(es_module_t mode, es_bits_length_t bits_len)
{
    bool res    = true;
    uint8_t reg = 0;
    int bits    = (int)bits_len;

    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        res = readBytes(ES8388_ADCCONTROL4, reg);
        reg = reg & 0xe3;
        res &= writeBytes(ES8388_ADCCONTROL4, reg | (bits << 2));
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        res = readBytes(ES8388_DACCONTROL1, reg);
        reg = reg & 0xc7;
        res &= writeBytes(ES8388_DACCONTROL1, reg | (bits << 3));
    }
    return res;
}

bool ES8388::setSampleRate(es_sample_rate_t rate)
{
    bool res           = true;
    uint8_t masterMode = 0x00;
    uint8_t adcFsRatio = 0x02;
    uint8_t dacFsRatio = 0x02;
    uint8_t mclkDiv    = 0x00;

    switch (rate) {
        case SAMPLE_RATE_8K:    // MCLK=12.288MHz
            adcFsRatio = 0x0A;  // 01010 (MCLK/1536)
            dacFsRatio = 0x0A;
            break;
        case SAMPLE_RATE_11K:   // MCLK=11.2896MHz
            adcFsRatio = 0x07;  // 00111 (MCLK/1024)
            dacFsRatio = 0x07;
            break;
        case SAMPLE_RATE_16K:   // MCLK=12.288MHz
            adcFsRatio = 0x06;  // 00110 (MCLK/768)
            dacFsRatio = 0x06;
            break;
        case SAMPLE_RATE_24K:   // MCLK=12.288MHz
            adcFsRatio = 0x04;  // 00100 (MCLK/512)
            dacFsRatio = 0x04;
            break;
        case SAMPLE_RATE_32K:   // MCLK=12.288MHz
            adcFsRatio = 0x03;  // 00011 (MCLK/384)
            dacFsRatio = 0x03;
            break;
        case SAMPLE_RATE_44K:   // MCLK=11.2896MHz
            adcFsRatio = 0x02;  // 00010 (MCLK/256)
            dacFsRatio = 0x02;
            mclkDiv    = 0x40;
            break;
        case SAMPLE_RATE_48K:
            adcFsRatio = 0x02;  // 00010 (MCLK/256)
            dacFsRatio = 0x02;
            break;
        default:
            return false;
    }
    res &= writeBytes(ES8388_MASTERMODE, masterMode | mclkDiv);
    res &= writeBytes(ES8388_ADCCONTROL5, adcFsRatio & 0x1F);
    res &= writeBytes(ES8388_DACCONTROL2, dacFsRatio & 0x1F);
    return res;
}