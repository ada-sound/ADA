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

static void _startup_amp_luglio(uint32_t i2c_device_addr) {
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

static void _startup_amp(uint32_t i2c_device_addr) {
    struct { uint8_t offset; uint8_t value; } startup_regs[] = {
        //program memory
        { 0x00, 0x00 }, //REG_PAGE                      page 0
        { 0x7f, 0x00 }, //REG_BOOK                      book 0
        { 0x02, 0x11 }, //REG_STANDBY                   power down and standby
        { 0x01, 0x11 }, //REG_RESET                     reset mode registers and modules
        { 0x00, 0x00 }, 
        { 0x00, 0x00 },
        { 0x00, 0x00 },
        { 0x00, 0x00 },
        { 0x03, 0x11 }, //REG_MUTE                      mute lr
        { 0x2a, 0x00 }, //REG_DAC_DATA_PATH             zero data lr (mute)
        { 0x25, 0x18 }, //REG_CLOCK_DETECTION_CONFIG    ignore MCLK halt detection, Ignore MCLK detection
        { 0x0d, 0x10 }, //REG_PLL_CLOCK_CONFIGURATION   the PLL reference clock is SCLK
        { 0x02, 0x00 }, //REG_STANDBY                   normal operation dac and dsp


        //Sample rate update
        { 0x00, 0x00 },
        { 0x7f, 0x00 },
        { 0x02, 0x80 }, //REG_STANDBY                   reset the DSP

        { 0x00, 0x00 },
        { 0x7f, 0x00 },

        // speed 03-48k 04-96k
        //dynamically reading speed
        { 0x22, 0x03 }, //REG_FS_SPEED_MODE             48 kHz

        { 0x00, 0x00 }, 
        { 0x7f, 0x00 }, 
        { 0x02, 0x00 }, //REG_STANDBY                   normal operation dac and dsp
        //register tuning
        { 0x00, 0x00 }, //REG_PAGE                      page 0
        { 0x7f, 0x00 }, //REG_BOOK                      book 0
        { 0x00, 0x00 },
        { 0x07, 0x00 }, //REG_SDOUT                     SDOUT is the DSP output
        { 0x08, 0x20 }, //REG_GPIO                      SDOUT is output
        { 0x55, 0x07 }, //REG_GPIO2_OUTPUT              0111: Serial audio interface data output (SDOUT)
        { 0x00, 0x00 },
        { 0x7f, 0x00 },
        { 0x00, 0x00 },
        { 0x3d, 0x30 }, //REG_LEFT_DIGITAL_VOLUME       100%
        { 0x3e, 0x30 }, //REG_RIGHT_DIGITAL_VOLUME      100%
        { 0x00, 0x00 },
        { 0x7f, 0x00 },
        { 0x00, 0x01 }, //REG_PAGE                      page 1
        { 0x02, 0x00 }, //REG_ANALOG_GAIN_CONTROL       0 db

        { 0x00, 0x00 }, //REG_PAGE                      page 0
        { 0x7f, 0x00 },
        { 0x03, 0x00 }, //REG_MUTE                      unmute lr
        { 0x2a, 0x11 }, //REG_DAC_DATA_PATH             {Left,Right} DAC Data Path is {Left,Right} channel data
    };

    for (int i = 0; i < sizeof(startup_regs) / sizeof(struct { uint8_t offset; uint8_t value; }); i++)
        i2c_write(_i2c_device_addr, registers[i].offset, registers[i].value);
}

static void _startup_amp_simple(uint32_t i2c_device_addr) {
    /* startup sequence according to SLASEG6A – MAY 2018 – REVISED NOVEMBER 2018 §8.3.11.1 */

    /* 1. Apply power to DAC_DVDD, DAC_AVDD, GVDD_x, and PVDD_x */
    /* 2. Apply I2S or TDM clocks to the device to enable the internal system clocks */
    /* 3. Mute the left and right DAC channels */
    i2c_write(i2c_device_addr, REG_PAGE, 0x00);
    i2c_write(i2c_device_addr, REG_BOOK, 0x00);

    /* mute right and left */
    i2c_write(i2c_device_addr, REG_MUTE, TAS3251_MUTE_LEFT|TAS3251_MUTE_RIGHT);

    /* volume 0 */
    tas3251_set_volume(0, left);
    tas3251_set_volume(0, right);

    /* enable dsp */
    i2c_write(i2c_device_addr, REG_STANDBY, REG_STANDBY|TAS3251_STANDBY_DSPR);
    //uint8_t status = i2c_read(i2c_device_addr, REG_STANDBY|TAS3251_STANDBY_DSPR);

    /* unmute right and left */
    i2c_write(i2c_device_addr, REG_MUTE, 0);

    /* enable global */
    i2c_write(i2c_device_addr, REG_STANDBY, TAS3251_STANDBY_ENABLE|TAS3251_STANDBY_DSPR);
    //status = i2c_read(i2c_device_addr, REG_STANDBY);
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

    tas3251_rst(false);
    tas3251_mute(false);

    /* start amplifier */
    //_startup_amp(_i2c_device_addr);
    //_startup_amp_simple(_i2c_device_addr);
    _startup_amp_luglio(_i2c_device_addr);

    return true;
}
