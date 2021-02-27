#include <stdbool.h>
#include <stdint.h>

typedef enum { left, right } channel_t;

bool tas3251_init(uint16_t i2c_device_addr, uint32_t audio_freq);
void tas3251_mute(bool mute);
void tas3251_rst(bool rst);
void tas3251_play(uint16_t* buffer, uint32_t size_bytes, void (*notify_end_play)());
void tas3251_set_volume(int purcent, channel_t channel);
