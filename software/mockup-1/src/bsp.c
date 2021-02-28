#include <stm32f4xx_hal.h>

#include "bsp.h"
#include "usb_audio.h"
#include "usbd_audio_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#define LED4_PIN GPIO_PIN_12
#define LED4_GPIO_PORT GPIOD
#define LED4_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED4_GPIO_CLK_DISABLE() __HAL_RCC_GPIOD_CLK_DISABLE()

#define LED3_PIN GPIO_PIN_13
#define LED3_GPIO_PORT GPIOD
#define LED3_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE() __HAL_RCC_GPIOD_CLK_DISABLE()

#define LED5_PIN GPIO_PIN_14
#define LED5_GPIO_PORT GPIOD
#define LED5_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED5_GPIO_CLK_DISABLE() __HAL_RCC_GPIOD_CLK_DISABLE()

#define LED6_PIN GPIO_PIN_15
#define LED6_GPIO_PORT GPIOD
#define LED6_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED6_GPIO_CLK_DISABLE() __HAL_RCC_GPIOD_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__) \
    do {                                \
        if ((__INDEX__) == 0)           \
            LED4_GPIO_CLK_ENABLE();     \
        else if ((__INDEX__) == 1)      \
            LED3_GPIO_CLK_ENABLE();     \
        else if ((__INDEX__) == 2)      \
            LED5_GPIO_CLK_ENABLE();     \
        else if ((__INDEX__) == 3)      \
            LED6_GPIO_CLK_ENABLE();     \
    } while (0)

#define LEDx_GPIO_CLK_DISABLE(__INDEX__) \
    do {                                 \
        if ((__INDEX__) == 0)            \
            LED4_GPIO_CLK_DISABLE();     \
        else if ((__INDEX__) == 1)       \
            LED3_GPIO_CLK_DISABLE();     \
        else if ((__INDEX__) == 2)       \
            LED5_GPIO_CLK_DISABLE();     \
        else if ((__INDEX__) == 3)       \
            LED6_GPIO_CLK_DISABLE();     \
    } while (0)

typedef enum {
    LED4 = 0,
    GREEN = LED4,
    LED3 = 1,
    ORANGE = LED3,
    LED5 = 2,
    RED = LED5,
    LED6 = 3,
    BLUE = LED6
} Led_TypeDef;

GPIO_TypeDef* GPIO_PORT[] = {LED4_GPIO_PORT, LED3_GPIO_PORT, LED5_GPIO_PORT, LED6_GPIO_PORT};
const uint16_t GPIO_PIN[] = {LED4_PIN, LED3_PIN, LED5_PIN, LED6_PIN};

void _bsp_led_init(Led_TypeDef Led) {
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable the GPIO_LED Clock */
    LEDx_GPIO_CLK_ENABLE(Led);

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = GPIO_PIN[Led];
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIO_PORT[Led], &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

static void _bsp_led_on(Led_TypeDef Led) { HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET); }
static void _bsp_led_toggle(Led_TypeDef Led) { HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]); }
static void _bsp_led_off(Led_TypeDef Led) { HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET); }

static void _system_clock_config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType =
        (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
    if (HAL_GetREVID() == 0x1001) {
        /* Enable the Flash prefetch */
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    }
}

/* CS43L22 (the STM32F4-Disco audio codec) reset pin */
static bool _cs43l22_shutdown(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Audio reset pin configuration */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* Power Down the codec */
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);

    /* Wait for a delay to insure registers erasing */
    HAL_Delay(5);

    return true;
}

bool bsp_init(void) {
    if (HAL_Init() != HAL_OK) return false;
    _system_clock_config();

    /* switch off the codec cs43l22 */
    return _cs43l22_shutdown();
}

bool mmi_init() {
    _bsp_led_init(ORANGE);
    _bsp_led_init(GREEN);
    _bsp_led_init(RED);
    _bsp_led_init(BLUE);

    _bsp_led_off(ORANGE);
    _bsp_led_off(GREEN);
    _bsp_led_off(RED);
    _bsp_led_on(BLUE);

    return true;
}

void mmi_heartbeat() {
    _bsp_led_toggle(BLUE);
    HAL_Delay(1000);
}

USBD_HandleTypeDef USBD_Device;

bool usb_init(void) {
    /* Init Device Library,Add Supported Class and Start the library*/
    USBD_Init(&USBD_Device, &AUDIO_Desc, DEVICE_FS);

    USBD_RegisterClass(&USBD_Device, &USBD_AUDIO);

    USBD_AUDIO_RegisterInterface(&USBD_Device, &USBD_AUDIO_fops_FS);

    USBD_Start(&USBD_Device);

    return true;
}

bool usb_start(void) {
    /* Start Device Process */
    return USBD_Start(&USBD_Device) == USBD_OK;
}

void fault() {
    _bsp_led_off(BLUE);
    while (true) {
        _bsp_led_toggle(RED);
        HAL_Delay(1000);
    }
}

void Error_Handler(void) { fault(); }
void USBD_error_handler(void) { fault(); }

void NMI_Handler(void) {}

void HardFault_Handler(void) {
    for (;;)
        ;
}

void MemManage_Handler(void) {
    for (;;)
        ;
}

void BusFault_Handler(void) {
    for (;;)
        ;
}

void UsageFault_Handler(void) {
    for (;;)
        ;
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void) { HAL_IncTick(); }

char* _sbrk(__attribute__((unused)) int incr) { return (char*)0; }

/** I2C
 */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define I2Cx_TIMEOUT_MAX 0x1000 /*<! The value of the maximal timeout for BUS waiting loops */

