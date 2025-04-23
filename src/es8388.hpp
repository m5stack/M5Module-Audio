/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef __ES8388_HPP
#define __ES8388_HPP

#include <Arduino.h>
#include <Wire.h>

// #define ES8388_DEBUG Serial  // This macro definition can be annotated without sending and receiving data prints
//         Define the serial port you want to use, e.g., Serial1 or Serial2
#if defined ES8388_DEBUG
#define serialPrint(...)   ES8388_DEBUG.print(__VA_ARGS__)
#define serialPrintln(...) ES8388_DEBUG.println(__VA_ARGS__)
#define serialPrintf(...)  ES8388_DEBUG.printf(__VA_ARGS__)
#define serialFlush()      ES8388_DEBUG.flush()
#else
#endif

/**
 * @file ES8388_registers.h
 * @brief Definitions for ES8388 audio codec registers
 */

/**
 *  @brief I2C device address for ES8388
 */
#define ES8388_ADDR (0x10)

/* ES8388 control registers */
/**
 * @brief Control register 1 - System control and clock settings
 */
#define ES8388_CONTROL1 (0x00)

/**
 * @brief Control register 2 - Interface format and clock mode
 */
#define ES8388_CONTROL2 (0x01)

/**
 * @brief Chip power management register
 */
#define ES8388_CHIPPOWER (0x02)

/**
 * @brief ADC power control register
 */
#define ES8388_ADCPOWER (0x03)

/**
 * @brief DAC power control register
 */
#define ES8388_DACPOWER (0x04)

/**
 * @brief Low power control register 1
 */
#define ES8388_CHIPLOPOW1 (0x05)

/**
 * @brief Low power control register 2
 */
#define ES8388_CHIPLOPOW2 (0x06)

/**
 * @brief Analog volume management control
 */
#define ES8388_ANAVOLMANAG (0x07)

/**
 * @brief Master/slave mode control
 */
#define ES8388_MASTERMODE (0x08)

/* ADC registers */
/**
 * @brief ADC control 1 - Input selection and PGA gain
 */
#define ES8388_ADCCONTROL1 (0x09)

/**
 @brief ADC control 2 - Digital mic settings
 */
#define ES8388_ADCCONTROL2 (0x0a)

/**
 * @brief ADC control 3 - High-pass filter and ADC mode
 */
#define ES8388_ADCCONTROL3 (0x0b)

/**
 * @brief ADC control 4 - Sample rate and clock divider
 */
#define ES8388_ADCCONTROL4 (0x0c)

/**
 * @brief ADC control 5 - Digital volume control
 */
#define ES8388_ADCCONTROL5 (0x0d)

/**
 * @brief ADC control 6 - ALC settings 1
 */
#define ES8388_ADCCONTROL6 (0x0e)

/**
 * @brief ADC control 7 - ALC settings 2
 */
#define ES8388_ADCCONTROL7 (0x0f)

/**
 * @brief ADC control 8 - ALC settings 3
 */
#define ES8388_ADCCONTROL8 (0x10)

/**
 * @brief ADC control 9 - Noise gate control
 */
#define ES8388_ADCCONTROL9 (0x11)

/**
 * @brief ADC control 10 - Zero detection
 */
#define ES8388_ADCCONTROL10 (0x12)

/**
 * @brief ADC control 11 - ALC enable and mode
 */
#define ES8388_ADCCONTROL11 (0x13)

/**
 * @brief ADC control 12 - Left channel volume
 */
#define ES8388_ADCCONTROL12 (0x14)

/**
 * @brief ADC control 13 - Right channel volume
 */
#define ES8388_ADCCONTROL13 (0x15)

/**
 * @brief ADC control 14 - Analog PGA control
 */
#define ES8388_ADCCONTROL14 (0x16)

/* DAC registers */
/**
 * @brief DAC control 1 - Soft ramp and volume control
 */
#define ES8388_DACCONTROL1 (0x17)

/**
 * @brief DAC control 2 - DAC interface format
 */
