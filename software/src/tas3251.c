#include <libopencm3/stm32/rcc.h>
#include "tas3251.h"

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
/**
  * @brief  Clock Config.
  * @param  hi2s: might be required to set audio peripheral predivider if any.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application     
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
__weak void BSP_AUDIO_OUT_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t AudioFreq, void *Params) {
    RCC_PeriphCLKInitTypeDef rccclkinit;
    uint8_t index = 0, freqindex = 0xFF;

    for (index = 0; index < 8; index++) {
        if (I2SFreq[index] == AudioFreq) {
            freqindex = index;
        }
    }
    /* Enable PLLI2S clock */
    HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);
    /* PLLI2S_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    if ((freqindex & 0x7) == 0) {
        /* I2S clock config 
    PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) � (PLLI2SN/PLLM)
    I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */
        rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
        rccclkinit.PLLI2S.PLLI2SN = I2SPLLN[freqindex];
        rccclkinit.PLLI2S.PLLI2SR = I2SPLLR[freqindex];
        HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
    } else {
        /* I2S clock config 
    PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) � (PLLI2SN/PLLM)
    I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */
        rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
        rccclkinit.PLLI2S.PLLI2SN = 258;
        rccclkinit.PLLI2S.PLLI2SR = 3;
        HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
    }
}
#endif

bool tas3251_init(void) {
    /* I2s */

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

    return true;
}
