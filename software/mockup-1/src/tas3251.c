#include <stm32f4xx_hal.h>

#include "tas3251.h"
#include "bsp.h"

/* complete configuration example */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#include "luglio.h"
#pragma GCC diagnostic pop

#include "tas3251_registers.h"

/* IO */
#define RST_PIN GPIO_PIN_14
#define RST_GPIO GPIOC
#define MUTE_PIN GPIO_PIN_15
#define MUTE_GPIO GPIOC

#define DMA_MAX_SZE 0xFFFF

/* Audio status definition */
#define AUDIODATA_SIZE 2 /* 16-bits audio data size */

typedef struct {
    uint32_t ChunkID;       /* 0 */
    uint32_t FileSize;      /* 4 */
    uint32_t FileFormat;    /* 8 */
    uint32_t SubChunk1ID;   /* 12 */
    uint32_t SubChunk1Size; /* 16 */
    uint16_t AudioFormat;   /* 20 */
    uint16_t NbrChannels;   /* 22 */
    uint32_t SampleRate;    /* 24 */

    uint32_t ByteRate;      /* 28 */
    uint16_t BlockAlign;    /* 32 */
    uint16_t BitPerSample;  /* 34 */
    uint32_t SubChunk2ID;   /* 36 */
    uint32_t SubChunk2Size; /* 40 */

} WAVE_FormatTypeDef;

extern WAVE_FormatTypeDef* waveformat;

/* Variables used in normal mode to manage audio file during DMA transfer */
static uint16_t* AudioPos;
static uint32_t AudioSizeBytes;
static uint16_t* AudioCurrentPos;
static uint32_t AudioRemSizeBytes;

void BSP_AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t SizeBytes) {
    /* Update the Media layer and enable it for play */
    AudioPos = pBuffer;
    AudioCurrentPos = AudioPos;
    AudioSizeBytes = SizeBytes;
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

void BSP_AUDIO_OUT_TransferComplete_CallBack() {
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
        //i2s_stop_dma();

        /* reset to begining */
        BSP_AUDIO_OUT_Play(AudioPos, AudioSizeBytes);
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
    int i = 0;
    while (i < sizeof(registers) / sizeof(cfg_reg)) {
        switch (registers[i].command) {
            case CFG_META_SWITCH:
                // Used in legacy applications.  Ignored here.
                break;
            case CFG_META_DELAY:
                HAL_Delay(registers[i].param);
                break;
            case CFG_META_BURST:
                //I2Cx_WriteMultipleData(i2c_device_addr, (unsigned char*)&registers[i + 1], registers[i].param);
                i += (registers[i].param + 1) / 2;
                break;
            default:
                i2c_write(i2c_device_addr, registers[i].offset, registers[i].value);
                break;
        }
        i++;
    }
}

static void _startup_amp_simple(uint32_t i2c_device_addr) {
    /* startup sequence according to SLASEG6A – MAY 2018 – REVISED NOVEMBER 2018 §8.3.11.1 */

    /* 1. Apply power to DAC_DVDD, DAC_AVDD, GVDD_x, and PVDD_x */
    /* 2. Apply I2S or TDM clocks to the device to enable the internal system clocks */
    /* 3. Mute the left and right DAC channels */
    i2c_write(i2c_device_addr, TAS3251_REG_PAGE, 0x00);
    HAL_Delay(1);
    i2c_write(i2c_device_addr, TAS3251_REG_BOOK, 0x00);
    HAL_Delay(1);

    /* mute right and left */
    i2c_write(i2c_device_addr, TAS3251_REG_MUTE, TAS3251_MUTE_LEFT|TAS3251_MUTE_RIGHT);
    HAL_Delay(1);

    /* standby */
    i2c_write(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_STANDBY);
    uint8_t status = i2c_read(i2c_device_addr, TAS3251_REG_STANDBY|TAS3251_STANDBY_DSPR);
    HAL_Delay(1);

    /* enable */
    //i2c_write(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_ENABLE);
    //HAL_Delay(1);

    /* 5.1. set digital volume */
    i2c_write(i2c_device_addr, TAS3251_REG_LEFT_DIGITAL_VOLUME, (uint8_t)200);
    HAL_Delay(1);
    i2c_write(i2c_device_addr, TAS3251_REG_RIGHT_DIGITAL_VOLUME, (uint8_t)200);
    HAL_Delay(1);

    /* dsp out of standby */
    //i2c_write(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_DSPR);
    //HAL_Delay(1);

    /* enable (but no DSP) */
    i2c_write(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_ENABLE|TAS3251_STANDBY_DSPR);
    status = i2c_read(i2c_device_addr, TAS3251_REG_STANDBY);
    HAL_Delay(1);

    /* unmute right and left */
    i2c_write(i2c_device_addr, TAS3251_REG_MUTE, 0);
}

void tas3251_mute(bool mute) { HAL_GPIO_WritePin(MUTE_GPIO, MUTE_PIN, mute ? GPIO_PIN_RESET : GPIO_PIN_SET); }

void tas3251_rst(bool rst) { HAL_GPIO_WritePin(RST_GPIO, RST_PIN, rst ? GPIO_PIN_RESET : GPIO_PIN_SET); }

bool tas3251_init(uint32_t i2c_device_addr) {
    if (!i2s_init(48000, BSP_AUDIO_OUT_TransferComplete_CallBack))
        return false;

    /* i2c commands to start the TAS3251 */
    i2c_init(i2c_device_addr);

    /* mute, rst the TAS3251 */
    _configure_io_out(RST_GPIO, RST_PIN);
    _configure_io_out(MUTE_GPIO, MUTE_PIN);
    tas3251_rst(false);
    tas3251_mute(true);

    /* start amplifier */
    //_startup_amp_simple(i2c_device_addr);
    _startup_amp(i2c_device_addr);

    /* */
    BSP_AUDIO_OUT_Play((uint16_t*)((uint8_t*)waveformat + sizeof(WAVE_FormatTypeDef)), waveformat->FileSize);

    return true;
}

bool tas3251_set_output_freq(void) { return true; }
