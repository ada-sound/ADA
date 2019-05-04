#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <stdlib.h>
#include "i2c.h"

#define I2C1_GPIO_PORT GPIOB
#define I2C1_GPIO_SCL GPIO6
#define I2C1_GPIO_SDA GPIO7
#define I2C1_GPIO_AF GPIO_AF4

#define I2C2_GPIO_PORT GPIOB
#define I2C2_GPIO_SCL GPIO10
#define I2C2_GPIO_SDA GPIO11
#define I2C2_GPIO_AF GPIO_AF4

#define I2C3_GPIO_PORT_SCL GPIOA
#define I2C3_GPIO_SCL GPIO8
#define I2C3_GPIO_PORT_SDA GPIOC
#define I2C3_GPIO_SDA GPIO9
#define I2C3_GPIO_SCL_AF GPIO_AF4
#define I2C3_GPIO_SDA_AF GPIO_AF4

struct i2c_periph i2c1;

static void gpio_enable_clock(uint32_t port)
{
  switch (port) {
    case GPIOA:
      rcc_periph_clock_enable(RCC_GPIOA);
      break;
    case GPIOB:
      rcc_periph_clock_enable(RCC_GPIOB);
      break;
    case GPIOC:
      rcc_periph_clock_enable(RCC_GPIOC);
      break;
    case GPIOD:
      rcc_periph_clock_enable(RCC_GPIOD);
      break;
  }
}

static void i2c_setup_gpio(uint32_t i2c) {
    switch (i2c) {
        case I2C1:
            gpio_enable_clock(I2C1_GPIO_PORT);
            gpio_mode_setup(I2C1_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                            I2C1_GPIO_SCL | I2C1_GPIO_SDA);
            gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,
                                    I2C1_GPIO_SCL | I2C1_GPIO_SDA);
            gpio_set_af(I2C1_GPIO_PORT, I2C1_GPIO_AF, I2C1_GPIO_SCL | I2C1_GPIO_SDA);
            break;
        case I2C2:
            gpio_enable_clock(I2C2_GPIO_PORT);
            gpio_mode_setup(I2C2_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                            I2C2_GPIO_SCL | I2C2_GPIO_SDA);
            gpio_set_output_options(I2C2_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,
                                    I2C2_GPIO_SCL | I2C2_GPIO_SDA);
            gpio_set_af(I2C2_GPIO_PORT, I2C2_GPIO_AF, I2C2_GPIO_SCL | I2C2_GPIO_SDA);
            break;
        case I2C3:
            gpio_enable_clock(I2C3_GPIO_PORT_SCL);
            gpio_mode_setup(I2C3_GPIO_PORT_SCL, GPIO_MODE_AF, GPIO_PUPD_NONE, I2C3_GPIO_SCL);
            gpio_set_output_options(I2C3_GPIO_PORT_SCL, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,
                                    I2C3_GPIO_SCL);
            gpio_set_af(I2C3_GPIO_PORT_SCL, I2C3_GPIO_SCL_AF, I2C3_GPIO_SCL);

            gpio_enable_clock(I2C3_GPIO_PORT_SDA);
            gpio_mode_setup(I2C3_GPIO_PORT_SDA, GPIO_MODE_AF, GPIO_PUPD_NONE, I2C3_GPIO_SDA);
            gpio_set_output_options(I2C3_GPIO_PORT_SDA, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,
                                    I2C3_GPIO_SDA);
            gpio_set_af(I2C3_GPIO_PORT_SDA, I2C3_GPIO_SDA_AF, I2C3_GPIO_SDA);
            break;
        default:
            break;
    }
}

bool i2c_init(void) {
    /* Configure and enable I2C1 event interrupt --------------------------------*/
    nvic_set_priority(NVIC_I2C1_EV_IRQ, NVIC_I2C1_IRQ_PRIO);
    nvic_enable_irq(NVIC_I2C1_EV_IRQ);

    /* Configure and enable I2C1 err interrupt ----------------------------------*/
    nvic_set_priority(NVIC_I2C1_ER_IRQ, NVIC_I2C1_IRQ_PRIO + 1);
    nvic_enable_irq(NVIC_I2C1_ER_IRQ);

    /* Enable peripheral clocks -------------------------------------------------*/
    /* Enable I2C1 clock */
    rcc_periph_clock_enable(RCC_I2C1);
    /* setup gpio clock and pins */
    i2c_setup_gpio(I2C1);

    rcc_periph_reset_pulse(RST_I2C1);

    // enable peripheral
    i2c_peripheral_enable(I2C1);

    i2c_set_own_7bit_slave_address(I2C1, 0);

    // enable error interrupts
    i2c_enable_interrupt(I2C1, I2C_CR2_ITERREN);

    i2c_setbitrate(&i2c1, I2C1_CLOCK_SPEED);

    return true;
}

void i2c1_ev_isr(void)
{
  uint32_t i2c = (uint32_t) i2c1.reg_addr;
  i2c_disable_interrupt(i2c, I2C_CR2_ITERREN);
  i2c1.watchdog = 0;
  i2c_irq(&i2c1);
  i2c_enable_interrupt(i2c, I2C_CR2_ITERREN);
}

void i2c1_er_isr(void)
{
  uint32_t i2c = (uint32_t) i2c1.reg_addr;
  i2c_disable_interrupt(i2c, I2C_CR2_ITEVTEN);
  i2c1.watchdog = 0; // restart watchdog
  i2c_irq(&i2c1);
  i2c_enable_interrupt(i2c, I2C_CR2_ITEVTEN);
}
