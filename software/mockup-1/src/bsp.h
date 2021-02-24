#include <stdbool.h>
#include <stdint.h>

/* BSP */
bool bsp_init(void);
void fault(void);

/* OTHER */
bool mmi_init(void);
void mmi_heartbeat(void);

bool usb_init(void);
bool usb_start(void);

/* i2c */
void i2c_init(uint32_t i2c_device_addr);
void i2c_write(uint32_t i2c_device_addr, uint16_t memaddr, uint8_t value);
void i2c_burst_write(uint32_t i2c_device_addr, uint16_t memaddr, uint8_t value[], uint16_t size);
uint8_t i2c_read(uint16_t i2c_device_addr, uint16_t memaddr);

/* i2s */
bool i2s_init(uint32_t AudioFreq, void (*transfer_complete_dma)());
void i2s_transmit_dma(uint16_t* audio_current_pos, uint16_t transmit_size);
void i2s_stop_dma();
