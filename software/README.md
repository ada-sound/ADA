# install the toolchain

cf https://www.st.com/en/development-tools/stm32cubemx.html

cf https://launchpad.net/gcc-arm-embedded

cf http://openocd.org/

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
```bash
# if needed correct the wrong pid of the stlink-v2.0 interface
sudo sed -i "s/0x3748/0x374B/g" /usr/share/openocd/scripts/interface/stlink-v2.cfg
# if needed apply the udev rules and add yourself to the plugdev group
sudo cp /usr/local/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d
sudo groupadd plugdev
sudo usermod -a -G plugdev louis
```

# build
```bash
make
```

# flash
```bash
openocd -f board/stm32f4discovery.cfg -c "program build/core.bin exit 0x08000000"
```

# debug

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
