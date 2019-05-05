#include <stm32f4xx_hal.h>
#include "tas3251.h"
#include "tas3251_registers.h"

/* I2S peripheral configuration defines */
#define I2S3 SPI3
#define I2S3_CLK_ENABLE() __HAL_RCC_SPI3_CLK_ENABLE()
#define I2S3_CLK_DISABLE() __HAL_RCC_SPI3_CLK_DISABLE()
#define I2S3_SCK_SD_WS_AF GPIO_AF6_SPI3
#define I2S3_SCK_SD_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2S3_MCK_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2S3_WS_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define I2S3_WS_PIN GPIO_PIN_4
#define I2S3_SCK_PIN GPIO_PIN_10
#define I2S3_SD_PIN GPIO_PIN_12
#define I2S3_MCK_PIN GPIO_PIN_7
#define I2S3_SCK_SD_GPIO_PORT GPIOC
#define I2S3_WS_GPIO_PORT GPIOA
#define I2S3_MCK_GPIO_PORT GPIOC

/* I2S DMA Stream definitions */
#define I2S3_DMAx_CLK_ENABLE() __HAL_RCC_DMA1_CLK_ENABLE()
#define I2S3_DMAx_CLK_DISABLE() __HAL_RCC_DMA1_CLK_DISABLE()
#define I2S3_DMAx_STREAM DMA1_Stream7
#define I2S3_DMAx_CHANNEL DMA_CHANNEL_0
#define I2S3_DMAx_IRQ DMA1_Stream7_IRQn
#define I2S3_DMAx_PERIPH_DATA_SIZE DMA_PDATAALIGN_HALFWORD
#define I2S3_DMAx_MEM_DATA_SIZE DMA_MDATAALIGN_HALFWORD
#define DMA_MAX_SZE 0xFFFF

#define DMA_MAX(_X_) (((_X_) <= DMA_MAX_SZE) ? (_X_) : DMA_MAX_SZE)

#define I2S3_IRQHandler DMA1_Stream7_IRQHandler

/* Select the interrupt preemption priority and subpriority for the DMA interrupt */
#define AUDIO_OUT_IRQ_PREPRIO 0x0E /* Select the preemption priority level(0 is the highest) */

/* Audio status definition */
#define AUDIO_OK 0
#define AUDIO_ERROR 1
#define AUDIO_TIMEOUT 2
#define AUDIODATA_SIZE 2 /* 16-bits audio data size */

/* Codec audio Standards */
#define CODEC_STANDARD 0x04
#define I2S_STANDARD I2S_STANDARD_PHILIPS

/* These PLL parameters are valid when the f(VCO clock) = 1Mhz */
static const uint32_t I2SFreq[8] = {8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000};
static const uint32_t I2SPLLN[8] = {256, 429, 213, 429, 426, 271, 258, 344};
static const uint32_t I2SPLLR[8] = {5, 4, 4, 4, 4, 6, 3, 1};

/* I2C clock speed configuration (in Hz) */
#ifndef BSP_I2C_SPEED
#define BSP_I2C_SPEED 100000
#endif /* BSP_I2C_SPEED */

/* I2C peripheral configuration defines (control interface of the audio codec) */
#define DISCOVERY_I2Cx I2C1
#define DISCOVERY_I2Cx_CLK_ENABLE() __HAL_RCC_I2C1_CLK_ENABLE()
#define DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_I2Cx_SCL_SDA_AF GPIO_AF4_I2C1
#define DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT GPIOB
#define DISCOVERY_I2Cx_SCL_PIN GPIO_PIN_6
#define DISCOVERY_I2Cx_SDA_PIN GPIO_PIN_9

#define DISCOVERY_I2Cx_FORCE_RESET() __HAL_RCC_I2C1_FORCE_RESET()
#define DISCOVERY_I2Cx_RELEASE_RESET() __HAL_RCC_I2C1_RELEASE_RESET()

/* I2C interrupt requests */
#define DISCOVERY_I2Cx_EV_IRQn I2C1_EV_IRQn
#define DISCOVERY_I2Cx_ER_IRQn I2C1_ER_IRQn

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define I2Cx_TIMEOUT_MAX 0x1000 /*<! The value of the maximal timeout for BUS waiting loops */

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

