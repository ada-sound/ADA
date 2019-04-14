#include <stdbool.h>
#include <stdint.h>

/* BSP */
bool bsp_init(void);
void fault(void);

/* LED */
typedef enum { led_green, led_orange, led_red, led_blue, led_max } led_t;
void led_on(led_t led);
void led_off(led_t led);

/* TIME */
void busy_delay_ms(uint32_t ms);

/* OTHER */
bool mmi_init(void);
