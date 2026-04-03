/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "M5Module_Audio.h"

#define TAG "EchoBase"

namespace {

uint32_t sample_rate_to_hz(es_sample_rate_t rate)
{
    switch (rate) {
        case SAMPLE_RATE_8K:
            return 8000;
        case SAMPLE_RATE_11K:
            return 11025;
        case SAMPLE_RATE_16K:
            return 16000;
        case SAMPLE_RATE_24K:
            return 24000;
        case SAMPLE_RATE_32K:
            return 32000;
        case SAMPLE_RATE_44K:
            return 44100;
        case SAMPLE_RATE_48K:
            return 48000;
        default:
            return 0;
    }
}

}  // namespace

#if USE_NEW_I2S_API
M5ModuleAudio::M5ModuleAudio() : _wire(nullptr), _sda(0), _scl(0), _addr(I2C_ADDR), es8388(nullptr)
{
}
#else
M5ModuleAudio::M5ModuleAudio(i2s_port_t i2s_num)
    : _wire(nullptr), _sda(0), _scl(0), _addr(I2C_ADDR), es8388(nullptr), i2s_num(i2s_num)
{
}
#endif

M5ModuleAudio::~M5ModuleAudio()
{
    if (es8388 != nullptr) {
        delete es8388;
        es8388 = nullptr;
    }
}

bool M5ModuleAudio::begin(TwoWire &wire, uint8_t addr)
{
    uint8_t sda = M5.getPin(m5::pin_name_t::in_i2c_sda);
    uint8_t scl = M5.getPin(m5::pin_name_t::in_i2c_scl);
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    uint8_t bck = M5.getPin(m5::pin_name_t::mbus_pin24);  // SCLK
    uint8_t mck = M5.getPin(m5::pin_name_t::mbus_pin22);  // MCLK
#else
    uint8_t bck = M5.getPin(m5::pin_name_t::mbus_pin22);  // SCLK
    uint8_t mck = M5.getPin(m5::pin_name_t::mbus_pin24);  // MCLK
#endif
    uint8_t di  = M5.getPin(m5::pin_name_t::mbus_pin26);
    uint8_t ws  = M5.getPin(m5::pin_name_t::mbus_pin21);  // LRCK
    uint8_t do_ = M5.getPin(m5::pin_name_t::mbus_pin23);
    printf("I2C SDA: %d, SCL: %d\n", sda, scl);
    printf("I2S MCK: %d, DI: %d, WS: %d, DO: %d, BCK: %d\n", mck, di, ws, do_, bck);
    return begin(wire, sda, scl, addr, 400000, 44100, mck, di, ws, do_, bck);
}

bool M5ModuleAudio::begin(TwoWire &wire, uint8_t sda, uint8_t scl, uint8_t addr, uint32_t speed)
{
    _i2s_initialized = false;
    _wire            = &wire;
    _sda             = sda;
    _scl             = scl;
    _addr            = addr;

    ESP_LOGI(TAG, "I2C initialized");

    _wire->begin(_sda, _scl, speed);
    delay(10);
    _wire->beginTransmission(addr);
    uint8_t error = _wire->endTransmission();
    if (error != 0) {
        return false;
    }

    if (!es8388_codec_init()) {
        ESP_LOGI(TAG, "ES8388 codec initialization failed");
        return false;
    }

    return setMicGain(MIC_GAIN_0DB);
}

