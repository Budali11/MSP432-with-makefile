#2022/7/31 22:56
#make-test

LINUX = 1

ifdef LINUX
OPENOCD_PATH = /usr/local/share/openocd
else
OPENOCD_PATH = I:/MCU/Openocd/INSTALL
endif

#define project name
PRJ_NAME = debug_uart
#build, contents obj file
BUILD_DIR = build

#fake target define
.PHONY: clean connect download gdb

#define tools
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
LD = $(GCC_PATH)/$(PREFIX)ld
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
LD = $(PREFIX)ld
endif

# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)


#sources
C_SOURCES = \
main.c \
Bsp/Src/adc14.c \
Bsp/Src/aes256.c \
Bsp/Src/comp_e.c \
Bsp/Src/cpu.c \
Bsp/Src/crc32.c \
Bsp/Src/cs.c \
Bsp/Src/dma.c \
Bsp/Src/flash.c \
Bsp/Src/fpu.c \
Bsp/Src/gpio.c \
Bsp/Src/i2c.c \
Bsp/Src/interrupt.c \
Bsp/Src/mpu.c \
Bsp/Src/pcm.c \
Bsp/Src/pmap.c \
Bsp/Src/pss.c \
Bsp/Src/ref_a.c \
Bsp/Src/reset.c \
Bsp/Src/rtc_c.c \
Bsp/Src/spi.c \
Bsp/Src/sysctl.c \
Bsp/Src/systick.c \
Bsp/Src/timer32.c \
Bsp/Src/timer_a.c \
Bsp/Src/uart.c \
Bsp/Src/wdt_a.c\
Core/Src/startup_msp432p401r_gcc.c \
Core/Src/system_msp432p401r.c \
Bus/IIC/Src/iic_bus.c \
User/Src/user_periph.c \
User/Src/syscalls.c \
User/Src/user_uart.c \
User/Src/user_dma.c \

# device source
C_DEVICE_SOURCES = 
C_SOURCES += $(C_DEVICE_SOURCES)

# exclude file
EXCLUDE = \
Bus/IIC/Src/iic_bus.c \
Bsp/Src/dma.c \
Bsp/Src/i2c.c \
Bsp/Src/comp_e.c \
Bsp/Src/spi.c \
Bsp/Src/crc32.c \
Bsp/Src/adc14.c \
Bsp/Src/aes256.c \
Bsp/Src/pmap.c \
Bsp/Src/timer32.c \
Bsp/Src/timer_a.c \

C_EXCLUDED = $(filter-out $(EXCLUDE), $(C_SOURCES))


#defines
C_DEFS = \
-D__MSP432P401R__ \
-DPROCESS_FACE \
-Dgcc \

#includes
C_INCLUDES = \
-ICore/Include \
-ICore/Include/CMSIS \
-IBsp/Inc \
-IUser/Inc \
-IBus/IIC/Inc \

#lib link
LIB = -lc -lm -lnosys \

#link options
LDFLAGS = $(MCU) -Tmsp432p401r.lds -specs=nosys.specs $(LIB) -Wl,-gc-sections,--cref -Wl,-Map=$(BUILD_DIR)/$(PRJ_NAME).map
#compile options
CCFLAGS = -c -g $(MCU) $(C_DEFS) $(C_INCLUDES) -Wall#

#path define
VPATH = \
Core/Include:Core/Include/CMSIS:Bsp/Inc:User/Inc:\
Bsp/Src:Core/Src:User/Src:\
Bus/IIC/Src:\

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_EXCLUDED:.c=.o)))

all: $(BUILD_DIR)/$(PRJ_NAME).elf $(BUILD_DIR)/$(PRJ_NAME).bin

$(BUILD_DIR)/$(PRJ_NAME).elf: $(OBJECTS) makefile | $(BUILD_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/$(PRJ_NAME).bin: $(BUILD_DIR)/$(PRJ_NAME).elf makefile|$(BUILD_DIR)
	$(CP) $< -O binary $@

$(BUILD_DIR)/%.o:%.c | $(BUILD_DIR)
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)
connect:
	openocd -f $(OPENOCD_PATH)/scripts/interface/xds110.cfg -f $(OPENOCD_PATH)/scripts/target/ti_msp432.cfg
download:
	openocd \
	-f $(OPENOCD_PATH)/scripts/interface/xds110.cfg \
	-f $(OPENOCD_PATH)/scripts/target/ti_msp432.cfg \
	-c init \
	-c halt \
	-c "flash write_image erase $(BUILD_DIR)/$(PRJ_NAME).elf 0x00000000" \
	-c reset \
	-c shutdown 
gdb:
	arm-none-eabi-gdb $(BUILD_DIR)/$(PRJ_NAME).elf
