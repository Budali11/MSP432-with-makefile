#2022/7/31 22:56
#make-test


#define project name
PRJ_NAME = mpu6050
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
User/Src/syscalls.c \

CPP_BUS_SOURCES = \
Bus/IIC/Src/iic_bus.cpp \


CPP_DEVICE_SOURCES = \
Devices/mpu6050/Src/mpu6050.cpp \


CPP_SOURCES = \
main.cpp \
User/Src/user_uart.cpp \
User/Src/user_tick.cpp \
User/Src/user_nvic.cpp \
User/Src/user_iic.cpp \
User/Src/user_timer32.cpp \


CPP_SOURCES += $(CPP_DEVICE_SOURCES) $(CPP_BUS_SOURCES)

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
-IDevices/mpu6050/Inc \
-IBus/IIC/Inc \


#lib link
LIB = -lc -lm -lnosys \

#link options
LDFLAGS = $(MCU) -Tmsp432p401r.lds -specs=nosys.specs $(LIB) -Wl,-gc-sections,--cref -Wl,-Map=$(BUILD_DIR)/$(PRJ_NAME).map
#compile options
CCFLAGS = -c -g $(MCU) $(C_DEFS) $(C_INCLUDES) #

#path define
VPATH = \
Core/Include:Core/Include/CMSIS:Bsp/Inc:\
Bsp/Src:Core/Src:User/Src:Devices/mpu6050/Src:\
Bus/IIC/Src\


OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))

all: $(BUILD_DIR)/$(PRJ_NAME).elf $(BUILD_DIR)/$(PRJ_NAME).bin


$(BUILD_DIR)/$(PRJ_NAME).elf: $(OBJECTS)
	$(CPP) $(OBJECTS) $(LDFLAGS) -o $@
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
	openocd -f /usr/local/share/openocd/scripts/interface/xds110.cfg -f /usr/local/share/openocd/scripts/target/ti_msp432.cfg
download:
	openocd \
	-f /usr/local/share/openocd/scripts/interface/xds110.cfg \
	-f /usr/local/share/openocd/scripts/target/ti_msp432.cfg \
	-c init \
	-c halt \
	-c "flash write_image erase $(BUILD_DIR)/$(PRJ_NAME).elf 0x00000000" \
	-c reset \
	-c shutdown 
gdb:
	arm-none-eabi-gdb $(BUILD_DIR)/$(PRJ_NAME).elf
