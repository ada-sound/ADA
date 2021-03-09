#include "bsp.h"
#include "ada_v0-1.h"

int main(void) {
    /* init bsp and MMI */
    if (!bsp_init() || !mmi_init() || !usb_init())
        fault();
    
    if (!ada_v01_init())
        fault();

    while (1) {
        mmi_heartbeat();
    }
}
