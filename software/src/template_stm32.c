#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LITTLE_BIT 800000
int main(void) {
        rcc_periph_clock_enable(RCC_GPIOD);
        gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	gpio_set(GPIOD, GPIO12);

	while(1) {
		/* wait a little bit */
		for (int i = 0; i < 5 * LITTLE_BIT; i++) {
			__asm__("nop");
		}
		gpio_toggle(GPIOD, GPIO12);
	}
}
