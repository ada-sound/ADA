#if 0
typedef enum 
{
  LED4 = 0,
  LED3 = 1,
  LED5 = 2,
  LED6 = 3
} Led_TypeDef;

void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

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

void BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET); 
}

void cs43l22_Shutdown(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Audio reset pin configuration */
  #define AUDIO_RESET_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
  #define AUDIO_RESET_PIN                       GPIO_PIN_4
  #define AUDIO_RESET_GPIO                      GPIOD
  GPIO_InitStruct.Pin = AUDIO_RESET_PIN; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(AUDIO_RESET_GPIO, &GPIO_InitStruct);    

  /* Power Down the codec */
  HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_RESET);//lru
  
  /* Wait for a delay to insure registers erasing */
  HAL_Delay(5); 
}
#endif

#include <stdbool.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <stm32f4_discovery_bsp.h>

#define LITTLE_BIT 800000

int main(void) {
    bool toggle = false;

    led_init(led_red);
    led_init(led_green);
    led_init(led_orange);
    led_init(led_blue);

    led_on(led_red);
    led_on(led_green);
    led_on(led_orange);
    led_on(led_blue);

    /* Power Down the codec CS43L22 */
    //cs43l22_Shutdown();

    while(1) {
        for (int i = 0; i < LITTLE_BIT; i++)
            __asm__("nop");

        if (toggle=!toggle)
            led_off(led_blue);
        else
            led_on(led_blue);
    }
}
