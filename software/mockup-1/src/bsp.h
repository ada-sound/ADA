#include <stdbool.h>
#include <stdint.h>

/* BSP */
bool bsp_init(void);
void fault(void);

/* OTHER */
bool mmi_init(void);

bool usb_init(void);
bool usb_start(void);