bool M5ModuleAudio::begin(TwoWire &wire, uint8_t sda, uint8_t scl, uint8_t addr, uint32_t speed, int sample_rate,
                          int i2s_mck, int i2s_di, int i2s_ws, int i2s_do, int i2s_bck)
{
    _wire = &wire;
    _sda  = sda;
    _scl  = scl;
    _addr = addr;

    // Store IO pins
    _i2s_mck = i2s_mck;
    _i2s_bck = i2s_bck;
    _i2s_ws  = i2s_ws;
    _i2s_do  = i2s_do;
    _i2s_di  = i2s_di;

    // Initialize I2C
    ESP_LOGI(TAG, "I2C initialized");

    _wire->begin(_sda, _scl, speed);
    delay(10);
    _wire->beginTransmission(addr);
    uint8_t error = _wire->endTransmission();
    if (error != 0) {
        return false;
    }

    // Initialize I2S driver
    if (!i2s_driver_init(sample_rate)) {
        ESP_LOGI(TAG, "I2S driver initialization failed");
        return false;
    }

    // Initialize ES8388 codec
    if (!es8388_codec_init()) {
        ESP_LOGI(TAG, "ES8388 codec initialization failed");
        return false;
    }

    setMicGain(MIC_GAIN_0DB);

    return true;
}

void M5ModuleAudio::setMICStatus(audio_mic_t status)
{
    uint8_t reg = MICROPHONE_STATUS;
    writeBytes(_addr, reg, (uint8_t *)&status, 1);
}

void M5ModuleAudio::setHPMode(audio_hpmode_t mode)
{
    uint8_t reg = HEADPHONE_MODE;
    writeBytes(_addr, reg, (uint8_t *)&mode, 1);
}

void M5ModuleAudio::setRGBBrightness(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    uint8_t reg = RGB_LED_BRIGHTNESS;
    writeBytes(_addr, reg, (uint8_t *)&brightness, 1);
}

void M5ModuleAudio::setRGBLED(uint8_t num, uint32_t color)
{
    if (num > 2) num = 2;
    color       = ((color & 0xFF) << 16) | (color & 0xFF00) | ((color >> 16) & 0xFF);
    uint8_t reg = RGB_LED + num * 3;
    writeBytes(_addr, reg, (uint8_t *)&color, 3);
}

uint8_t M5ModuleAudio::setI2CAddress(uint8_t newAddr)
{
    newAddr     = constrain(newAddr, I2C_ADDR_MIN, I2C_ADDR_MAX);
    uint8_t reg = I2C_ADDRESS;
    writeBytes(_addr, reg, (uint8_t *)&newAddr, 1);
    _addr = newAddr;
    delay(20);
    return _addr;
}

void M5ModuleAudio::setFlashWriteBack()
{
    uint8_t data = 1;
    uint8_t reg  = FLASH_WRITE;
    writeBytes(_addr, reg, (uint8_t *)&data, 1);
    delay(20);
}

audio_mic_t M5ModuleAudio::getMICStatus()
{
    uint8_t data;
    uint8_t reg = MICROPHONE_STATUS;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return (audio_mic_t)data;
}

audio_hpmode_t M5ModuleAudio::getHPMode()
{
    uint8_t data;
    uint8_t reg = HEADPHONE_MODE;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return (audio_hpmode_t)data;
}

uint8_t M5ModuleAudio::getHPInsertStatus()
{
    uint8_t data;
    uint8_t reg = HEADPHONE_INSERT_STATUS;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return data;
}

uint8_t M5ModuleAudio::getRGBBrightness()
{
    uint8_t data;
    uint8_t reg = RGB_LED_BRIGHTNESS;
    readBytes(_addr, reg, (uint8_t *)&data, 1);
    return data;
}