#define ES8388_DACCONTROL2 (0x18)

/**
 * @brief DAC control 3 - De-emphasis and mute control
 */
#define ES8388_DACCONTROL3 (0x19)

/**
 * @brief DAC control 4 - Sample rate and clock divider
 */
#define ES8388_DACCONTROL4 (0x1a)

/**
 * @brief DAC control 5 - Digital volume left
 */
#define ES8388_DACCONTROL5 (0x1b)

/**
 * @brief DAC control 6 - Digital volume right
 */
#define ES8388_DACCONTROL6 (0x1c)

/**
 * @brief DAC control 7 - Volume mixer control
 */
#define ES8388_DACCONTROL7 (0x1d)

/**
 * @brief DAC control 8 - Mixer settings
 */
#define ES8388_DACCONTROL8 (0x1e)

/**
 * @brief DAC control 9 - Channel swap and mute
 */
#define ES8388_DACCONTROL9 (0x1f)

/**
 * @brief DAC control 10 - Zero cross detection
 */
#define ES8388_DACCONTROL10 (0x20)

/**
 * @brief DAC control 11 - Volume ramp rate
 */
#define ES8388_DACCONTROL11 (0x21)

/**
 * @brief DAC control 12 - Power management
 */
#define ES8388_DACCONTROL12 (0x22)

/**
 * @brief DAC control 13 - Analog output control
 */
#define ES8388_DACCONTROL13 (0x23)

/**
 * @brief DAC control 14 - Output phase control
 */
#define ES8388_DACCONTROL14 (0x24)

/**
 * @brief DAC control 15 - Output level control
 */
#define ES8388_DACCONTROL15 (0x25)

/**
 * @brief DAC control 16 - Mixer volume left
 */
#define ES8388_DACCONTROL16 (0x26)

/**
 * @brief DAC control 17 - Mixer volume right
 */
#define ES8388_DACCONTROL17 (0x27)

/**
 * @brief DAC control 18 - ALC control 1
 */
#define ES8388_DACCONTROL18 (0x28)

/**
 * @brief DAC control 19 - ALC control 2
 */
#define ES8388_DACCONTROL19 (0x29)

/**
 * @brief DAC control 20 - ALC control 3
 */
#define ES8388_DACCONTROL20 (0x2a)

/**
 * @brief DAC control 21 - ALC control 4
 */
#define ES8388_DACCONTROL21 (0x2b)

/**
 * @brief DAC control 22 - Noise gate control
 */
#define ES8388_DACCONTROL22 (0x2c)

/**
 * @brief DAC control 23 - Zero detection
 */
#define ES8388_DACCONTROL23 (0x2d)

/**
 * @brief DAC control 24 - Output mute control
 * */
#define ES8388_DACCONTROL24 (0x2e)

/**
 * @brief DAC control 25 - Volume bypass control
 */
#define ES8388_DACCONTROL25 (0x2f)

/**
 * @brief DAC control 26 - Output phase control
 */
#define ES8388_DACCONTROL26 (0x30)

/**
 * @brief DAC control 27 - Volume fade control
 */
#define ES8388_DACCONTROL27 (0x31)

/**
 * @brief DAC control 28 - Power on delay control
 */
#define ES8388_DACCONTROL28 (0x32)

/**
 * @brief DAC control 29 - Output driver control
 */
#define ES8388_DACCONTROL29 (0x33)

/**
 * @brief DAC control 30 - Reserved/test mode
 */
#define ES8388_DACCONTROL30 (0x34)

/**
 * @brief Mixer input source selection
 * @note Mixer configuration affects audio path routing
 */
typedef enum {
    MIXLIN1, /**< Direct line input 1 (unprocessed signal path) */
    MIXLIN2, /**< Direct line input 2 (alternative signal path) */
    MIXRES,  /**< Reserved mixer source (do not use in normal operation) */
    MIXADC   /**< Processed signal from ADC/ALC (with automatic level control) */
} es_mixsel_t;

