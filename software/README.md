# install the toolchain

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
