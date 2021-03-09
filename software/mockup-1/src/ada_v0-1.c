#include <stdint.h>

#include "bsp.h"
#include "tas3251.h"
//#include "audio_sample.h"

static int8_t Audio_Init(uint32_t AudioFreq, uint32_t Volume, uint32_t options);
static int8_t Audio_DeInit(uint32_t options);
static int8_t Audio_PlaybackCmd(uint8_t* pbuf, uint32_t size, uint8_t cmd);
static int8_t Audio_VolumeCtl(uint8_t vol);
static int8_t Audio_MuteCtl(uint8_t cmd);
static int8_t Audio_PeriodicTC(uint8_t cmd);
static int8_t Audio_GetState(void);

static USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops = {Audio_Init,    Audio_DeInit,     Audio_PlaybackCmd, Audio_VolumeCtl,
                                                Audio_MuteCtl, Audio_PeriodicTC, Audio_GetState};

#if 0
static void _loop_play_tas3251() {
    WAVE_FormatTypeDef* wav = wav_get_audio_sample();
    tas3251_play(wav->data, wav->FileSize, _loop_play_tas3251);
}
#endif

static const uint16_t _i2c_device_address = 0x94;

bool ada_v01_init(void) {
    if (!tas3251_init(_i2c_device_address, 48000))
        return false;

    tas3251_set_volume(0, left);
    tas3251_set_volume(0, right);

    return usb_start(&USBD_AUDIO_fops);
}

/**
  * @brief  Initializes the AUDIO media low layer.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  options: Reserved for future use 
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Audio_Init(uint32_t AudioFreq, uint32_t Volume, uint32_t options) {
    if (tas3251_init(_i2c_device_address, AudioFreq)) {
        tas3251_set_volume(70, left);
        tas3251_set_volume(70, right);
        return USBD_OK;
    }
    return USBD_FAIL;
}

/**
  * @brief  De-Initializes the AUDIO media low layer.      
  * @param  options: Reserved for future use
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Audio_DeInit(uint32_t options) {
    //BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    return 0;
}

/**
  * @brief  Handles AUDIO command.        
  * @param  pbuf: Pointer to buffer of data to be sent
  * @param  size: Number of data to be sent (in bytes)
  * @param  cmd: Command opcode
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Audio_PlaybackCmd(uint8_t* pbuf, uint32_t size, uint8_t cmd) {
    switch (cmd) {
        case AUDIO_CMD_START:
            tas3251_play((uint16_t*)pbuf, size, usb_transfer_complete);
            break;

        case AUDIO_CMD_PLAY:
            tas3251_play((uint16_t*)pbuf, size, usb_transfer_complete);
            break;
    }

    return 0;
}

/**
  * @brief  Controls AUDIO Volume.             
  * @param  vol: Volume level (0..100)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Audio_VolumeCtl(uint8_t vol) {
    tas3251_set_volume(vol, left);
    tas3251_set_volume(vol, right);
    return 0;
}

/**
  * @brief  Controls AUDIO Mute.              
  * @param  cmd: Command opcode
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Audio_MuteCtl(uint8_t cmd) {
    tas3251_mute(cmd != 0);
    return 0;
}

/**
  * @brief  Audio_PeriodicTC              
  * @param  cmd: Command opcode
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Audio_PeriodicTC(uint8_t cmd) { return 0; }

/**
  * @brief  Gets AUDIO State.              
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t Audio_GetState(void) { return 0; }

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/qq
