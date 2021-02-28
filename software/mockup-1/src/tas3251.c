#include <stm32f4xx_hal.h>

#include "bsp.h"
#include "tas3251.h"
#include "tas3251_init_db.h"
#include "tas3251_registers.h"

/* IO */
#define RST_PIN GPIO_PIN_14
#define RST_GPIO GPIOC
#define MUTE_PIN GPIO_PIN_15
#define MUTE_GPIO GPIOC

#define DMA_MAX_SZE 0xFFFF

/* Audio status definition */
#define AUDIODATA_SIZE 2 /* 16-bits audio data size */

/* Variables used in normal mode to manage audio file during DMA transfer */
static uint16_t* AudioPos;
static uint32_t AudioSizeBytes;
static uint16_t* AudioCurrentPos;
static uint32_t AudioRemSizeBytes;
static void (*_notify_end_play)();

static uint16_t _i2c_device_addr;
static uint32_t _audio_freq;

void tas3251_play(uint16_t* buffer, uint32_t size_bytes, void (*notify_end_play)()) {
    _notify_end_play = notify_end_play;

    /* Update the Media layer and enable it for play */
    AudioPos = buffer;
    AudioCurrentPos = AudioPos;
    AudioSizeBytes = size_bytes;
    AudioRemSizeBytes = AudioSizeBytes;

    uint16_t transmit_size;
    if (AudioRemSizeBytes < (DMA_MAX_SZE * AUDIODATA_SIZE))
        transmit_size = AudioRemSizeBytes / AUDIODATA_SIZE;
    else
        transmit_size = DMA_MAX_SZE;

    i2s_transmit_dma(AudioCurrentPos, transmit_size);

    AudioRemSizeBytes -= transmit_size * AUDIODATA_SIZE;
    AudioCurrentPos += transmit_size;
}

static void _dma_transfer_complete() {
    if (AudioRemSizeBytes > 0 && AudioRemSizeBytes >= (DMA_MAX_SZE * AUDIODATA_SIZE)) {
        uint16_t transmit_size;
        if (AudioRemSizeBytes < (DMA_MAX_SZE * AUDIODATA_SIZE))
            transmit_size = AudioRemSizeBytes / AUDIODATA_SIZE;
        else
            transmit_size = DMA_MAX_SZE;

        i2s_transmit_dma(AudioCurrentPos, transmit_size);

        AudioRemSizeBytes -= transmit_size * AUDIODATA_SIZE;
        AudioCurrentPos += transmit_size;
    } else {
        /* Call DMA Stop to disable DMA stream before stopping codec */
        i2s_stop_dma();
        (*_notify_end_play)();
    }
}

static bool _configure_io_out(GPIO_TypeDef* gpio, uint32_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Audio reset pin configuration */
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);

    /* Wait for a delay to insure registers erasing */
    HAL_Delay(5);

    return true;
}

static void _startup_amp(uint32_t i2c_device_addr) {
    for (int i = 0; i < sizeof(registers) / sizeof(cfg_reg); i++)
        i2c_write(_i2c_device_addr, registers[i].offset, registers[i].value);
}

void tas3251_set_volume(int purcent, channel_t channel) {
    /* 0%=255, 100%=48 */
    uint8_t set = 48 + ((100 - purcent) * (255 - 48)) / 100;
    i2c_write(_i2c_device_addr, channel == left ? REG_LEFT_DIGITAL_VOLUME:REG_RIGHT_DIGITAL_VOLUME, set);
}

void tas3251_mute(bool mute) { HAL_GPIO_WritePin(MUTE_GPIO, MUTE_PIN, mute ? GPIO_PIN_SET : GPIO_PIN_RESET); }

void tas3251_rst(bool rst) { HAL_GPIO_WritePin(RST_GPIO, RST_PIN, rst ? GPIO_PIN_RESET : GPIO_PIN_SET); }

bool tas3251_init(uint16_t i2c_device_addr, uint32_t audio_freq) {    
    _i2c_device_addr = i2c_device_addr;
    _audio_freq = audio_freq;

    if (!i2s_init(_audio_freq, _dma_transfer_complete))
        return false;

    /* i2c commands to start the TAS3251 */
    i2c_init();

    /* mute, rst the TAS3251 */
    _configure_io_out(RST_GPIO, RST_PIN);
    _configure_io_out(MUTE_GPIO, MUTE_PIN);

    /* start amplifier */
    _startup_amp(_i2c_device_addr);

    tas3251_mute(false);
    tas3251_rst(false);

    return true;
}