#define DISCOVERY_I2Cx_FORCE_RESET() __HAL_RCC_I2C1_FORCE_RESET()
#define DISCOVERY_I2Cx_RELEASE_RESET() __HAL_RCC_I2C1_RELEASE_RESET()

static I2C_HandleTypeDef _i2c_handle;
static uint32_t I2cxTimeout = I2Cx_TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */

static void _i2c_msp_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();

    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0); /* highest priority */
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0); /* highest priority */
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

static void _i2c_error() {
    HAL_I2C_DeInit(&_i2c_handle);
    i2c_init();
}

void i2c_init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    if (HAL_I2C_GetState(&_i2c_handle) == HAL_I2C_STATE_RESET) {
        _i2c_handle.Init.ClockSpeed = 100000;
        _i2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
        _i2c_handle.Init.OwnAddress1 = 0x33;
        _i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        _i2c_handle.Instance = I2C1;

        _i2c_msp_init();
        HAL_I2C_Init(&_i2c_handle);
    }
}

void i2c_write(uint16_t device_addr, uint16_t memaddr, uint8_t value) {
    if (HAL_I2C_Mem_Write(&_i2c_handle, device_addr, memaddr, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout) != HAL_OK)
        _i2c_error();
}

void i2c_burst_write(uint16_t device_addr, uint16_t memaddr, uint8_t value[], uint16_t size) {
    if (HAL_I2C_Mem_Write(&_i2c_handle, device_addr, memaddr, I2C_MEMADD_SIZE_8BIT, value, size, I2cxTimeout) != HAL_OK)
        _i2c_error();
}

uint8_t i2c_read(uint16_t device_addr, uint16_t memaddr)
{
    uint8_t value = 0;
    if(HAL_I2C_Mem_Read(&_i2c_handle, device_addr, memaddr, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout) != HAL_OK)
        _i2c_error();

    return value;
}

/** i2s
 */

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

static I2S_HandleTypeDef hAudioOutI2s;
static void (*_i2s_transfer_complete_dma)();

/* These PLL parameters are valid when the f(VCO clock) = 1Mhz */
static const uint32_t I2SFreq[8] = {8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000};
static const uint32_t I2SPLLN[8] = {256, 429, 213, 429, 426, 271, 258, 344};
static const uint32_t I2SPLLR[8] = {5, 4, 4, 4, 4, 6, 3, 1};

/**
  * @brief  Clock Config.
  * @param  hi2s: might be required to set audio peripheral predivider if any.
  * @param  audio_freq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application     
  * @param  params : pointer on additional configuration parameters, can be NULL.
  */
static void _i2s_clk_config(I2S_HandleTypeDef* hi2s, uint32_t audio_freq, void* params) {
    RCC_PeriphCLKInitTypeDef rccclkinit;
    uint8_t index = 0, freqindex = 0xFF;

    for (index = 0; index < 8; index++)
        if (I2SFreq[index] == audio_freq) {
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

/**
  * @brief  AUDIO OUT I2S MSP Init.
  * @param  hi2s: might be required to set audio peripheral predivider if any.
  * @param  params : pointer on additional configuration parameters, can be NULL.
  */
static void _i2s_pin_dma_config(I2S_HandleTypeDef* hi2s, void* params) {
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
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH; 
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

bool i2s_init(uint32_t audio_freq, void (*transfer_complete_dma)()) {
    _i2s_transfer_complete_dma = transfer_complete_dma;

    _i2s_clk_config(&hAudioOutI2s, audio_freq, NULL);

    hAudioOutI2s.Instance = I2S3;
    if (HAL_I2S_GetState(&hAudioOutI2s) == HAL_I2S_STATE_RESET)
        _i2s_pin_dma_config(&hAudioOutI2s, NULL);

    __HAL_I2S_DISABLE(&hAudioOutI2s);

    /* I2S3 peripheral configuration */
    hAudioOutI2s.Init.AudioFreq = audio_freq;
    hAudioOutI2s.Init.ClockSource = I2S_CLOCK_PLL;
    hAudioOutI2s.Init.CPOL = I2S_CPOL_LOW;
    hAudioOutI2s.Init.DataFormat = I2S_DATAFORMAT_16B;
    hAudioOutI2s.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hAudioOutI2s.Init.Mode = I2S_MODE_MASTER_TX;
    hAudioOutI2s.Init.Standard = I2S_STANDARD_PHILIPS;

    /* Initialize the I2S peripheral with the structure above */
    if (HAL_I2S_Init(&hAudioOutI2s) != HAL_OK) {
        return false;
    } else {
        return true;
    }
}

void i2s_transmit_dma(uint16_t* audio_current_pos, uint16_t transmit_size) {
    HAL_I2S_Transmit_DMA(&hAudioOutI2s, audio_current_pos, transmit_size);
}

void i2s_stop_dma() {
    HAL_I2S_DMAStop(&hAudioOutI2s);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef* hi2s) {
    if (hi2s->Instance == I2S3) {
        /* Call the user function which will manage directly transfer complete */
        (*_i2s_transfer_complete_dma)();
    }
}

/*
void EXTI0_IRQHandler(void) { HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN); }
void EXTI1_IRQHandler(void) { HAL_GPIO_EXTI_IRQHandler(ACCELERO_INT2_PIN); }
void I2S2_IRQHandler(void) { HAL_DMA_IRQHandler(hAudioInI2s.hdmarx); }
*/

void I2S3_IRQHandler(void) { HAL_DMA_IRQHandler(hAudioOutI2s.hdmatx); }

#if 0
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef* hi2s) {
    if (hi2s->Instance == I2S3) {
        /* Manage the remaining file size and new address offset: This function should
       be coded by user (its prototype is already declared in stm32f4_discovery_audio.h) */
        BSP_AUDIO_OUT_HalfTransfer_CallBack();
    }
}
#endif
