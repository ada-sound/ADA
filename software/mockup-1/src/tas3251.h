#include <stdbool.h>
#include <stdint.h>

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

typedef enum { left, right } channel_t;

bool tas3251_init(uint16_t i2c_device_addr, uint32_t audio_freq);
void tas3251_mute(bool mute);
void tas3251_rst(bool rst);
void tas3251_play(uint16_t* buffer, uint32_t size_bytes, void (*notify_end_play)());
void tas3251_set_volume(int purcent, channel_t channel);
