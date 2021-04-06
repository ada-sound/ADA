#include "bsp.h"
#include "ada_v0-1.h"
#include "f4-disco.h"

int main(void) {
    /* init bsp and MMI */
    if (!bsp_init() || !mmi_init())
        fault();
    
    //if (!ada_v01_init())
    //    fault();

    if (!f4_disco_init())
        fault();

    while (1) {
        mmi_heartbeat();
    }
}
