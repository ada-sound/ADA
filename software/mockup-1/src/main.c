#include "bsp.h"
#include "tas3251.h"
#include "cs43l22.h"
#include "audio_sample.h"

static void _loop_play_tas3251() {
    WAVE_FormatTypeDef* wav = wav_get_audio_sample();
    tas3251_play(wav->data, wav->FileSize, _loop_play_tas3251);
}

int main(void) {
    /* init bsp and MMI */
    if (!bsp_init() || !mmi_init()/* || !usb_init()*/)
        fault();

    if (!tas3251_init(0x94, 48000))
        fault();

    tas3251_set_volume(90, left);
    tas3251_set_volume(90, right);
    _loop_play_tas3251();

    while (1) {
        mmi_heartbeat();
    }
}
