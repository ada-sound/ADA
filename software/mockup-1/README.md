# install the toolchain

Follow the steps below. As a documentation you can refer to
- https://www.st.com/en/development-tools/stm32cubemx.html
- https://launchpad.net/gcc-arm-embedded
- http://openocd.org/

## deb world

```bash
# install a gcc toolchain
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt update
sudo apt install gcc-arm-embedded

# install openocd
sudo apt install openocd
```

## rpm world

```bash
# install a gcc toolchain
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib

# install arm-none-eabi-gdb from https://apps.fedoraproject.org/packages/
wget https://kojipkgs.fedoraproject.org//packages/arm-none-eabi-gdb/7.6.2/4.fc24/x86_64/arm-none-eabi-gdb-7.6.2-4.fc24.x86_64.rpm
sudo dnf install ./arm-none-eabi-gdb-7.6.2-4.fc24.x86_64.rpm

# install openocd
sudo dnf install openocd
```

## openocd issues

Sometimes seen on linux: wrong pid of ST-LINK/V2 in openocd.

If openocd fails to detect your ST-LINK/V2 you can try to do this:

```bash
# if needed correct the wrong pid of the stlink-v2.0 interface
sudo sed -i "s/0x3748/0x374B/g" /usr/share/openocd/scripts/interface/stlink-v2.cfg
# if needed apply the udev rules and add yourself to the plugdev group
sudo cp /usr/local/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d
sudo groupadd plugdev
sudo usermod -a -G plugdev louis
```

## ST libraries

### STM32CubeF4

Download X-CUBE-USB-AUDIO v1.24.0 from https://github.com/ada-sound/STM32CubeF4

Untar it in `src/STM32CubeF4`

### X-CUBE-USB-AUDIO

Download X-CUBE-USB-AUDIO from https://my.st.com/content/my_st_com/en/products/embedded-software/mcu-mpu-embedded-software/stm32-embedded-software/stm32cube-expansion-packages/x-cube-usb-audio.html

Untar it in `src/X-CUBE-USB-AUDIO`

### tree

Tree should look like

```bash
➤ tree -L 2 src/
.
├── (src, headers, md, ld)
├── STM32CubeF4
│   └── STM32Cube_FW_F4_V1.24.0
└── X-CUBE-USB-AUDIO
    └── STM32CubeExpansion_USBAudioStreaming_V1.0.0
```

# build
```bash

```

# flash
```bash
openocd -f board/stm32f4discovery.cfg -c "program bin/ada.bin exit 0x08000000"
```

# debug

* example with vscode and openocd

Install marus25 Cortex-debug extension v0.3.12

cf https://github.com/Marus/cortex-debug/wiki

Add a new debug configuration like
```
{
    "cwd": "path-to-mockup-1",
    "executable": "bin/ada.elf",
    "name": "mockup-1 debug OPENOCD",
    "request": "launch",
    "type": "cortex-debug",
    "servertype": "openocd",
    "configFiles": [ "board/stm32f4discovery.cfg"]
},
```

* manually

In a terminal:
```bash
openocd -f board/stm32f4discovery.cfg
```

In another terminal:
```bash
arm-none-eabi-gdb
(gdb) target remote localhost:3333
(gdb) symbol-file bin/ada.elf
Breakpoint 1 at 0x8000546: file Src/main.c, line 74.
(gdb) 
```

* vscode with Cortex-Debug plugin
 
cf https://marcelball.ca/projects/cortex-debug/

launch.json
```
{
    "type": "cortex-debug",
    "request": "launch",
    "servertype": "openocd",
    "cwd": "${workspaceRoot}",
    "executable": "./bin/stm32/diy-fda.elf",
    "name": "Debug (OpenOCD)",
    "device": "STM32F4x",
    "configFiles": [
        "board/stm32f4discovery.cfg"
    ]
}
```
# wiring


| STM32F4-Discovery | TAS3251EVM      |
|-------------------|-----------------|
| I2C1 SDA PB9      | J9-2  SDA       |
| I2C1 SCL PB6      | J9-1  SCL       |
| I2S3 MCK PC7      | J9-3  MCLK      |
| I2S3 SCK PC10     | J9-7  BCLK      |
| I2S3 SD  PC12     | J9-5  SDIN      |
| I2S3 WS  PA4      | J9-11 LRCLK     |
|                   | J9-13 MUTE      |
|                   | J9-14 AMP RESET |

| STM32F4-Discovery | ADA 10_2020 v0.1 | Color  |
|-------------------|------------------|--------|
| I2C1 SDA PB9      | SDA              | blue   |
| I2C1 SCL PB6      | SCL              | violet |
| I2S3 MCK PC7      | MCLK             | grey   |
| I2S3 SCK PC10     | SCLK             | white  |
| I2S3 SD  PC12     | SDIN             | black  |
| I2S3 WS  PA4      | LRCK             | brown  |
| PC15              | MUTE             | green  |
| PC14              | RST              | yellow |

| STM32F4-Discovery | ADA 10_2020 v0.1 | Color  |
|-------------------|------------------|--------|
| I2C1 SDA PB9      | SDA              | blue   |
| I2C1 SCL PB6      | SCL              | violet |
| I2S2 MCK PC6      | MCLK             | grey   |
| I2S2 SCK PB10     | SCLK             | white  |
| I2S2 SD  PB15     | SDIN             | black  |
| I2S2 WS  PB9      | LRCK             | brown  |
| PC15              | MUTE             | green  |
| PC14              | RST              | yellow |