uint32_t M5ModuleAudio::getRGBLED(uint8_t num)
{
    uint8_t rgb[3] = {0};
    uint8_t reg    = RGB_LED + num * 3;
    readBytes(_addr, reg, rgb, 3);
    return (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
}

uint8_t M5ModuleAudio::getFirmwareVersion()
{
    uint8_t version;
    uint8_t reg = FIRMWARE_VERSION;
    readBytes(_addr, reg, (uint8_t *)&version, 1);
    return version;
}

uint8_t M5ModuleAudio::getI2CAddress()
{
    uint8_t I2CAddress;
    uint8_t reg = I2C_ADDRESS;
    readBytes(_addr, reg, (uint8_t *)&I2CAddress, 1);
    return I2CAddress;
}

bool M5ModuleAudio::es8388_codec_init()
{
    if (es8388 != nullptr) {
        delete es8388;
        es8388 = nullptr;
    }

    es8388 = new ES8388(_wire, _sda, _scl);
    if (!es8388->init()) {
        ESP_LOGI(TAG, "Failed to initialize ES8388 codec");
        return false;
    }

    ESP_LOGI(TAG, "ES8388 codec initialized successfully");
    return true;
}

bool M5ModuleAudio::i2s_driver_init(int sample_rate)
{
    _i2s_initialized = false;
#if USE_NEW_I2S_API
    I2S.setPins(_i2s_bck, _i2s_ws, _i2s_do, _i2s_di, _i2s_mck);  // SCK, WS, SDOUT, SDIN, MCLK
    if (!I2S.begin(I2S_MODE_STD, sample_rate, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
        ESP_LOGI(TAG, "Failed to initialize I2S (new API)");
        return false;
    }
    _i2s_initialized = true;
    return true;

#else
    // Initialize I2S driver
    i2s_cfg.mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX);
    i2s_cfg.sample_rate          = (uint32_t)sample_rate;
    i2s_cfg.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_cfg.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    i2s_cfg.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1;
    i2s_cfg.dma_buf_count        = 8;
    i2s_cfg.dma_buf_len          = 512;
    i2s_cfg.use_apll             = false;
    i2s_cfg.tx_desc_auto_clear   = true;
    i2s_cfg.fixed_mclk           = 0;

#if defined(CONFIG_IDF_TARGET_ESP32)
    // ESP-IDF 4.x on ESP32 still needs CLK_OUT1 routed explicitly for MCLK.
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
#endif

    // I2S pin configuration
    i2s_pin_cfg.mck_io_num   = _i2s_mck;
    i2s_pin_cfg.bck_io_num   = _i2s_bck;
    i2s_pin_cfg.ws_io_num    = _i2s_ws;
    i2s_pin_cfg.data_out_num = _i2s_do;
    i2s_pin_cfg.data_in_num  = _i2s_di;

    if (i2s_driver_install(i2s_num, &i2s_cfg, 0, NULL) != ESP_OK) {
        ESP_LOGI(TAG, "Failed to install I2S driver");
        return false;
    }

    if (i2s_set_pin(i2s_num, &i2s_pin_cfg) != ESP_OK) {
        ESP_LOGI(TAG, "Failed to set I2S pins");
        return false;
    }

    if (i2s_set_clk(i2s_num, i2s_cfg.sample_rate, i2s_cfg.bits_per_sample, I2S_CHANNEL_STEREO) != ESP_OK) {
        ESP_LOGI(TAG, "Failed to set I2S clock");
        return false;
    }

    i2s_zero_dma_buffer(i2s_num);
    // idf 4.x
    i2s_start(i2s_num);
    _i2s_initialized = true;

    return true;
#endif
}

bool M5ModuleAudio::setSpeakerVolume(int volume)
{
    if (volume < 0 || volume > 100) {
        ESP_LOGI(TAG, "Volume out of range (0-100)");
        return false;
    }

    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setDACVolume(volume)) {
        ESP_LOGI(TAG, "Failed to set speaker volume");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setMicGain(es_mic_gain_t gain)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setMicGain(gain)) {
        ESP_LOGI(TAG, "Failed to set microphone gain");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setMicInputLine(es_adc_input_t input)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setADCInput(input)) {
        ESP_LOGI(TAG, "Failed to set ADC input");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setMicPGAGain(es_mic_gain_t gain)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setMicGain(gain)) {
        ESP_LOGI(TAG, "Failed to configure ES8388 PGA gain");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setMicAdcVolume(uint8_t volume)
{
    if (volume > 100) {
        ESP_LOGI(TAG, "ADC Volume out of range (0-100)");
        return false;
    }

    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setADCVolume(volume)) {
        ESP_LOGI(TAG, "Failed to set ADC volume");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setMute(bool mute)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setDACmute(mute)) {
        ESP_LOGI(TAG, "Failed to set mute state");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setSpeakerOutput(es_dac_output_t output)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setDACOutput(output)) {
        ESP_LOGI(TAG, "Failed to set DAC output");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setMixSourceSelect(es_mixsel_t lmixsel, es_mixsel_t rmixsel)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setMixSourceSelect(lmixsel, rmixsel)) {
        ESP_LOGI(TAG, "Failed to set mix source");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setBitsSample(es_module_t mode, es_bits_length_t bits_len)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    if (!es8388->setBitsSample(mode, bits_len)) {
        ESP_LOGI(TAG, "Failed to set sample bits");
        return false;
    }
    return true;
}

bool M5ModuleAudio::setSampleRate(es_sample_rate_t rate)
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return false;
    }

    uint32_t sample_rate_hz = sample_rate_to_hz(rate);
    if (sample_rate_hz == 0) {
        ESP_LOGI(TAG, "Invalid sample rate");
        return false;
    }

    if (_i2s_initialized) {
#if USE_NEW_I2S_API
        if (!I2S.configureRX(sample_rate_hz, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
            ESP_LOGI(TAG, "Failed to set I2S RX sample rate");
            return false;
        }
        if (!I2S.configureTX(sample_rate_hz, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
            ESP_LOGI(TAG, "Failed to set I2S TX sample rate");
            return false;
        }
#else
        if (i2s_set_clk(i2s_num, sample_rate_hz, i2s_cfg.bits_per_sample, I2S_CHANNEL_STEREO) != ESP_OK) {
            ESP_LOGI(TAG, "Failed to set I2S sample rate");
            return false;
        }
        i2s_cfg.sample_rate = sample_rate_hz;
#endif
    }

    if (!es8388->setSampleRate(rate)) {
        ESP_LOGI(TAG, "Failed to set sample rate");
        return false;
    }
    return true;
}

uint8_t M5ModuleAudio::getMicGain()
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return 0;
    }

    return es8388->getMicGain();
}

