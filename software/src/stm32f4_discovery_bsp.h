typedef enum
{
    led_green,
    led_orange,
    led_red,
    led_blue,
    led_max
} led_t;

void led_init(led_t led);
void led_on(led_t led);
void led_off(led_t led);

void cs43l22_Shutdown(void);
