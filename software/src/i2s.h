#include <libopencm3/stm32/spi.h>

/*
HAL model

HAL_I2S_Init()
HAL_I2S_DeInit()
HAL_I2S_MspInit()
HAL_I2S_MspDeInit()

HAL_I2S_Transmit()
HAL_I2S_Receive()
HAL_I2S_Transmit_IT()
HAL_I2S_Receive_IT()
HAL_I2S_Transmit_DMA()
HAL_I2S_Receive_DMA()
HAL_I2S_DMAPause()
HAL_I2S_DMAResume()
HAL_I2S_DMAStop()
HAL_I2S_IRQHandler()
HAL_I2S_TxHalfCpltCallback()
HAL_I2S_TxCpltCallback()
HAL_I2S_RxHalfCpltCallback()
HAL_I2S_RxCpltCallback()
HAL_I2S_ErrorCallback()
HAL_I2S_GetState()
HAL_I2S_GetError()
*/

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

#define I2S3_DMAx_CLK_ENABLE() rcc_periph_clock_enable(RCC_DMA1)

/** @defgroup I2S_Audio_Frequency I2S Audio Frequency
  * @{
  */
#define I2S_AUDIOFREQ_192K (192000U)
#define I2S_AUDIOFREQ_96K (96000U)
#define I2S_AUDIOFREQ_48K (48000U)
#define I2S_AUDIOFREQ_44K (44100U)
#define I2S_AUDIOFREQ_32K (32000U)
#define I2S_AUDIOFREQ_22K (22050U)
#define I2S_AUDIOFREQ_16K (16000U)
#define I2S_AUDIOFREQ_11K (11025U)
#define I2S_AUDIOFREQ_8K (8000U)
#define I2S_AUDIOFREQ_DEFAULT (2U)

/** @defgroup I2S_Data_Format I2S Data Format
  * @{
  */
#define I2S_DATAFORMAT_16B (0x00000000U)
#define I2S_DATAFORMAT_16B_EXTENDED (SPI_I2SCFGR_CHLEN)
#define I2S_DATAFORMAT_24B ((SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN_0))
#define I2S_DATAFORMAT_32B ((SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN_1))

/** @defgroup I2S_Standard I2S Standard
  * @{
  */
#define I2S_STANDARD_PHILIPS (0x00000000U)
#define I2S_STANDARD_MSB (SPI_I2SCFGR_I2SSTD_0)
#define I2S_STANDARD_LSB (SPI_I2SCFGR_I2SSTD_1)
#define I2S_STANDARD_PCM_SHORT ((SPI_I2SCFGR_I2SSTD_0 | SPI_I2SCFGR_I2SSTD_1))
#define I2S_STANDARD_PCM_LONG ((SPI_I2SCFGR_I2SSTD_0 | SPI_I2SCFGR_I2SSTD_1 | SPI_I2SCFGR_PCMSYNC))

/** @defgroup I2S_MCLK_Output I2S MCLK Output
  * @{
  */
#define I2S_MCLKOUTPUT_ENABLE (SPI_I2SPR_MCKOE)
#define I2S_MCLKOUTPUT_DISABLE (0x00000000U)

/** @defgroup I2S_Mode I2S Mode
  * @{
  */
#define I2S_MODE_SLAVE_TX (0x00000000U)
#define I2S_MODE_SLAVE_RX (SPI_I2SCFGR_I2SCFG_0)
#define I2S_MODE_MASTER_TX (SPI_I2SCFGR_I2SCFG_1)
#define I2S_MODE_MASTER_RX ((SPI_I2SCFGR_I2SCFG_0 | SPI_I2SCFGR_I2SCFG_1))

/** @defgroup I2S_Data_Format I2S Data Format
  * @{
  */
#define I2S_DATAFORMAT_16B (0x00000000U)
#define I2S_DATAFORMAT_16B_EXTENDED (SPI_I2SCFGR_CHLEN)
#define I2S_DATAFORMAT_24B ((SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN_0))
#define I2S_DATAFORMAT_32B ((SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN_1))

/** @defgroup I2S_Clock_Polarity I2S Clock Polarity
  * @{
  */
#define I2S_CPOL_LOW (0x00000000U)
#define I2S_CPOL_HIGH (SPI_I2SCFGR_CKPOL)

/** @defgroup I2S_FullDuplex_Mode I2S FullDuplex Mode
  * @{
  */
#define I2S_FULLDUPLEXMODE_DISABLE (0x00000000U)
#define I2S_FULLDUPLEXMODE_ENABLE (0x00000001U)

#define SPI_I2SCFGR_I2SCFG_Pos (8U)
#define SPI_I2SCFGR_I2SCFG_Msk (0x3UL << SPI_I2SCFGR_I2SCFG_Pos) /*!< 0x00000300 */
#define SPI_I2SCFGR_I2SCFG SPI_I2SCFGR_I2SCFG_Msk /*!<I2SCFG[1:0] bits (I2S configuration mode) */
#define SPI_I2SCFGR_I2SCFG_0 (0x1UL << SPI_I2SCFGR_I2SCFG_Pos) /*!< 0x00000100 */
#define SPI_I2SCFGR_I2SCFG_1 (0x2UL << SPI_I2SCFGR_I2SCFG_Pos) /*!< 0x00000200 */

#define SPI_I2SCFGR_ASTRTEN_Pos (12U)
#define SPI_I2SCFGR_ASTRTEN_Msk (0x1UL << SPI_I2SCFGR_ASTRTEN_Pos) /*!< 0x00001000 */
#define SPI_I2SCFGR_ASTRTEN SPI_I2SCFGR_ASTRTEN_Msk                /*!<Asynchronous start enable */

#define I2S_CLOCK_PLL (0x00000000U)  //lru: good or not ?

#define SPI_I2SCFGR_I2SSTD_Pos (4U)
#define SPI_I2SCFGR_I2SSTD_Msk (0x3UL << SPI_I2SCFGR_I2SSTD_Pos) /*!< 0x00000030 */
#define SPI_I2SCFGR_I2SSTD SPI_I2SCFGR_I2SSTD_Msk /*!<I2SSTD[1:0] bits (I2S standard selection) */
#define SPI_I2SCFGR_I2SSTD_0 (0x1UL << SPI_I2SCFGR_I2SSTD_Pos) /*!< 0x00000010 */
#define SPI_I2SCFGR_I2SSTD_1 (0x2UL << SPI_I2SCFGR_I2SSTD_Pos) /*!< 0x00000020 */

bool I2S_Init(void);
void BSP_AUDIO_OUT_ClockConfig(uint32_t AudioFreq);
void BSP_AUDIO_OUT_MspInit(void);