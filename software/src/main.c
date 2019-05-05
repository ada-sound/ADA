#include "bsp.h"
#include "tas3251.h"

int main(void) {
    /* init bsp and MMI */
    if (!bsp_init() || !mmi_init()) fault();

    /* */
    if (!tas3251_init(0X94)) fault();

    while (true)
        ;
}
