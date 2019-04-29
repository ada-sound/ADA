# Mockup

### 1. **Hardware and doc**
- a [STM32F4 Discovery board](https://www.st.com/en/)
- a Texas Instrument [TAS3251EVM eval board](http://www.ti.com/tool/TAS3251EVM)

### 2. **Configure TAS3251EVM**
- **Configure a default conf** in 48-kHz USB BTL mode by following [SLAU757B](slau757b.pdf) §1 'Quick Start (BTL mode)'
- Then **configure external I2S and I2C** by
    - disabling the XMOS: set J33
    - enabling the I2S from J9 connector: set J30 to 2-3
    - disabling the onboard MSP430: set J12 to 2-3
    - disabling the I2S buffers: set J35

### 3. **Wiring**

```
STM32F4-Discovery           TAS3251EVM
I2C1 SDA PB9  +-----------+ J9-2  SDA
I2C1 SCL PB6  +-----------+ J9-1  SCL
I2S3 MCK PC7  +-----------+ J9-3  MCLK
I2S3 SCK PC10 +-----------+ J9-7  BCLK
I2S3 SD  PC12 +-----------+ J9-5  SDIN
I2S3 WS  PA4  +-----------+ J9-11 LRCLK
              +-----------+ J9-13 MUTE
              +-----------+ J9-14 AMP RESET
```

### 4. **Software configuration**
- on STM32F4-Discovery
  - using I2C1, I2S3, DMA1_Stream7
  - setting I2C address to 0x94

### 5. **Software procedures**
- **init the amp**
  - disabling the CS43L22: set and keep PD4 low on the STM32F4-Discovery
  - starting the amp: cf [SLASEG6A](tas3251.pdf) p47

### 6. **Pre-sw build**

A pre-project based on SW4STM + STMCubeF4 exists!

- get SW4STM (Ac6 tools) then STM32CubeF4 1.24.0
- patch STM32Cube_FW_F4_V1.24.0 to add TAS3251

  ```
  unzip en.STM32Cube_FW_F4_V1.24.0.zip
  cd STM32Cube_FW_F4_V1.24.0 && patch --binary -p1 < ../ada_STM32CubeF4_tas3251.patch
  ```

- Run SW4STM then import the STM32F4-Discovery BSP project
    - Import -> Existing Projects into Workspace: choose STM32Cube_FW_F4_V1.24.0/Projects/STM32F4-Discovery/Examples/BSP/SW4STM32/
- Add TAS3251
  - add the symbol: Properties-> C/C++ Build -> Settings -> MCU GCC COmpiler -> Preprocessor : define symbol TAS3251
  - Add tas3251.c in the project tree in Drivers/BSP/Components close to cs43l22.c [sorry I didn't do it in the patch]
