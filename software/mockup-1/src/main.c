#include "bsp.h"
#include "tas3251.h"

int main(void) {
    /* init bsp and MMI */
    if (!bsp_init() || !mmi_init() /*|| !usb_init()*/) fault();

    /* */
    if (!tas3251_init(0X94)) fault();

    /* if (!usb_start()) fault(); */
    while (1) {        
        mmi_heartbeat();
    }
}
