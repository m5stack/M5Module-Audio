/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef M5MODULE_AUDIO_H
#define M5MODULE_AUDIO_H

#include <Arduino.h>
#include <Wire.h>
#include <FS.h>
#include "es8388.hpp"
#include "M5Unified.h"

#ifdef ESP_IDF_VERSION
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0))
#define USE_NEW_I2S_API 1
#include <ESP_I2S.h>
#else
#define USE_NEW_I2S_API 0
#include "driver/i2s.h"
#endif
#else
#define USE_NEW_I2S_API 0
#endif

// #define AUDIO_I2C_DEBUG Serial
#if defined AUDIO_I2C_DEBUG
#define serialPrint(...)   AUDIO_I2C_DEBUG.print(__VA_ARGS__)
#define serialPrintln(...) AUDIO_I2C_DEBUG.println(__VA_ARGS__)
#define serialPrintf(...)  AUDIO_I2C_DEBUG.printf(__VA_ARGS__)
#define serialFlush()      AUDIO_I2C_DEBUG.flush()
#else
#endif

#define I2C_ADDR                (0x33)
#define I2C_ADDR_MIN            (0x08)
#define I2C_ADDR_MAX            (0x77)
#define MICROPHONE_STATUS       (0x00)
#define HEADPHONE_MODE          (0x10)
#define HEADPHONE_INSERT_STATUS (0x20)
#define RGB_LED_BRIGHTNESS      (0x30)
#define RGB_LED                 (0x40)
#define FLASH_WRITE             (0xF0)
#define FIRMWARE_VERSION        (0xFE)
#define I2C_ADDRESS             (0xFF)

/**
 * @enum audio_mic_t
 * @brief Microphone/LINE input configuration options
 * @details Corresponds to register 0x00 (R/W)
 *
 * @var AUDIO_MIC_CLOSE
 * Disable MIC/LINE input (value: 0)
 * @var AUDIO_MIC_OPEN
 * Enable MIC/LINE input (value: 1, default)
 */
typedef enum { AUDIO_MIC_CLOSE = 0, AUDIO_MIC_OPEN } audio_mic_t;

/**
 * @enum audio_hpmode_t
 * @brief Headphone output mode configuration options
 * @details Corresponds to register 0x10 (R/W)
 *
 * @var AUDIO_HPMODE_NATIONAL
 * National Standard audio mode (value: 0, default)
 * @var AUDIO_HPMODE_AMERICAN
 * American Standard audio mode (value: 1)
 */
typedef enum { AUDIO_HPMODE_NATIONAL = 0, AUDIO_HPMODE_AMERICAN } audio_hpmode_t;

/**
 * @brief Class to manage M5ModuleAudio operations including audio input/output and codec control.
 */
class M5ModuleAudio {
public:
#if USE_NEW_I2S_API
    M5ModuleAudio();
#else
    M5ModuleAudio(i2s_port_t i2s_num = I2S_NUM_0);
#endif
    ~M5ModuleAudio();

    /**
     * @brief Initializes the M5ModuleAudio with specific audio settings.
     *
     * @param sample_rate Sampling rate in Hz (defaults to 32000 Hz).
     * @param i2c_sda I2C data line pin number.
     * @param i2c_scl I2C clock line pin number.
     * @param i2s_di I2S data input pin.
     * @param i2s_ws I2S word select pin.
     * @param i2s_do I2S data output pin.
     * @param i2s_bck I2S bit clock pin.
     * @param wire Reference to a TwoWire object (defaults to Wire).
     * @return bool True if initialization was successful, false otherwise.
     */
    bool begin(TwoWire& wire, uint8_t sda, uint8_t scl, uint8_t addr, uint32_t speed, int sample_rate, int i2s_mck,
               int i2s_di, int i2s_ws, int i2s_do, int i2s_bck);

    bool begin(TwoWire& wire, uint8_t sda, uint8_t scl, uint8_t addr = I2C_ADDR, uint32_t speed = 400000);

    bool begin(TwoWire& wire, uint8_t addr = I2C_ADDR);

    /**
     * @brief Set microphone/LINE input status
     * @param status Audio input state (AUDIO_MIC_CLOSE/AUDIO_MIC_OPEN)
     * @note Controls register 0x00 (R/W)
     * @see audio_mic_t
     */
    void setMICStatus(audio_mic_t status);

    /**
     * @brief Set headphone output standard mode
     * @param mode Audio output standard (AUDIO_HPMODE_NATIONAL/AUDIO_HPMODE_AMERICAN)
     * @note Controls register 0x10 (R/W)
     * @see audio_hpmode_t
     */
    void setHPMode(audio_hpmode_t mode);