/**
 * @brief ES8388 functional module selection
 */
typedef enum {
    ES_MODULE_MIN     = -1,   /**< Minimum module selector (validation flag) */
    ES_MODULE_ADC     = 0x01, /**< Enable ADC module */
    ES_MODULE_DAC     = 0x02, /**< Enable DAC module */
    ES_MODULE_ADC_DAC = 0x03, /**< Enable both ADC and DAC modules */
    ES_MODULE_LINE    = 0x04, /**< Line-in/out interface control */
    ES_MODULE_MAX             /**< Maximum module selector (validation flag) */
} es_module_t;

/**
 * @brief Audio data bit length configuration
 * @note Actual supported resolution depends on hardware capabilities
 */
typedef enum {
    BIT_LENGTH_MIN    = -1,   /**< Minimum bit length flag (validation) */
    BIT_LENGTH_16BITS = 0x03, /**< 16-bit word length */
    BIT_LENGTH_18BITS = 0x02, /**< 18-bit word length */
    BIT_LENGTH_20BITS = 0x01, /**< 20-bit word length */
    BIT_LENGTH_24BITS = 0x00, /**< 24-bit word length (typical default) */
    BIT_LENGTH_32BITS = 0x04, /**< 32-bit word length */
    BIT_LENGTH_MAX,           /**< Maximum bit length flag (validation) */
} es_bits_length_t;

/**
 * @brief ADC input channel selection
 * @note Configuration affects both left and right channels
 */
typedef enum {
    ADC_INPUT_LINPUT1_RINPUT1 = 0x00, /**< Use input 1 for both channels */
    ADC_INPUT_LINPUT2_RINPUT2 = 0x10, /**< Use input 2 for both channels */
    ADC_INPUT_DIFFERENCE1     = 0xf0, /**< Differential input configuration */
} es_adc_input_t;

/**
 * @brief DAC output channel selection
 * @note Multiple outputs can be combined using bitwise OR
 */
typedef enum {
    DAC_OUTPUT_OUT1 = 0x30, /**< Enable output channel 1 */
    DAC_OUTPUT_OUT2 = 0x0C, /**< Enable output channel 2 */
    DAC_OUTPUT_ALL  = 0x3c, /**< Enable all output channels */
} es_dac_output_t;

/**
 * @brief Microphone gain settings
 * @note Values represent gain steps in 3dB increments
 */
typedef enum {
    MIC_GAIN_0DB = 0, /**< 0dB microphone gain */
    MIC_GAIN_3DB,     /**< 3dB microphone gain */
    MIC_GAIN_6DB,     /**< 6dB microphone gain */
    MIC_GAIN_9DB,     /**< 9dB microphone gain */
    MIC_GAIN_12DB,    /**< 12dB microphone gain */
    MIC_GAIN_15DB,    /**< 15dB microphone gain */
    MIC_GAIN_18DB,    /**< 18dB microphone gain */
    MIC_GAIN_21DB,    /**< 21dB microphone gain */
    MIC_GAIN_24DB,    /**< 24dB microphone gain (maximum) */
} es_mic_gain_t;

typedef enum {
    SAMPLE_RATE_8K = 0, /**< 8000 Hz sampling rate */
    SAMPLE_RATE_11K,    /**< 11025 Hz sampling rate */
    SAMPLE_RATE_16K,    /**< 16000 Hz sampling rate */
    SAMPLE_RATE_24K,    /**< 24000 Hz sampling rate */
    SAMPLE_RATE_32K,    /**< 32000 Hz sampling rate */
    SAMPLE_RATE_44K,    /**< 44100 Hz sampling rate (CD quality) */
    SAMPLE_RATE_48K     /**< 48000 Hz sampling rate (professional audio) */
} es_sample_rate_t;

