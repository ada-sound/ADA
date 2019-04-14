#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "i2s.h"
#include "tas3251.h"

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

#define I2S3_MCK_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define __HAL_RCC_GPIOC_CLK_ENABLE()                          \
    do {                                                      \
        __IO uint32_t tmpreg = 0x00U;                         \
        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);           \
        /* Delay after an RCC peripheral clock enabling */    \
        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); \
        UNUSED(tmpreg);                                       \
    } while (0U)

__weak void BSP_AUDIO_OUT_MspInit(I2S_HandleTypeDef *hi2s, void *Params)
{
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
}
#endif

#if 0
#define __HAL_RCC_SPI3_CLK_ENABLE()                          \
    do {                                                     \
        __IO uint32_t tmpreg = 0x00U;                        \
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI3EN);           \
        /* Delay after an RCC peripheral clock enabling */   \
        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI3EN); \
        UNUSED(tmpreg);                                      \
    } while (0U)
#define I2S3_CLK_ENABLE() __HAL_RCC_SPI3_CLK_ENABLE()
#endif

#if 0
static uint8_t I2S3_Init(uint32_t AudioFreq)
{
  /* Initialize the hAudioOutI2s Instance parameter */
  hAudioOutI2s.Instance         = I2S3;

 /* Disable I2S block */
  __HAL_I2S_DISABLE(&hAudioOutI2s);
  
  /* I2S3 peripheral configuration */
  hAudioOutI2s.Init.AudioFreq   = AudioFreq;
  hAudioOutI2s.Init.ClockSource = I2S_CLOCK_PLL;
  hAudioOutI2s.Init.CPOL        = I2S_CPOL_LOW;
  hAudioOutI2s.Init.DataFormat  = I2S_DATAFORMAT_16B;
  hAudioOutI2s.Init.MCLKOutput  = I2S_MCLKOUTPUT_ENABLE;
  hAudioOutI2s.Init.Mode        = I2S_MODE_MASTER_TX;
  hAudioOutI2s.Init.Standard    = I2S_STANDARD;
  /* Initialize the I2S peripheral with the structure above */  
  if(HAL_I2S_Init(&hAudioOutI2s) != HAL_OK)
  {
    return AUDIO_ERROR;
  }
  else
  {
    return AUDIO_OK;
  }
}
#endif

#define I2S3_CLK_ENABLE() rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI3EN)

#define I2S3_SCK_SD_CLK_ENABLE() rcc_periph_clock_enable(RCC_GPIOC)
#define I2S3_SCK_SD_GPIO_PORT GPIOC
#define I2S3_SCK_PIN GPIO10
#define I2S3_SD_PIN GPIO12
#define I2S3_SCK_SD_WS_AF GPIO_AF6

#define I2S3_WS_CLK_ENABLE() rcc_periph_clock_enable(RCC_GPIOA)
#define I2S3_WS_PIN GPIO4
#define I2S3_WS_GPIO_PORT GPIOA

#define I2S3_MCK_CLK_ENABLE() rcc_periph_clock_enable(RCC_GPIOC)
#define I2S3_MCK_GPIO_PORT GPIOC
#define I2S3_MCK_PIN GPIO7

bool tas3251_init(void) {
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
    //I2S3_DMAx_CLK_ENABLE();

    /**
     * I2S3_Init
     */

    /*----------------------- SPIx I2SCFGR & I2SPR Configuration ----------------*/
    /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
    /*CLEAR_BIT(hi2s->Instance->I2SCFGR,
              (SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CKPOL | SPI_I2SCFGR_I2SSTD |
               SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG | SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD));
    hi2s->Instance->I2SPR = 0x0002U;*/

    return true;
}

bool tas3251_set_output_freq(void) { return true; }
