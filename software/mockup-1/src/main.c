#include "bsp.h"
#include "tas3251.h"
#include "cs43l22.h"
#include "audio_sample.h"

/*
typedef struct
{
  uint32_t  (*Init)(uint16_t, uint16_t, uint8_t, uint32_t);
  void      (*DeInit)(void);
  uint32_t  (*ReadID)(uint16_t);
  uint32_t  (*Play)(uint16_t, uint16_t*, uint16_t);
  uint32_t  (*Pause)(uint16_t);
  uint32_t  (*Resume)(uint16_t);
  uint32_t  (*Stop)(uint16_t, uint32_t);
  uint32_t  (*SetFrequency)(uint16_t, uint32_t);
  uint32_t  (*SetVolume)(uint16_t, uint8_t);
  uint32_t  (*SetMute)(uint16_t, uint32_t);
  uint32_t  (*SetOutputMode)(uint16_t, uint8_t);
  uint32_t  (*Reset)(uint16_t);
} AUDIO_DrvTypeDef;
*/

static void _loop_play_tas3251() {
    WAVE_FormatTypeDef* wav = wav_get_audio_sample();
    tas3251_play(wav->data, wav->FileSize, _loop_play_tas3251);
}

static void _loop_play_cs43l22() {
    WAVE_FormatTypeDef* wav = wav_get_audio_sample();
    tas3251_play(wav->data, wav->FileSize, _loop_play_cs43l22);
}

int main(void) {
    /* init bsp and MMI */
    if (!bsp_init() || !mmi_init() /*|| !usb_init()*/)
        fault();

    /* if (!usb_start())
        fault(); */

    //if (!tas3251_init(0x94, 48000))
    if (!tas3251_init(0x94, 48000))
        fault();

    //tas3251_set_volume(10, left);
    //tas3251_set_volume(10, right);
    _loop_play_tas3251();

    //if (cs43l22_Init(0x96, OUTPUT_DEVICE_HEADPHONE, 128, 48000))
    //    fault();
    //_loop_playing_cs43l22();

    while (1) {
        mmi_heartbeat();
    }
}
