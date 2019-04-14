#include "i2s.h"

/* These PLL parameters are valid when the f(VCO clock) = 1Mhz */
static const int _I2S_freq_max = 8;
static const struct {
    uint32_t freq;
    uint32_t plln;
    uint32_t pllr;
} _I2S_freq[8 /*_I2S_freq_max*/] = {{8000, 256, 5},  {11025, 429, 4}, {16000, 213, 4},
                                    {22050, 429, 4}, {32000, 426, 4}, {44100, 271, 6},
                                    {48000, 258, 3}, {96000, 344, 1}};

#if 0
    /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */ 
    BSP_AUDIO_OUT_ClockConfig(&hAudioOutI2s, AudioFreq, NULL);

    /* I2S data transfer preparation:
    Prepare the Media to be used for the audio transfer from memory to I2S peripheral */
    hAudioOutI2s.Instance = I2S3;
    if(HAL_I2S_GetState(&hAudioOutI2s) == HAL_I2S_STATE_RESET)
    {
    /* Init the I2S MSP: this __weak function can be redefined by the application*/
    BSP_AUDIO_OUT_MspInit(&hAudioOutI2s, NULL);
    }

    /* I2S data transfer preparation:
    Prepare the Media to be used for the audio transfer from memory to I2S peripheral */
    /* Configure the I2S peripheral */
    if(I2S3_Init(AudioFreq) != AUDIO_OK)
    return AUDIO_ERROR;
#endif
