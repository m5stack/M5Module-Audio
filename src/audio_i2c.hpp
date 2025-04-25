/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef __AUDIO_I2C_HPP
#define __AUDIO_I2C_HPP

#include "Arduino.h"
#include "Wire.h"

// #define AUDIO_I2C_DEBUG Serial  // This macro definition can be annotated without sending and receiving data prints
//         Define the serial port you want to use, e.g., Serial1 or Serial2
#if defined AUDIO_I2C_DEBUG
#define serialPrint(...)   AUDIO_I2C_DEBUG.print(__VA_ARGS__)
#define serialPrintln(...) AUDIO_I2C_DEBUG.println(__VA_ARGS__)
#define serialPrintf(...)  AUDIO_I2C_DEBUG.printf(__VA_ARGS__)
#define serialFlush()      AUDIO_I2C_DEBUG.flush()
#else
#endif

/**
 * @brief I2C Address Indicates the default address.
 */
#define I2C_ADDR (0x33)

/**
 * @brief Minimum valid I2C address.
 */
#define I2C_ADDR_MIN (0x08)

/**
 * @brief Maximum valid I2C address.
 */
#define I2C_ADDR_MAX (0x77)

/**
 * @brief Microphone/LINE Input Configuration Register (R/W)
 */
#define MICROPHONE_STATUS (0x00)

/**
 * @brief Headphone Mode Configuration Register (R/W)
 */
#define HEADPHONE_MODE (0x10)

/**
 * @brief Headphone Insertion Detection Register (Read-Only)
 */
#define HEADPHONE_INSERT_STATUS (0x20)

/**S
 * @brief RGB LED Brightness Control Register (R/W)
 */
#define RGB_LED_BRIGHTNESS (0x30)

/**
 * @brief RGB LED Color Control Register (R/W)
 */
#define RGB_LED (0x40)

/**
 * @brief Configuration Save Register (Write-Only)
 */
#define FLASH_WRITE (0xF0)

/**
 * @brief Firmware Version Register (Read-Only)
 */
#define FIRMWARE_VERSION (0xFE)

/**
 * @brief I2C Device Address Configuration Register (R/W)
 */
#define I2C_ADDRESS (0xFF)

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

class AudioI2c {
public:
    /**
     * @brief Initializes the I2C communication with specified parameters.
     *
     * This function sets up the I2C bus using the provided SDA and SCL pins,
     * as well as the desired communication speed. It can also specify a default
     * I2C address for the device.
     *
     * @param wire Pointer to a TwoWire object that represents the I2C bus.
     * @param sda The pin number for the SDA line (default is -1).
     * @param scl The pin number for the SCL line (default is -1).
     * @param speed The I2C clock speed in Hertz (default is 400000L).
     * @param addr The I2C address of the device (default is I2C_ADDR).
     *
     * @return True if the initialization was successful, false otherwise.
     */
    bool begin(TwoWire* wire, uint8_t sda = -1, uint8_t scl = -1, uint32_t speed = 400000L, uint8_t addr = I2C_ADDR);

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

private:
    TwoWire* _wire = &Wire;
    uint8_t _addr;
    uint8_t _scl;
    uint8_t _sda;
    uint32_t _speed;

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
};
#endif