#2022/7/31 22:56
#make-test


#define project name
PRJ_NAME = pwm
#build, contents obj file
BUILD_DIR = build

#fake target define
.PHONY: clean connect gdb download

#define tools
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CPP = $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
LD = $(GCC_PATH)/$(PREFIX)ld
else
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
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
/Bsp/Src/adc14.c \
/Bsp/Src/aes256.c \
/Bsp/Src/comp_e.c \
/Bsp/Src/cpu.c \
/Bsp/Src/crc32.c \
/Bsp/Src/cs.c \
/Bsp/Src/dma.c \
/Bsp/Src/flash.c \
/Bsp/Src/fpu.c \
/Bsp/Src/gpio.c \
/Bsp/Src/i2c.c \
/Bsp/Src/interrupt.c \
/Bsp/Src/mpu.c \
/Bsp/Src/pcm.c \
/Bsp/Src/pmap.c \
/Bsp/Src/pss.c \
/Bsp/Src/ref_a.c \
/Bsp/Src/reset.c \
/Bsp/Src/rtc_c.c \
/Bsp/Src/spi.c \
/Bsp/Src/sysctl.c \
/Bsp/Src/systick.c \
/Bsp/Src/timer32.c \
/Bsp/Src/timer_a.c \
/Bsp/Src/uart.c \
/Bsp/Src/wdt_a.c\
/Core/Src/startup_msp432p401r_gcc.c \
/Core/Src/system_msp432p401r.c \

CPP_SOURCES = \
/main.cpp \
/User/Src/user_uart.cpp \
/User/Src/user_pwm.cpp \

#defines
C_DEFS = \
-D__MSP432P401R__ \
-Dgcc \

#includes
C_INCLUDES = \
-ICore/Include \
-ICore/Include/CMSIS \
-IBsp/Inc \
-IUser/Inc \

#lib link
LIB = -lc -lm -lnosys \

#link options
LDFLAGS = $(MCU) -Tmsp432p401r.lds -specs=nosys.specs $(LIB) -Wl,-gc-sections,--cref -Wl,-Map=$(BUILD_DIR)/$(PRJ_NAME).map#-nostdlib
#compile options
CCFLAGS = -c -g $(MCU) $(C_DEFS) $(C_INCLUDES) #

#path define
VPATH = \
Core/Include:Core/Include/CMSIS:Bsp/Inc:\
Bsp/Src:Core/Src/:User/Src/:\

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))

all: $(BUILD_DIR)/$(PRJ_NAME).elf $(BUILD_DIR)/$(PRJ_NAME).bin


$(BUILD_DIR)/$(PRJ_NAME).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/$(PRJ_NAME).bin: $(BUILD_DIR)/$(PRJ_NAME).elf
	$(CP) $< -O binary $@

$(BUILD_DIR)/%.o:%.c | $(BUILD_DIR)
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/%.o:%.cpp | $(BUILD_DIR)
	$(CPP) $(CCFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)
connect:
	openocd -f I:\MCU\Openocd\INSTALL\scripts\interface\cmsis-dap.cfg -f I:\MCU\Openocd\INSTALL\scripts\target\ti_msp432.cfg
gdb:
	arm-none-eabi-gdb $(BUILD_DIR)/$(PRJ_NAME).elf
download:
	openocd \
	-f I:/MCU/Openocd/INSTALL/scripts/interface/cmsis-dap.cfg \
	-f I:/MCU/Openocd/INSTALL/scripts/target/ti_msp432.cfg \
	-c init \
	-c halt \
	-c "flash write_image erase $(BUILD_DIR)/$(PRJ_NAME).elf 0x00000000" \
	-c reset \
	-c shutdown 