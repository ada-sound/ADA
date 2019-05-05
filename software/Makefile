## output
OUTPUT_NAME=ada
OUTPUT_DIR=bin

## toolchain
PREFIX?=arm-none-eabi-
CC=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
STM32CUBE=src/STM32Cube_FW_F4_V1.24.0
STM32CUBE_ARCHIVE=src/STM32Cube_FW_F4_V1.24.0.extract.tar.bz2
HAL=$(STM32CUBE)/Drivers/STM32F4xx_HAL_Driver
USB_DEVICE_LIB=src/STM32_USB_Device_Library
STREAMING=src/Streaming

## sources and headers
SRC=src/startup_stm32f407xx.s \
	src/system_stm32f4xx.c \
	src/main.c \
	src/bsp.c \
	src/tas3251.c \
	src/audio_sample.c \
	src/usbd_conf.c \
	src/usbd_desc.c \
	$(HAL)/Src/stm32f4xx_hal.c \
	$(HAL)/Src/stm32f4xx_hal_cortex.c \
	$(HAL)/Src/stm32f4xx_hal_dma.c \
	$(HAL)/Src/stm32f4xx_hal_gpio.c \
	$(HAL)/Src/stm32f4xx_hal_i2c.c \
	$(HAL)/Src/stm32f4xx_hal_i2s.c \
	$(HAL)/Src/stm32f4xx_hal_i2s_ex.c \
	$(HAL)/Src/stm32f4xx_hal_rcc.c \
	$(HAL)/Src/stm32f4xx_hal_rcc_ex.c \
	$(HAL)/Src/stm32f4xx_hal_spi.c \
	$(USB_DEVICE_LIB)/Class/AUDIO_10/Src/usbd_audio.c \
	$(USB_DEVICE_LIB)/Core/Src/usbd_core.c \
	$(USB_DEVICE_LIB)/Core/Src/usbd_ctlreq.c \
	$(USB_DEVICE_LIB)/Core/Src/usbd_ioreq.c

INC=-I./src
INC+=-I$(STM32CUBE)/Drivers/STM32F4xx_HAL_Driver/Inc
INC+=-I$(STM32CUBE)/Drivers/CMSIS/Device/ST/STM32F4xx/Include
INC+=-I$(STM32CUBE)/Drivers/BSP/Components/Common
INC+=-I$(STM32CUBE)/Drivers/CMSIS/Include
INC+=-I$(USB_DEVICE_LIB)/Class/AUDIO_10/Inc
INC+=-I$(USB_DEVICE_LIB)/Core/Inc
INC+=-I$(STREAMING)/Inc

LINKER_FILE=src/STM32F407VG_FLASH.ld

## compilation rules
CFLAGS_CORTEX_M4=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
GCCFLAGS=--static -nostartfiles -std=c11 -Wall
GCCFLAGS+=-fno-common -ffunction-sections -fdata-sections
#GCCDEBUGFLAGS=-g3 -O0 -DPENDSV_DEBUG
GCCDEBUGFLAGS=-g3 -Os -DPENDSV_DEBUG
#GCCDEBUGFLAGS=-Os -DNDEBUG
CFLAGS=$(INC) -DSTM32F407xx -DUSE_USB_HS $(CFLAGS_CORTEX_M4) $(GCCFLAGS) $(GCCDEBUGFLAGS)

LFLAGS=-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
LFLAGS+=$(SRC) -T $(LINKER_FILE)

## the project
ELF=$(OUTPUT_DIR)/$(OUTPUT_NAME).elf
BIN=$(OUTPUT_DIR)/$(OUTPUT_NAME).bin
HEX=$(OUTPUT_DIR)/$(OUTPUT_NAME).hex

%.bin: %.elf
	@#printf "  OBJCOPY $(*).bin\n"
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	@#printf "  OBJCOPY $(*).hex\n"
	$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.elf: $(SRC)
	@echo "Creating $(*).elf"
	$(CC) $(CFLAGS) $(LFLAGS) -o $(*).elf

## generic targets
.DEFAULT_GOAL := all
all: outdir stm32_hal $(ELF) $(BIN) $(HEX)

outdir:
	mkdir -p $(OUTPUT_DIR)

stm32_hal:
	@if [ ! -d $(STM32CUBE) ]; then echo Extracting STM32 HAL and LL; tar xf $(STM32CUBE_ARCHIVE) -C src; fi

clean:
	$(RM) -r $(OUTPUT_DIR) $(ELF) $(BIN) $(HEX)

distclean:
	$(RM) -r $(OUTPUT_DIR) $(STM32CUBE) $(ELF) $(BIN) $(HEX)

$(V).SILENT:
