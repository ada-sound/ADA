#include <stdbool.h>
#include <stdint.h>

bool tas3251_init(uint32_t i2c_device_addr);
void tas3251_mute(bool mute);
void tas3251_rst(bool rst);
bool tas3251_set_audio_freq(uint32_t audio_freq);