#include <libopencm3/cm3/scs.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <bsp.h>

/* low-level loop to wait some cpu cycles */
#define BUSY_WAIT_CYCLES(cycles)            \
    __asm__ volatile(" mov r0, %[" #cycles  \
                     "] \n\t"               \
                     "1: subs r0, #1 \n\t"  \
                     " bhi 1b \n\t"         \
                     :                      \
                     : [cycles] "r"(cycles) \
                     : "r0")
static uint32_t _cycles_per_loop;
static uint32_t _cycles_shift;

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
static void _led_init(led_t led) {
    rcc_periph_clock_enable(_leds[led].clken);
    gpio_mode_setup(_leds[led].port, _leds[led].mode, _leds[led].pull_up_down, _leds[led].gpios);
}

void led_on(led_t led) { gpio_set(_leds[led].port, _leds[led].gpios); }

void led_off(led_t led) { gpio_clear(_leds[led].port, _leds[led].gpios); }

static uint32_t _ms_to_cycles(uint32_t ms) {
    return (ms * (rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ].ahb_frequency / 1000)) / _cycles_per_loop -
           _cycles_shift;
}

static uint32_t _us_to_cycles(uint32_t us) {
    return (us * rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ].ahb_frequency) / _cycles_per_loop -
           _cycles_shift;
}

void busy_delay_ms(uint32_t ms) {
    uint32_t cycles_to_wait =
        (ms * (rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ].ahb_frequency / 1000)) / _cycles_per_loop -
        _cycles_shift;

    BUSY_WAIT_CYCLES(cycles_to_wait);
}

static inline void _start_cyccnt(void) { SCS_DWT_CTRL = 0x40000001; }

static inline void _stop_cyccnt(void) { SCS_DWT_CTRL = 0x40000000; }

static inline uint32_t _get_cyccnt(void) { return SCS_DWT_CYCCNT; }

void _calibrate_delay_cycles(uint32_t* cycles_per_loop, uint32_t* cycles_more) {
    uint32_t loops = 1000;
    uint32_t it1, it2;
    uint32_t ret;

    _start_cyccnt();
    it1 = _get_cyccnt();

    __asm__ volatile(
        "mov r0, %[loops] \n\t"
        "1: subs r0, #1 \n\t"
        " bhi 1b \n\t"
        :
        : [loops] "r"(loops)
        : "r0");

    it2 = _get_cyccnt();
    _stop_cyccnt();

    *cycles_per_loop = (it2 - it1) / loops;
    *cycles_more = (it2 - it1) - (*cycles_per_loop) * loops;
}

bool _cs43l22_shutdown(void) {
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

    /* power down the codec */
    gpio_clear(GPIOD, GPIO4);

    /* wait for a delay to insure registers erasing */
    busy_delay_ms(5);

    return true;
}

bool bsp_init(void) {
    /* calibrate delay cycles */
    _calibrate_delay_cycles(&_cycles_per_loop, &_cycles_shift);

    /* init board clocks */
    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    /* leds */
    _led_init(led_blue);
    _led_init(led_red);
    _led_init(led_green);
    _led_init(led_orange);

    /* switch off the codec cs43l22 */
    return _cs43l22_shutdown();
}

bool mmi_init() {
    led_on(led_blue);
    return true;
}

void fault() {
    bool state = true;

    /* bye bye */
    while (true) {
        if (state)
            led_on(led_red);
        else
            led_off(led_red);
        state = !state;
        busy_delay_ms(500);
    }
}
