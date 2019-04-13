#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <stm32f4_discovery_bsp.h>

static const struct { uint32_t port; uint8_t mode; uint8_t pull_up_down; uint16_t gpios; enum rcc_periph_clken clken; }
    _leds[led_max] = {
        {GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12, RCC_GPIOD},
        {GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13, RCC_GPIOD},
        {GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14, RCC_GPIOD},
        {GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15, RCC_GPIOD}};

/* */
void led_init(led_t led)
{
    rcc_periph_clock_enable(_leds[led].clken);
    gpio_mode_setup(_leds[led].port, _leds[led].mode, _leds[led].pull_up_down, _leds[led].gpios);
}

void led_on(led_t led)
{
    gpio_set(_leds[led].port, _leds[led].gpios);
}

void led_off(led_t led)
{
    gpio_clear(_leds[led].port, _leds[led].gpios);
}

void cs43l22_Shutdown(void)
{

}
