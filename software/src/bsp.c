#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <bsp.h>

static const struct {
    uint32_t port;
    uint8_t mode;
    uint8_t pull_up_down;
    uint16_t gpios;
    enum rcc_periph_clken clken;
} _leds[led_max] = {{GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12, RCC_GPIOD},
                    {GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13, RCC_GPIOD},
                    {GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14, RCC_GPIOD},
                    {GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15, RCC_GPIOD}};

/* */
void led_init(led_t led) {
    rcc_periph_clock_enable(_leds[led].clken);
    gpio_mode_setup(_leds[led].port, _leds[led].mode, _leds[led].pull_up_down, _leds[led].gpios);
}

void led_on(led_t led) { gpio_set(_leds[led].port, _leds[led].gpios); }

void led_off(led_t led) { gpio_clear(_leds[led].port, _leds[led].gpios); }

#define STM32_CYCLES_PER_LOOP 3
#define STM32_LOST_CYCLES_ONE_TIME 8
void busy_delay_ms(uint32_t ms) {
    uint32_t cycles_to_wait = (ms * (rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ].ahb_frequency / 1000)) /
                                  STM32_CYCLES_PER_LOOP -
                              STM32_LOST_CYCLES_ONE_TIME;

    __asm__ volatile(
        " mov r0, %[cycles_to_wait] \n\t"
        "1: subs r0, #1 \n\t"
        " bhi 1b \n\t"
        :
        : [cycles_to_wait] "r"(cycles_to_wait)
        : "r0");
}

#if 0
#define start_timer() *((volatile uint32_t*)0xE0001000) = 0x40000001  // Enable CYCCNT register
#define stop_timer() *((volatile uint32_t*)0xE0001000) = 0x40000000   // Disable CYCCNT register
#define get_timer() *((volatile uint32_t*)0xE0001004)  // Get value from CYCCNT register
uint32_t measure_cycles(uint32_t loops) {
    uint32_t it1, it2;
    uint32_t ret;

    start_timer();

    it1 = get_timer();

    __asm__ volatile(
        "mov r0, %[loops] \n\t"
        "1: subs r0, #1 \n\t"
        " bhi 1b \n\t"
        :
        : [loops] "r"(loops)
        : "r0");

    it2 = get_timer();

    stop_timer();

    ret = it2 - it1;
    return ret;

    /*
    loops = 1: ret = 11
    loops = 1000: ret = 3008
    loops = 10000: ret = 30008
    loops = 100000: ret = 300008
    -> the __asm__ blocks worths (3 * loops) + 8
    */
}
#endif

bool system_clock_init(void) {
    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
    return true;
}

bool mmi_init() {
    led_init(led_blue);
    led_init(led_red);
    led_init(led_green);
    led_init(led_orange);

    led_on(led_blue);

    return true;
}

bool cs43l22_shutdown(void) {
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

    /* power down the codec */
    gpio_clear(GPIOD, GPIO4);

    /* wait for a delay to insure registers erasing */
    busy_delay_ms(5);

    return true;
}

void fault() {
    bool state = true;
    while (true) {
        if (state)
            led_on(led_red);
        else
            led_off(led_red);
        state = !state;
        busy_delay_ms(1000);
    }
}