static I2S_HandleTypeDef hAudioOutI2s;

/* Variables used in normal mode to manage audio file during DMA transfer */
static uint16_t* AudioPos;
static uint32_t AudioSizeBytes;
static uint16_t* AudioCurrentPos;
static uint32_t AudioRemSizeBytes;

uint32_t I2cxTimeout = I2Cx_TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */

static I2C_HandleTypeDef I2cHandle;

static uint8_t _i2s3_init(uint32_t AudioFreq) {
    /* Initialize the hAudioOutI2s Instance parameter */
    hAudioOutI2s.Instance = I2S3;

    /* Disable I2S block */
    __HAL_I2S_DISABLE(&hAudioOutI2s);

    /* I2S3 peripheral configuration */
    hAudioOutI2s.Init.AudioFreq = AudioFreq;
    hAudioOutI2s.Init.ClockSource = I2S_CLOCK_PLL;
    hAudioOutI2s.Init.CPOL = I2S_CPOL_LOW;
    hAudioOutI2s.Init.DataFormat = I2S_DATAFORMAT_16B;
    hAudioOutI2s.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hAudioOutI2s.Init.Mode = I2S_MODE_MASTER_TX;
    hAudioOutI2s.Init.Standard = I2S_STANDARD;

    /* Initialize the I2S peripheral with the structure above */
    if (HAL_I2S_Init(&hAudioOutI2s) != HAL_OK) {
        return AUDIO_ERROR;
    } else {
        return AUDIO_OK;
    }
}

/**
  * @brief  AUDIO OUT I2S MSP Init.
  * @param  hi2s: might be required to set audio peripheral predivider if any.
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
static void BSP_AUDIO_OUT_MspInit(I2S_HandleTypeDef* hi2s, void* Params) {
    static DMA_HandleTypeDef hdma_i2sTx;
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable I2S3 clock */
    I2S3_CLK_ENABLE();

    /*** Configure the GPIOs ***/
    /* Enable I2S GPIO clocks */
    I2S3_SCK_SD_CLK_ENABLE();
    I2S3_WS_CLK_ENABLE();

    /* I2S3 pins configuration: WS, SCK and SD pins ----------------------------*/
    GPIO_InitStruct.Pin = I2S3_SCK_PIN | I2S3_SD_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = I2S3_SCK_SD_WS_AF;
    HAL_GPIO_Init(I2S3_SCK_SD_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = I2S3_WS_PIN;
    HAL_GPIO_Init(I2S3_WS_GPIO_PORT, &GPIO_InitStruct);

    /* I2S3 pins configuration: MCK pin */
    I2S3_MCK_CLK_ENABLE();
    GPIO_InitStruct.Pin = I2S3_MCK_PIN;
    HAL_GPIO_Init(I2S3_MCK_GPIO_PORT, &GPIO_InitStruct);

    /* Enable the I2S DMA clock */
    I2S3_DMAx_CLK_ENABLE();

    if (hi2s->Instance == I2S3) {
        /* Configure the hdma_i2sTx handle parameters */
        hdma_i2sTx.Init.Channel = I2S3_DMAx_CHANNEL;
        hdma_i2sTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_i2sTx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_i2sTx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_i2sTx.Init.PeriphDataAlignment = I2S3_DMAx_PERIPH_DATA_SIZE;
        hdma_i2sTx.Init.MemDataAlignment = I2S3_DMAx_MEM_DATA_SIZE;
        hdma_i2sTx.Init.Mode = DMA_NORMAL;
        hdma_i2sTx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_i2sTx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
        hdma_i2sTx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        hdma_i2sTx.Init.MemBurst = DMA_MBURST_SINGLE;
        hdma_i2sTx.Init.PeriphBurst = DMA_PBURST_SINGLE;

        hdma_i2sTx.Instance = I2S3_DMAx_STREAM;

        /* Associate the DMA handle */
        __HAL_LINKDMA(hi2s, hdmatx, hdma_i2sTx);

        /* Deinitialize the Stream for new transfer */
        HAL_DMA_DeInit(&hdma_i2sTx);

        /* Configure the DMA Stream */
        HAL_DMA_Init(&hdma_i2sTx);
    }

    /* I2S DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(I2S3_DMAx_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
    HAL_NVIC_EnableIRQ(I2S3_DMAx_IRQ);
}

/**
  * @brief  Clock Config.
  * @param  hi2s: might be required to set audio peripheral predivider if any.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application     
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
static void BSP_AUDIO_OUT_ClockConfig(I2S_HandleTypeDef* hi2s, uint32_t AudioFreq, void* Params) {
    RCC_PeriphCLKInitTypeDef rccclkinit;
    uint8_t index = 0, freqindex = 0xFF;

    for (index = 0; index < 8; index++)
        if (I2SFreq[index] == AudioFreq) {
            freqindex = index;
            break;
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

    HAL_I2S_Transmit_DMA(&hAudioOutI2s, AudioCurrentPos, transmit_size);

    AudioRemSizeBytes -= transmit_size * AUDIODATA_SIZE;
    AudioCurrentPos += transmit_size;
}

void BSP_AUDIO_OUT_TransferComplete_CallBack() {
    if (AudioRemSizeBytes > 0) {
        uint16_t transmit_size;
        if (AudioRemSizeBytes < (DMA_MAX_SZE * AUDIODATA_SIZE))
            transmit_size = AudioRemSizeBytes / AUDIODATA_SIZE;
        else
            transmit_size = DMA_MAX_SZE;

        HAL_I2S_Transmit_DMA(&hAudioOutI2s, AudioCurrentPos, transmit_size);

        AudioRemSizeBytes -= transmit_size * AUDIODATA_SIZE;
        AudioCurrentPos += transmit_size;
    } else {
        /* Call DMA Stop to disable DMA stream before stopping codec */
        //HAL_I2S_DMAStop(&hAudioOutI2s);

        /* reset to begining */
        BSP_AUDIO_OUT_Play(AudioPos, AudioSizeBytes);
    }
}

