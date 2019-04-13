## toolchain
PREFIX?=arm-none-eabi-
CC=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
OD=bin

## compilation rules
SRC=src/main.c src/stm32f4_discovery_bsp.c

#SFLAGS= --static -nostartfiles -std=c11 -g3 -Os
SFLAGS= --static -nostartfiles -std=c11 -g3 -O0
SFLAGS+= -fno-common -ffunction-sections -fdata-sections
SFLAGS+= -I./src
SFLAGS+= -I./libopencm3/include -L./libopencm3/lib
LFLAGS+=-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
M4FH_FLAGS=$(SFLAGS) -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
LFLAGS_STM32=$(LFLAGS) $(SRC) -T src/ld.stm32.basic
STM32F4_CFLAGS=$(M4FH_FLAGS) -DSTM32F4 $(LFLAGS_STM32) -lopencm3_stm32f4

## libopencm3
libopencm3:
	@echo "Cloning libopencm3 submodule"
	[ ! -d libopencm3 ] && git clone git@github.com:libopencm3/libopencm3.git

libopencm3/Makefile:
	@echo "Initializing libopencm3 submodule"
	git submodule update --init

libopencm3/lib/libopencm3_%.a: libopencm3/Makefile
	$(MAKE) TARGETS=stm32/f4 -j5 -C libopencm3

## the sw
BOARDS_ELF=$(OD)/stm32/stm32f4discover.elf
BOARDS_BIN=$(OD)/stm32/stm32f4discover.bin
BOARDS_HEX=$(OD)/stm32/stm32f4discover.hex

%.bin: %.elf
	@#printf "  OBJCOPY $(*).bin\n"
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	@#printf "  OBJCOPY $(*).hex\n"
	$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.elf: $(SRC) libopencm3/lib/libopencm3_stm32f4.a
	@echo "  stm32f4 -> Creating $(*).elf"
	$(CC) $(STM32F4_CFLAGS) -o $(*).elf

## generic targets
.DEFAULT_GOAL := all
all: outdir libopencm3 $(BOARDS_ELF) $(BOARDS_BIN) $(BOARDS_HEX)

outdir:
	mkdir -p $(OD)/stm32

clean:
	$(RM) -r $(OD) libopencm3 $(BOARDS_ELF) $(BOARDS_BIN) $(BOARDS_HEX)

$(V).SILENT:
