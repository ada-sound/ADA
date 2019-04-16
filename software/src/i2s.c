#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

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

static const uint32_t Mode = I2S_MODE_MASTER_TX;
static const uint32_t Standard = I2S_STANDARD_PHILIPS;
static const uint32_t DataFormat = I2S_DATAFORMAT_16B;
static const uint32_t MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
static const uint32_t AudioFreq = I2S_AUDIOFREQ_DEFAULT;
static const uint32_t CPOL = I2S_CPOL_LOW;
static const uint32_t ClockSource = I2S_CLOCK_PLL;
static const uint32_t FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

/**
  * @brief  Clock Config.
  * @param  hi2s: might be required to set audio peripheral predivider if any.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application     
  */
void BSP_AUDIO_OUT_ClockConfig(uint32_t AudioFreq) {
    uint8_t index = 0, freqindex;
    bool found = false;

    for (index = 0; index < 8; index++) {
        if (_I2S_freq[index].freq == AudioFreq) {
            freqindex = index;
            found = true;
            break;
        }
    }

    if (found) {
        /* I2S clock config 
        PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) � (PLLI2SN/PLLM)
        I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */

/* Alias word address of PLLI2SON bit */
#define RCC_PLLI2SON_BIT_NUMBER 0x1AU
#define RCC_CR_PLLI2SON_BB (PERIPH_BB_BASE + (RCC_CR_OFFSET * 32U) + (RCC_PLLI2SON_BIT_NUMBER * 4U))

#define __HAL_RCC_PLLI2S_ENABLE() (*(__IO uint32_t *)RCC_CR_PLLI2SON_BB = ENABLE)
#define __HAL_RCC_PLLI2S_DISABLE() (*(__IO uint32_t *)RCC_CR_PLLI2SON_BB = DISABLE)

        /* Disable the PLLI2S */
        rcc_osc_off(RCC_PLLI2S);

        /* Configure the PLLI2S division factors */
        /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) * (PLLI2SN/PLLM) */
        /* I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */
#define __HAL_RCC_PLLI2S_CONFIG(__PLLI2SN__, __PLLI2SR__)                \
    (RCC_PLLI2SCFGR = (((__PLLI2SN__) << RCC_PLLI2SCFGR_PLLI2SN_SHIFT) | \
                       ((__PLLI2SR__) << RCC_PLLI2SCFGR_PLLI2SR_SHIFT)))
        __HAL_RCC_PLLI2S_CONFIG(_I2S_freq[freqindex].plln, _I2S_freq[freqindex].pllr);

        /* Enable the PLLI2S */
        rcc_osc_off(RCC_PLLI2S);
    }
}

