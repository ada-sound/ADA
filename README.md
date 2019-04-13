# the ADA project

**ADA** - **A**ll **D**igital **A**mp - aims to design a **premium quality hi-fi audio amplifier** (hardware and software) based on a full digital amplifier chip **TAS3251** from Texas Instruments

The first requirements are:
- 2-way amplifier, about 60W at 8 ohms per voice
- digital sources *only*
  * USB Audio 2.0
  * SPDIF optical
  * SPDIF coaxial
  * HDMI-ARC
- auto-sleep
- auto-select source
- minimal MMI: power-on pushbutton, volume rotactor, audio-source rotactor, LCD panel, status LED

The board must be seen by a computer as a 2 channels 96kHz (or higher) sound card
It shall allow these lossless non-compressed audio formats: WAV, DCD, FLAC, ALAC, AIFF

For later:
- Bluetooth AptX
- HDMI + CEC (TV remote-> amp)
- HDMI + HEC (TV ethernet-> amp)

# first proto

Made from
- a [TAS3251](http://www.ti.com/product/TAS3251) out from a [TAS3251EVM eval board](http://www.ti.com/tool/TAS3251EVM)
- a [STM32F4](https://www.st.com/en/microcontrollers-microprocessors/stm32f4-series.html) out from a [STM32F4 Discovery board](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)
- some goodies like a rotactor, an LCD panel etc.

## install the toolchain

cf https://www.st.com/en/development-tools/stm32cubemx.html
cf https://launchpad.net/gcc-arm-embedded
cf http://openocd.org/

Run as root:
```bash

# install a gcc toolchain
add-apt-repository ppa:team-gcc-arm-embedded/ppa
apt update
apt install gcc-arm-embedded

# install openocd
apt install openocd
su
echo -e "8c8\n< hla_vid_pid 0x0483 0x3748\n---\n> hla_vid_pid 0x0483 0x374B\n" | patch -p1 /usr/share/openocd/scripts/interface/stlink-v2.cfg
```

## build
```bash
make
```

## flash
```bash
openocd -f board/stm32f4discovery.cfg -c "program build/core.bin exit 0x08000000"
```

## debug

cf http://www.justinmklam.com/posts/2017/10/vscode-debugger-setup/

* manually

```bash
openocd -f board/stm32f4discovery.cfg
arm-none-eabi-gdb
(gdb) target remote localhost:3333
(gdb) symbol-file /home/louis/Development/diy-fda/try1/build/diy-fda.elf
(gdb) monitor reset halt
(gdb) break main
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

# roadmap

## before the proto
Make work this:
1. [x] ST->i2c->TAS
2. [x] ST->gpio->DAC, RESET
3. [x] ST->i2s->TAS, TAS in the good mode
4. [ ] ST USB Audio 2
