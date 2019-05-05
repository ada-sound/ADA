#include <stm32f4xx_hal.h>
#include "bsp.h"

#define LEDn 4

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

/* CS43L22 (the STM32F4-Disco audio codec) reset pin */
#define CS43L22_RESET_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define CS43L22_RESET_PIN GPIO_PIN_4
#define CS43L22_RESET_GPIO GPIOD

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

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED4_GPIO_PORT, LED3_GPIO_PORT, LED5_GPIO_PORT, LED6_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED4_PIN, LED3_PIN, LED5_PIN, LED6_PIN};

/* */
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

static void _bsp_led_on(Led_TypeDef Led) {
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

static void _bsp_led_off(Led_TypeDef Led) {
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

static void _system_clock_config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
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

static bool _cs43l22_shutdown(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Audio reset pin configuration */
    GPIO_InitStruct.Pin = CS43L22_RESET_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(CS43L22_RESET_GPIO, &GPIO_InitStruct);

    /* Power Down the codec */
    HAL_GPIO_WritePin(CS43L22_RESET_GPIO, CS43L22_RESET_PIN, GPIO_PIN_RESET);  //lru

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

void fault() {
    _bsp_led_on(RED);
    for (;;)
        ;
}

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
