#include <stdbool.h>
#include <stdint.h>

typedef enum { led_green, led_orange, led_red, led_blue, led_max } led_t;

void led_init(led_t led);
void led_on(led_t led);
void led_off(led_t led);

void delay(uint32_t ms);

bool system_clock_init(void);
bool mmi_init(void);

void fault(void);

bool cs43l22_shutdown(void);