uint8_t *M5ModuleAudio::readAllReg()
{
    if (es8388 == nullptr) {
        ESP_LOGI(TAG, "ES8388 codec is not initialized");
        return nullptr;
    }

    return es8388->readAllReg();
}

int M5ModuleAudio::getBufferSize(int duration, int sample_rate)
{
    if (!sample_rate) {
#if USE_NEW_I2S_API
        sample_rate = I2S.txSampleRate();
#else
        sample_rate = i2s_cfg.sample_rate;
#endif
    }
    return duration * sample_rate * 2 * 2;  // sample_rate * bytes_per_sample * channels
}

int M5ModuleAudio::getDuration(int size, int sample_rate)
{
    if (!sample_rate) {
#if USE_NEW_I2S_API
        sample_rate = I2S.txSampleRate();
#else
        sample_rate = i2s_cfg.sample_rate;
#endif
    }
    return size / (sample_rate * 2 * 2);  // sample_rate * bytes_per_sample * channels
}

bool M5ModuleAudio::record(FS &fs, const char *filename, int size)
{
    // Open file for writing
    File file = fs.open(filename, FILE_WRITE);
    if (!file) {
        ESP_LOGI(TAG, "Failed to open file for recording");
        return false;
    }

    // Define chunk size (e.g., 1024 bytes)
    const size_t CHUNK_SIZE = 1024;
    uint8_t buffer[CHUNK_SIZE];
    size_t total_bytes_to_record = size;  // sample_rate * bytes_per_sample * channels
    size_t bytes_recorded        = 0;

    while (bytes_recorded < total_bytes_to_record) {
        size_t bytes_to_read = CHUNK_SIZE;
        if (bytes_recorded + CHUNK_SIZE > total_bytes_to_record) {
            bytes_to_read = total_bytes_to_record - bytes_recorded;
        }

        size_t bytes_read = 0;
#if USE_NEW_I2S_API
        bytes_read = I2S.readBytes((char *)buffer, bytes_to_read);
        if (bytes_read != bytes_to_read) {
            ESP_LOGI(TAG, "Recording failed during I2S read");
            file.close();
            return false;
        }
#else
        esp_err_t err = i2s_read(i2s_num, buffer, bytes_to_read, &bytes_read, portMAX_DELAY);
        if (err != ESP_OK || bytes_read != bytes_to_read) {
            ESP_LOGI(TAG, "Recording failed during I2S read");
            file.close();
            return false;
        }
#endif

        file.write(buffer, bytes_read);
        bytes_recorded += bytes_read;
    }

    file.close();
    return true;
}