    /**
     * @brief Set global brightness for all RGB LEDs
     * @param brightness Brightness value (0-100)
     * @note Writes to register 0x30 (R/W), default:10
     */
    void setRGBBrightness(uint8_t brightness);

    /**
     * @brief Set color for specific RGB LED
     * @param num LED number (1-3)
     * @param color 24-bit color value (0xRRGGBB)
     * @note Writes to register 0x40 (R/W). Color components are automatically
     *       decomposed into 3 bytes (R, G, B)
     */
    void setRGBLED(uint8_t num, uint32_t color);

    /**
     * @brief Trigger configuration save to internal Flash
     * @note Writes 1 to register 0xF0 (Write-Only). Requires 20ms for Flash erase.
     * @warning Avoid frequent writes to extend Flash lifespan
     */
    void setFlashWriteBack();

    /**
     * @brief Set device I2C address
     * @param newAddr New I2C address (0x08-0x77)
     * @return Actual set address (may differ if invalid input)
     * @note Writes to register 0xFF (R/W). Requires reboot to take effect.
     */
    uint8_t setI2CAddress(uint8_t newAddr);

    /**
     * @brief Get current microphone/LINE input status
     * @return Current audio input state
     * @note Reads from register 0x00 (R/W)
     */
    audio_mic_t getMICStatus();

    /**
     * @brief Get current headphone output standard mode
     * @return Current audio output standard
     * @note Reads from register 0x10 (R/W)
     */
    audio_hpmode_t getHPMode();

    /**
     * @brief Get headphone insertion status
     * @return 0=Not inserted, 1=Inserted
     * @note Reads from register 0x20 (Read-Only)
     */
    uint8_t getHPInsertStatus();

    /**
     * @brief Get current RGB brightness value
     * @return Brightness level (0-100)
     * @note Reads from register 0x30 (R/W)
     */
    uint8_t getRGBBrightness();

    /**
     * @brief Get current color for specific RGB LED
     * @param num LED number (1-3)
     * @return 24-bit color value (0xRRGGBB)
     * @note Reads from register 0x40 (R/W). Combines 3 bytes (R, G, B) into color
     */
    uint32_t getRGBLED(uint8_t num);

    /**
     * @brief Get firmware version
     * @return 4-byte version code (e.g., 0x01020304 = v1.2.3.4)
     * @note Reads from register 0xFE (Read-Only)
     */
    uint8_t getFirmwareVersion();

    /**
     * @brief Get current I2C device address
     * @return Current I2C address
     * @note Reads from register 0xFF (R/W). Default:0x33
     */
    uint8_t getI2CAddress();

    /**
     * @brief Sets the speaker volume.
     *
     * @param volume Volume level (0-100).
     * @return bool True if volume was set successfully, false if the operation failed.
     */
    bool setSpeakerVolume(int volume);

    /**
     * @brief Sets the microphone gain.
     *
     * @param gain Microphone gain level.
     * @return bool True if gain was set successfully, false if the operation failed.
     */
    bool setMicGain(es_mic_gain_t gain);

    /**
     * @brief Selects the ES8388 ADC input source.
     *
     * @param input ADC input routing.
     * @return bool True if the source was set successfully, false otherwise.
     */
    bool setMicInputLine(es_adc_input_t input);

    /**
     * @brief Sets the microphone PGA gain.
     *
     * @param gain PGA gain value.
     * @return bool True if PGA gain was set successfully, false if the operation failed.
     */
    bool setMicPGAGain(es_mic_gain_t gain);

    /**
     * @brief Sets the microphone ADC volume.
     *
     * @param volume Volume level (0-100).
     * @return bool True if volume was set successfully, false if the operation failed.
     */
    bool setMicAdcVolume(uint8_t volume);

    /**
     * @brief Mutes or unmutes the speaker.
     *
     * @param mute Set true to mute, false to unmute.
     * @return bool True if the mute state was set successfully, false if the operation failed.
     */
    bool setMute(bool mute);

    /**
     * @brief Selects the ES8388 DAC output path.
     *
     * @param output Output routing mask.
     * @return bool True if the output path was set successfully, false otherwise.
     */
    bool setSpeakerOutput(es_dac_output_t output);

    /**
     * @brief Configures the ES8388 mixer input sources.
     *
     * @param lmixsel Left mixer source.
     * @param rmixsel Right mixer source.
     * @return bool True if the mixer was configured successfully, false otherwise.
     */
    bool setMixSourceSelect(es_mixsel_t lmixsel, es_mixsel_t rmixsel);