static void I2Cx_MspInit(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable I2C GPIO clocks */
    DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

    /* DISCOVERY_I2Cx SCL and SDA pins configuration ---------------------------*/
    GPIO_InitStruct.Pin = DISCOVERY_I2Cx_SCL_PIN | DISCOVERY_I2Cx_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = DISCOVERY_I2Cx_SCL_SDA_AF;
    HAL_GPIO_Init(DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, &GPIO_InitStruct);

    /* Enable the DISCOVERY_I2Cx peripheral clock */
    DISCOVERY_I2Cx_CLK_ENABLE();

    /* Force the I2C peripheral clock reset */
    DISCOVERY_I2Cx_FORCE_RESET();

    /* Release the I2C peripheral clock reset */
    DISCOVERY_I2Cx_RELEASE_RESET();

    /* Enable and set I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority(DISCOVERY_I2Cx_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_EV_IRQn);

    /* Enable and set I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority(DISCOVERY_I2Cx_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_ER_IRQn);
}

static void I2Cx_Init(void) {
    if (HAL_I2C_GetState(&I2cHandle) == HAL_I2C_STATE_RESET) {
        /* DISCOVERY_I2Cx peripheral configuration */
        I2cHandle.Init.ClockSpeed = BSP_I2C_SPEED;
        I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
        I2cHandle.Init.OwnAddress1 = 0x33;
        I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        I2cHandle.Instance = DISCOVERY_I2Cx;

        /* Init the I2C */
        I2Cx_MspInit();
        HAL_I2C_Init(&I2cHandle);
    }
}

static void I2Cx_Error(uint8_t Addr) {
    /* De-initialize the I2C communication bus */
    HAL_I2C_DeInit(&I2cHandle);

    /* Re-Initialize the I2C communication bus */
    I2Cx_Init();
}

static void I2Cx_WriteData(uint8_t Addr, uint8_t Reg, uint8_t Value) {
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Write(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout);

    /* Check the communication status */
    if (status != HAL_OK) {
        /* Execute user timeout callback */
        I2Cx_Error(Addr);
    }
}