bool M5ModuleAudio::record(uint8_t *buffer, int size)
{
    // Calculate the number of bytes to record
    size_t bytes_to_record = size;  // sample_rate * bytes_per_sample * channels
    memset(buffer, 0, bytes_to_record);

    size_t bytes_read = 0;
#if USE_NEW_I2S_API
    bytes_read = I2S.readBytes((char *)buffer, bytes_to_record);
    if (bytes_read != bytes_to_record) {
        ESP_LOGI(TAG, "Recording failed during I2S read");
        return false;
    }
#else
    esp_err_t err = i2s_read(i2s_num, buffer, bytes_to_record, &bytes_read, getDuration(size) * 1000 + 1000);
    if (err != ESP_OK || bytes_read != bytes_to_record) {
        ESP_LOGI(TAG, "Recording failed");
        return false;
    }
#endif
    return true;
}

bool M5ModuleAudio::play(FS &fs, const char *filename)
{
    // Open file for reading
    File file = fs.open(filename, FILE_READ);
    if (!file) {
        ESP_LOGI(TAG, "Failed to open file for playback");
        return false;
    }

    // Define chunk size (e.g., 1024 bytes)
    const size_t CHUNK_SIZE = 1024;
    uint8_t buffer[CHUNK_SIZE];

    while (file.available()) {
        size_t bytes_to_read = CHUNK_SIZE;
        if (file.available() < CHUNK_SIZE) {
            bytes_to_read = file.available();
        }

        size_t bytes_read = file.read(buffer, bytes_to_read);
        if (bytes_read > 0) {
            size_t bytes_written = 0;
#if USE_NEW_I2S_API
            bytes_written = I2S.write(buffer, bytes_read);
            if (bytes_written < 0) {
                ESP_LOGI(TAG, "Playback failed during I2S write");
                file.close();
                return false;
            }
#else
            esp_err_t err = i2s_write(i2s_num, buffer, bytes_read, &bytes_written, portMAX_DELAY);
            if (err != ESP_OK) {
                ESP_LOGI(TAG, "Playback failed during I2S write");
                file.close();
                return false;
            }
#endif
        }
    }

    file.close();
    return true;
}

bool M5ModuleAudio::play(const uint8_t *buffer, int size)
{
    size_t bytes_written = 0;
#if USE_NEW_I2S_API
    bytes_written = I2S.write(buffer, size);
    if (bytes_written < 0) {
        ESP_LOGI(TAG, "Playback failed");
        return false;
    }
#else
    esp_err_t err = i2s_write(i2s_num, buffer, size, &bytes_written, portMAX_DELAY);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Playback failed");
        return false;
    }
#endif
    return true;
}

void M5ModuleAudio::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
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
    Serial.print(",");
    Serial.print(reg);
    Serial.print(",");
    for (int i = 0; i < length; i++) {
        Serial.print(buffer[i]);
        if (i < length - 1) {
            Serial.print(",");
        }
    }
    Serial.println("]");
#else
#endif
}

void M5ModuleAudio::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
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
    Serial.print(",");
    Serial.print(reg);
    Serial.print(",");
    for (int i = 0; i < length; i++) {
        Serial.print(buffer[i]);
        if (i < length - 1) {
            Serial.print(",");
        }
    }
    Serial.println("]");
#else
#endif
}