void BSP_AUDIO_OUT_MspInit() {
#if 0
    static DMA_HandleTypeDef hdma_i2sTx;
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable I2S3 clock */
    I2S3_CLK_ENABLE();

    /*** Configure the GPIOs ***/  
    /* Enable I2S GPIO clocks */
    I2S3_SCK_SD_CLK_ENABLE();
    I2S3_WS_CLK_ENABLE();

    /* I2S3 pins configuration: WS, SCK and SD pins ----------------------------*/
    GPIO_InitStruct.Pin         = I2S3_SCK_PIN | I2S3_SD_PIN; 
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull        = GPIO_NOPULL;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate   = I2S3_SCK_SD_WS_AF;
    HAL_GPIO_Init(I2S3_SCK_SD_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin         = I2S3_WS_PIN ;
    HAL_GPIO_Init(I2S3_WS_GPIO_PORT, &GPIO_InitStruct); 

    /* I2S3 pins configuration: MCK pin */
    I2S3_MCK_CLK_ENABLE();
    GPIO_InitStruct.Pin         = I2S3_MCK_PIN; 
    HAL_GPIO_Init(I2S3_MCK_GPIO_PORT, &GPIO_InitStruct);   

    /* Enable the I2S DMA clock */
    I2S3_DMAx_CLK_ENABLE(); 

    if(hi2s->Instance == I2S3)
    {
    /* Configure the hdma_i2sTx handle parameters */   
    hdma_i2sTx.Init.Channel             = I2S3_DMAx_CHANNEL;  
    hdma_i2sTx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_i2sTx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_i2sTx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_i2sTx.Init.PeriphDataAlignment = I2S3_DMAx_PERIPH_DATA_SIZE;
    hdma_i2sTx.Init.MemDataAlignment    = I2S3_DMAx_MEM_DATA_SIZE;
    hdma_i2sTx.Init.Mode                = DMA_NORMAL;
    hdma_i2sTx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_i2sTx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;         
    hdma_i2sTx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_i2sTx.Init.MemBurst            = DMA_MBURST_SINGLE;
    hdma_i2sTx.Init.PeriphBurst         = DMA_PBURST_SINGLE; 

    hdma_i2sTx.Instance                 = I2S3_DMAx_STREAM;

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
#endif
}

bool I2S_Init() {
    uint32_t packetlength;
    uint32_t i2sclk;

    /* Enable I2S3 clock */
    I2S3_CLK_ENABLE();

    /* I2S3 SCK, SD pins config */
    I2S3_SCK_SD_CLK_ENABLE();
    gpio_mode_setup(I2S3_SCK_SD_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                    I2S3_SCK_PIN | I2S3_SD_PIN);
    gpio_set_output_options(I2S3_SCK_SD_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                            I2S3_SCK_PIN | I2S3_SD_PIN);
    gpio_set_af(I2S3_SCK_SD_GPIO_PORT, I2S3_SCK_SD_WS_AF, I2S3_SCK_PIN | I2S3_SD_PIN);

    /* I2S3 WS pin config */
    I2S3_WS_CLK_ENABLE();
    gpio_mode_setup(I2S3_WS_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2S3_WS_PIN);
    gpio_set_output_options(I2S3_WS_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2S3_WS_PIN);
    gpio_set_af(I2S3_WS_GPIO_PORT, I2S3_SCK_SD_WS_AF, I2S3_WS_PIN);

    /* I2S3 MCK pin config */
    I2S3_MCK_CLK_ENABLE();
    gpio_mode_setup(I2S3_MCK_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2S3_MCK_PIN);
    gpio_set_output_options(I2S3_MCK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2S3_MCK_PIN);
    gpio_set_af(I2S3_MCK_GPIO_PORT, I2S3_SCK_SD_WS_AF, I2S3_MCK_PIN);

    /* Enable the I2S DMA clock */
    I2S3_DMAx_CLK_ENABLE();

    /**
     * I2S3_Init
     */

    /*----------------------- SPIx I2SCFGR & I2SPR Configuration ----------------*/
    /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
    SPI2_I2SCFGR &=
        ~(SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN_LSB | SPI_I2SCFGR_CKPOL | SPI_I2SCFGR_I2SSTD_LSB |
          SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG_LSB | SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD);

    /*----------------------- I2SPR: I2SDIV and ODD Calculation -----------------*/
    /* If the requested audio frequency is not the default, compute the prescaler */
#if 0
    if (AudioFreq != I2S_AUDIOFREQ_DEFAULT) {
        /* Check the frame length (For the Prescaler computing) ********************/
        if (DataFormat == I2S_DATAFORMAT_16B) {
            /* Packet length is 16 bits */
            packetlength = 16U;
        } else {
            /* Packet length is 32 bits */
            packetlength = 32U;
        }

        /* I2S standard */
        if (Standard <= I2S_STANDARD_LSB) {
            /* In I2S standard packet lenght is multiplied by 2 */
            packetlength = packetlength * 2U;
        }

        /* Get the source clock value **********************************************/
#if defined(I2S_APB1_APB2_FEATURE)
        if (IS_I2S_APB1_INSTANCE(hi2s->Instance)) {
            i2sclk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2S_APB1);
        } else {
            i2sclk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2S_APB2);
        }
#else
        i2sclk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2S);
#endif

        /* Compute the Real divider depending on the MCLK output state, with a floating point */
        if (hi2s->Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE) {
            /* MCLK output is enabled */
            if (hi2s->Init.DataFormat != I2S_DATAFORMAT_16B) {
                tmp = (uint32_t)(((((i2sclk / (packetlength * 4U)) * 10U) / hi2s->Init.AudioFreq)) +
                                 5U);
            } else {
                tmp = (uint32_t)(((((i2sclk / (packetlength * 8U)) * 10U) / hi2s->Init.AudioFreq)) +
                                 5U);
            }
        } else {
            /* MCLK output is disabled */
            tmp = (uint32_t)(((((i2sclk / packetlength) * 10U) / hi2s->Init.AudioFreq)) + 5U);
        }

        /* Remove the flatting point */
        tmp = tmp / 10U;

        /* Check the parity of the divider */
        i2sodd = (uint32_t)(tmp & (uint32_t)1U);

        /* Compute the i2sdiv prescaler */
        i2sdiv = (uint32_t)((tmp - i2sodd) / 2U);

        /* Get the Mask for the Odd bit (SPI_I2SPR[8]) register */
        i2sodd = (uint32_t)(i2sodd << 8U);
    } else {
        /* Set the default values */
        i2sdiv = 2U;
        i2sodd = 0U;
    }

    /* Test if the divider is 1 or 0 or greater than 0xFF */
    if ((i2sdiv < 2U) || (i2sdiv > 0xFFU)) {
        /* Set the error code and execute error callback*/
        SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_PRESCALER);
        return HAL_ERROR;
    }