    /**
     * @brief Configures ES8388 sample width.
     *
     * @param mode Codec path to configure.
     * @param bits_len Sample bit width.
     * @return bool True if the setting was applied successfully, false otherwise.
     */
    bool setBitsSample(es_module_t mode, es_bits_length_t bits_len);

    /**
     * @brief Configures ES8388 sample rate.
     *
     * @param rate Target sample rate.
     * @return bool True if the setting was applied successfully, false otherwise.
     */
    bool setSampleRate(es_sample_rate_t rate);

    /**
     * @brief Reads back the configured ES8388 microphone gain.
     *
     * @return uint8_t Current microphone gain register value.
     */
    uint8_t getMicGain();

    /**
     * @brief Reads all ES8388 registers.
     *
     * @return uint8_t* Pointer to a 53-byte register snapshot, or nullptr if unavailable.
     */
    uint8_t* readAllReg();

    /**
     * @brief Calculates the buffer size needed for a recording of the specified duration.
     *
     * @param duration Recording duration in seconds.
     * @param sample_rate Sampling rate in Hz (defaults to 0).
     * @return int Buffer size in bytes required for the recording.
     */
    int getBufferSize(int duration, int sample_rate = 0);

    /**
     * @brief Determines the duration of a recording for a specified buffer size.
     *
     * @param size Buffer size in bytes.
     * @param sample_rate Sampling rate in Hz (defaults to 0).
     * @return int Duration in seconds of the recording.
     */
    int getDuration(int size, int sample_rate = 0);

    /**
     * @brief Records audio to a file.
     *
     * @param fs Reference to the filesystem to use.
     * @param filename Name of the file to save the recording to.
     * @param size Buffer size in bytes (should be pre-calculated using getBufferSize()).
     * @return bool True if the recording was successful, false if it failed.
     */
    bool record(FS& fs, const char* filename, int size);

    /**
     * @brief Records audio to a buffer.
     *
     * @param buffer Pointer to the buffer where audio data will be stored.
     * @param size Buffer size in bytes (should be pre-calculated using getBufferSize()).
     * @return bool True if the recording was successful, false if it failed.
     */
    bool record(uint8_t* buffer, int size);

    /**
     * @brief Plays audio from a file.
     *
     * @param fs Reference to the filesystem from which to play the audio.
     * @param filename Name of the file containing the audio to play.
     * @return bool True if playback was successful, false if it failed.
     */
    bool play(FS& fs, const char* filename);

    /**
     * @brief Plays audio from a buffer.
     *
     * @param buffer Pointer to the audio buffer.
     * @param size Size of the audio buffer in bytes.
     * @return bool True if playback was successful, false if it failed.
     */
    bool play(const uint8_t* buffer, int size);

private:
    TwoWire* _wire;
    uint8_t _sda, _scl;
    uint8_t _addr;

    ES8388* es8388;

    // I2S configuration
#if USE_NEW_I2S_API
    I2SClass I2S;
#else
    i2s_port_t i2s_num;
    i2s_config_t i2s_cfg;
    i2s_pin_config_t i2s_pin_cfg;
#endif
    bool _i2s_initialized = false;

    // I2S pin numbers
    int _i2s_mck = -1;
    int _i2s_bck = -1;
    int _i2s_ws  = -1;
    int _i2s_do  = -1;
    int _i2s_di  = -1;

    /**
     * @brief Writes multiple bytes to a specified register.
     *
     * This function writes a sequence of bytes from the provided buffer
     * to the device located at the specified I2C address and register.
     *
     * @param addr   The I2C address of the device.
     * @param reg    The register address where the data will be written.
     * @param buffer A pointer to the data buffer that contains the bytes to be written.
     * @param length The number of bytes to write from the buffer.
     */
    void writeBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);

    /**
     * @brief Reads multiple bytes from a specified register.
     *
     * This function reads a sequence of bytes from the device located at
     * the specified I2C address and register into the provided buffer.
     *
     * @param addr   The I2C address of the device.
     * @param reg    The register address from which the data will be read.
     * @param buffer A pointer to the data buffer where the read bytes will be stored.
     * @param length The number of bytes to read into the buffer.
     */
    void readBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);

    // Initializes the ES8388 codec
    bool es8388_codec_init();

    // Initializes the I2S driver
    bool i2s_driver_init(int sample_rate);
};

#endif  // M5ModuleAudio_H
