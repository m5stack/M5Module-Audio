# Module-Audio

## Overview

### SKU:M144

**Module Audio** is an audio-interaction expansion module for M5Stack, built on the ES8388 audio codec solution. It offers dual 3.5 mm jacks (one TRS jack for microphone input only, one TRRS jack for both microphone input and headphone output) to fulfill various recording and stereo playback needs. An onboard STM32G030F6P6 microcontroller handles TRRS jack insertion detection and drives WS2812C RGB LEDs. Register configuration enables automatic switching between CTIA (American) and OMTP (International) wiring standards, ensuring compatibility with most headsets featuring integrated microphones. This module is ideal for smart voice, interactive art, educational entertainment, portable recording, and other audio applications.

## I2C bus selection

Module Audio supports both the M5Unified internal I2C bus and the Arduino `Wire` bus. Call `M5.begin()` before using `M5.In_I2C`.

```cpp
M5ModuleAudio audio;

M5.begin();
audio.begin(M5.In_I2C);
```

Existing `Wire` code remains supported:

```cpp
M5ModuleAudio audio;

M5.begin();
audio.begin(Wire);
```

## Microphone routing

The LINE/MIC jack and the headset microphone share the ES8388 `LIN1` input. Keep the two STM32-controlled paths
mutually exclusive.

Use the LINE/MIC jack:

```cpp
audio.setHPMICStatus(AUDIO_MIC_CLOSE);
audio.setMICStatus(AUDIO_MIC_OPEN);
audio.setMicInputLine(ADC_INPUT_LINPUT1_RINPUT1);
```

Use the microphone in a connected headset:

```cpp
if (audio.getHPInsertStatus()) {
    audio.setMICStatus(AUDIO_MIC_CLOSE);
    audio.setHPMICStatus(AUDIO_MIC_OPEN);
    audio.setMicInputLine(ADC_INPUT_LINPUT1_RINPUT1);
}
```

Register `0x11` is reset to `AUDIO_MIC_CLOSE` at power-up. Headset insertion or removal does not change it.

## Device UID

Firmware `0xF1` exposes the STM32 96-bit UID in registers `0xE0` through `0xEB`:

```cpp
uint8_t uid[DEVICE_UID_LENGTH];
if (audio.getDeviceUID(uid)) {
    for (uint8_t value : uid) {
        Serial.printf("%02X", value);
    }
    Serial.println();
}
```

## Related Link

- [Document & Datasheet](https://docs.m5stack.com/en/module/Module-Audio)

## License

- [MIT](https://github.com/m5stack/Module-Audio/blob/main/LICENSE)