bool tas3251_init(uint32_t i2c_device_addr) {
    uint32_t AudioFreq = 48000;

    /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
    BSP_AUDIO_OUT_ClockConfig(&hAudioOutI2s, AudioFreq, NULL);

    /* I2S data transfer preparation:
      Prepare the Media to be used for the audio transfer from memory to I2S peripheral */
    hAudioOutI2s.Instance = I2S3;
    if (HAL_I2S_GetState(&hAudioOutI2s) == HAL_I2S_STATE_RESET)
        /* Init the I2S MSP: this __weak function can be redefined by the application*/
        BSP_AUDIO_OUT_MspInit(&hAudioOutI2s, NULL);

    /* I2S data transfer preparation:
       Prepare the Media to be used for the audio transfer from memory to I2S peripheral */
    /* Configure the I2S peripheral */
    if (_i2s3_init(AudioFreq) != AUDIO_OK) return false;

    /* i2c commands to start the TAS32512 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    I2Cx_Init();

    /* startup sequence according to SLASEG6A – MAY 2018 – REVISED NOVEMBER 2018 §8.3.11.1 */

    /* 1. Apply power to DAC_DVDD, DAC_AVDD, GVDD_x, and PVDD_x */
    /* 2. Apply I2S or TDM clocks to the device to enable the internal system clocks */
    /* 3. Mute the left and right DAC channels */
    I2Cx_WriteData(i2c_device_addr, TAS3251_REG_PAGE, 0x00);
    HAL_Delay(1);
    I2Cx_WriteData(i2c_device_addr, TAS3251_REG_BOOK, 0x00);
    HAL_Delay(1);

    /* mute right and left */
    //I2Cx_WriteData(i2c_device_addr, TAS3251_REG_MUTE, TAS3251_MUTE_LEFT|TAS3251_MUTE_RIGHT);
    //HAL_Delay(1);

    /* standby */
    //I2Cx_WriteData(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_STANDBY);
    //uint8_t status = AUDIO_IO_Read(i2c_device_addr, TAS3251_REG_STANDBY);
    //HAL_Delay(1);

    /* enable */
    //I2Cx_WriteData(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_ENABLE);
    //HAL_Delay(1);

    /* 5.1. set digital volume */
    I2Cx_WriteData(i2c_device_addr, TAS3251_REG_LEFT_DIGITAL_VOLUME, (uint8_t)135);
    HAL_Delay(1);
    I2Cx_WriteData(i2c_device_addr, TAS3251_REG_RIGHT_DIGITAL_VOLUME, (uint8_t)135);
    HAL_Delay(1);

    /* dsp out of standby */
    //I2Cx_WriteData(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_DSPR);
    //HAL_Delay(1);

    /* enable */
    I2Cx_WriteData(i2c_device_addr, TAS3251_REG_STANDBY, TAS3251_STANDBY_ENABLE);
    HAL_Delay(1);

    /* unmute right and left */
    //I2Cx_WriteData(i2c_device_addr, TAS3251_REG_MUTE, 0);

    /* */
    BSP_AUDIO_OUT_Play((uint16_t*)((uint8_t*)waveformat + sizeof(WAVE_FormatTypeDef)), waveformat->FileSize);

    return true;
}

bool tas3251_set_output_freq(void) { return true; }

/*
void EXTI0_IRQHandler(void) { HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN); }
void EXTI1_IRQHandler(void) { HAL_GPIO_EXTI_IRQHandler(ACCELERO_INT2_PIN); }
void I2S2_IRQHandler(void) { HAL_DMA_IRQHandler(hAudioInI2s.hdmarx); }
*/

void I2S3_IRQHandler(void) { HAL_DMA_IRQHandler(hAudioOutI2s.hdmatx); }

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef* hi2s) {
    if (hi2s->Instance == I2S3) {
        /* Call the user function which will manage directly transfer complete */
        BSP_AUDIO_OUT_TransferComplete_CallBack();
    }
}

#if 0
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef* hi2s) {
    if (hi2s->Instance == I2S3) {
        /* Manage the remaining file size and new address offset: This function should
       be coded by user (its prototype is already declared in stm32f4_discovery_audio.h) */
        BSP_AUDIO_OUT_HalfTransfer_CallBack();
    }
}
#endif