#endif

    /*----------------------- SPIx I2SCFGR & I2SPR Configuration ----------------*/

    /* Write to SPIx I2SPR register the computed value */
    //found with project STM32Cube_FW_F4_V1.24.0/Projects/STM32F4-Discovery/Examples/BSP
    //i2sclk = 86000000;
    //tmp=7;
    //i2sodd=1;
    //i2sdiv=3;
    uint32_t i2sdiv = 3;
    uint32_t i2sodd = 1;
    SPI2_I2SPR = (uint32_t)i2sdiv | (uint32_t)i2sodd | (uint32_t)MCLKOutput;

    /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
    /* And configure the I2S with the I2S_InitStruct values                      */
    SPI2_I2SCFGR &=
        ~(SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN_LSB | SPI_I2SCFGR_CKPOL | SPI_I2SCFGR_I2SSTD_LSB |
          SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG_LSB | SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD);

    SPI2_I2SCFGR |= SPI_I2SCFGR_I2SMOD | Mode | Standard | DataFormat | CPOL;

    if (Standard == I2S_STANDARD_PCM_SHORT || Standard == I2S_STANDARD_PCM_LONG)
        SPI2_I2SCFGR |= SPI_I2SCFGR_ASTRTEN;

#if 0
#if defined(SPI_I2S_FULLDUPLEX_SUPPORT)

    /* Configure the I2S extended if the full duplex mode is enabled */
    assert_param(IS_I2S_FULLDUPLEX_MODE(hi2s->Init.FullDuplexMode));

    if (hi2s->Init.FullDuplexMode == I2S_FULLDUPLEXMODE_ENABLE) {
        /* Set FullDuplex I2S IrqHandler ISR if FULLDUPLEXMODE is enabled */
        hi2s->IrqHandlerISR = HAL_I2SEx_FullDuplex_IRQHandler;

        /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
        CLEAR_BIT(
            I2SxEXT(hi2s->Instance)->I2SCFGR,
            (SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CKPOL | SPI_I2SCFGR_I2SSTD |
             SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG | SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD));
        I2SxEXT(hi2s->Instance)->I2SPR = 2U;

        /* Get the I2SCFGR register value */
        tmpreg = I2SxEXT(hi2s->Instance)->I2SCFGR;

        /* Get the mode to be configured for the extended I2S */
        if ((hi2s->Init.Mode == I2S_MODE_MASTER_TX) || (hi2s->Init.Mode == I2S_MODE_SLAVE_TX)) {
            tmp = I2S_MODE_SLAVE_RX;
        } else /* I2S_MODE_MASTER_RX ||  I2S_MODE_SLAVE_RX */
        {
            tmp = I2S_MODE_SLAVE_TX;
        }

        /* Configure the I2S Slave with the I2S Master parameter values */
        tmpreg |= (uint16_t)((uint16_t)SPI_I2SCFGR_I2SMOD |
                             (uint16_t)(tmp | (uint16_t)(hi2s->Init.Standard |
                                                         (uint16_t)(hi2s->Init.DataFormat |
                                                                    (uint16_t)hi2s->Init.CPOL))));

        /* Write to SPIx I2SCFGR */
        WRITE_REG(I2SxEXT(hi2s->Instance)->I2SCFGR, tmpreg);
    }
#endif /* SPI_I2S_FULLDUPLEX_SUPPORT */
#endif
}