class ES8388 {
public:
    /**
     * @brief Constructs ES8388 codec controller object
     * @param wire I2C bus pointer (default Wire)
     * @param sda SDA pin number (default -1)
     * @param scl SCL pin number (default -1)
     * @param speed I2C clock speed in Hz (default 400kHz)
     */
    ES8388(TwoWire* wire, uint8_t sda = -1, uint8_t scl = -1, uint32_t speed = 400000L);

    /**
     * @brief Reads all codec registers into a buffer
     * @return Pointer to 53-byte array containing register values (0x00-0x34)
     * @warning Caller must manage memory allocation/deallocation
     */
    uint8_t* readAllReg();

    /**
     * @brief Initializes codec with default configuration
     * @return true if initialization succeeded, false on communication failure
     * @note Performs hardware reset and configures basic audio paths
     */
    bool init();

    /**
     * @brief Sets microphone amplifier gain
     * @param gain Microphone gain level (0-24dB in 3dB steps)
     * @return true if write succeeded
     * @see es_mic_gain_t
     */
    bool setMicGain(es_mic_gain_t gain);

    /**
     * @brief Selects ADC input source
     * @param input Analog input channel configuration
     * @return true if register write succeeded
     * @see es_adc_input_t
     */
    bool setADCInput(es_adc_input_t input);

    /**
     * @brief Sets ADC digital volume
     * @param volume Volume level (0-100% mapped to 0-255 register value)
     * @return true if value was within valid range and write succeeded
     */
    bool setADCVolume(uint8_t volume);

    /**
     * @brief Configures DAC output channels
     * @param output Output channel combination
     * @return true if configuration succeeded
     * @see es_dac_output_t
     */
    bool setDACOutput(es_dac_output_t output);

    /**
     * @brief Sets DAC output volume
     * @param volume Volume level (0-100% mapped to 0-255 register value)
     * @return true if value was within valid range and write succeeded
     */
    bool setDACVolume(uint8_t volume);

    /**
     * @brief Controls DAC mute function
     * @param mute true to enable mute, false to disable
     * @return true if control command succeeded
     */
    bool setDACmute(bool mute);

    /**
     * @brief Configures mixer input sources
     * @param lmixsel Left channel mixer source
     * @param rmixsel Right channel mixer source
     * @return true if both channel configurations succeeded
     * @see es_mixsel_t
     */
    bool setMixSourceSelect(es_mixsel_t lmixsel, es_mixsel_t rmixsel);

    /**
     * @brief Configures audio sample format
     * @param mode Audio module (ADC/DAC)
     * @param len Bit length configuration
     * @return true if both module and bit length were valid
     * @see es_module_t, es_bits_length_t
     */
    bool setBitsSample(es_module_t mode, es_bits_length_t len);

    /**
     * @brief Sets audio sampling rate
     * @param rate Sample rate enumeration value
     * @return true if rate was supported and configuration succeeded
     * @note Actual rate depends on master clock configuration
     * @see es_sample_rate_t
     */
    bool setSampleRate(es_sample_rate_t rate);

    /**
     * @brief Retrieves current microphone gain setting
     * @return Current mic gain as es_mic_gain_t enumeration value
     * @note Returns stored value, not read from hardware
     */
    uint8_t getMicGain();

private:
    TwoWire* _wire;
    uint8_t _scl;
    uint8_t _sda;
    uint32_t _speed;

    /**
     * @brief Write a single byte to the ES8388 register via I2C
     * @param reg Target register address (0x00-0x34 per ES8388 spec)
     * @param data 8-bit value to write to the register
     * @return true if I2C transmission succeeded, false on communication failure
     * @note Uses the predefined ES8388_ADDR (0x10) for device addressing
     */
    bool writeBytes(uint8_t reg, uint8_t data);

    /**
     * @brief Read a single byte from the ES8388 register via I2C
     * @param reg Source register address (0x00-0x34 per ES8388 spec)
     * @param[out] data Reference to store the read 8-bit value
     * @return true if I2C transmission succeeded, false on communication failure
     * @note Implements standard I2C read sequence: write register address then read data
     */
    bool readBytes(uint8_t reg, uint8_t& data);
};

#endif