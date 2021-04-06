#include <stdbool.h>
#include <stdint.h>

#include "usbd_def.h"
#include "usbd_audio.h"

/* BSP */
bool bsp_init(void);
void fault(void);

/* mmi */
bool mmi_init(void);
void mmi_heartbeat(void);

/* usb */
bool usb_init(void);
bool usb_start(USBD_AUDIO_ItfTypeDef* pUSBD_AUDIO_fops);
void usb_transfer_complete(void);
void usb_half_transfer_complete(void);


/* i2c */
void i2c_init();
void i2c_write(uint16_t device_addr, uint16_t memaddr, uint8_t value);
void i2c_burst_write(uint16_t device_addr, uint16_t memaddr, uint8_t value[], uint16_t size);
uint8_t i2c_read(uint16_t device_addr, uint16_t memaddr);

/* i2s */
bool i2s_init(uint32_t audio_freq, void (*transfer_complete_dma)());
void i2s_transmit_dma(uint16_t* audio_current_pos, uint16_t transmit_size);
void i2s_stop_dma();
void i2s_deinit(void);

bool cs43l22_enable(bool enable);
