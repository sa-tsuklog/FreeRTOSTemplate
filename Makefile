SHELL = cmd.exe
TARGET_ARCH   = -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16
INCLUDE_DIRS  = -I ./Libraries \
				-I ./Libraries/STM32F4xx_StdPeriph_Driver/inc \
				-I ./Libraries/CMSIS/Device/ST/STM32F4xx/include \
				-I ./Libraries/CMSIS/Include \
				-I $(TOOLDIR)../arm-none-eabi/include \
				-I $(TOOLDIR)../arm-none-eabi/include/c++/4.6.2 \
				-I ./Libraries/FreeRTOS/Source \
				-I ./Libraries/FreeRTOS/Source/portable/GCC/ARM_CM4F \
				-I ./Libraries/FreeRTOS/Source/include \
				-I ./Libraries/FreeRTOS/Demo/Common/include \
				-I src
STARTUP_DIR = ./Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/
BOARD_OPTS = -DHSE_VALUE=((uint32_t)8000000) -DSTM32F4XX
FIRMWARE_OPTS = -DUSE_STDPERIPH_DRIVER
COMPILE_OPTS  = -O2 -g3 -ffunction-sections -fdata-sections -fsigned-char -fno-rtti -fno-exceptions -Wall -fmessage-length=0 $(INCLUDE_DIRS) $(BOARD_OPTS) $(FIRMWARE_OPTS) -fpermissive -mfpu=fpv4-sp-d16

TOOLDIR = ../../yagarto/bin/
CC      = $(TOOLDIR)arm-none-eabi-g++
CXX		= $(CC)
AS      = $(CC)
LD      = $(CC)
AR      = $(TOOLDIR)arm-none-eabi-ar
OBJCOPY = $(TOOLDIR)arm-none-eabi-objcopy
CFLAGS  = $(COMPILE_OPTS)
CXXFLAGS= $(COMPILE_OPTS)
ASFLAGS = -x assembler-with-cpp -c $(TARGET_ARCH) $(COMPILE_OPTS) 
LDFLAGS = -Wl,--gc-sections,-Map=bin\main.map,-cref -T stm32_flash.ld $(INCLUDE_DIRS) -lstdc++ -L $(TOOLDIR)../arm-none-eabi/lib/thumb -L ./Libraries

all: libstm32f4xx startup bin\main.hex

SRC_DIRS = src
SRC_DIRS += src\Stdout
SRC_DIRS += src\Adis16488
SRC_DIRS += src\Mpu-9250
SRC_DIRS += src\Servo

# main.o is compiled by suffix rule automatucally
bin\main.hex: $(patsubst %.c,%.o,$(wildcard $(addsuffix /*.c, $(SRC_DIRS)))) \
    $(patsubst %.cpp,%.o,$(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))) \
    $(STARTUP_DIR)startup_stm32f4xx.o ./Libraries/libstm32f4xx.a
     
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ -o bin\main.elf 
	$(OBJCOPY) -O ihex bin\main.elf bin\main.hex

FREERTOS_DIR = .\Libraries\FreeRTOS

# many of xxx.o are compiled by suffix rule automatically
LIB_OBJS = $(sort \
 $(patsubst %.c,%.o,$(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/src/*.c)) \
 $(patsubst %.c,%.o,$(wildcard $(FREERTOS_DIR)/Source/portable/MemMang/heap_2.c)) \
 $(patsubst %.c,%.o,$(wildcard $(FREERTOS_DIR)/Source/*.c)) \
 $(patsubst %.c,%.o,$(wildcard $(FREERTOS_DIR)\Source\portable\GCC\ARM_CM4F/*.c)) \
 $(patsubst %.c,%.o,$(FREERTOS_DIR)/Demo/Common/Minimal/GenQTest.c) 		\
 $(patsubst %.c,%.o,$(FREERTOS_DIR)/Demo/Common/Minimal/BlockQ.c) 			\
 $(patsubst %.c,%.o,$(FREERTOS_DIR)/Demo/Common/Minimal/blocktim.c) 		\
 $(patsubst %.c,%.o,$(FREERTOS_DIR)/Demo/Common/Minimal/QPeek.c) 			\
 $(patsubst %.c,%.o,$(FREERTOS_DIR)/Demo/Common/Minimal/PollQ.c) 			)

libstm32f4xx: $(LIB_OBJS)
	$(AR) cr ./Libraries/libstm32f4xx.a $(LIB_OBJS)
	
startup:
	$(AS) -o $(STARTUP_DIR)/startup_stm32f4xx.o $(ASFLAGS) $(STARTUP_DIR)startup_stm32f4xx.s

$(LIB_OBJS): \
 $(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/inc/*.h) \
 $(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/src/*.c) \
 makefile

clean:
	del /f /q *.o *.s bin\* $(FREERTOS_DIR)\Demo\Common\Minimal\*.o .\Libraries\FreeRTOS\Source\*.o .\Libraries\FreeRTOS\Source\portable\GCC\ARM_CM4F\*.o .\Libraries\FreeRTOS\Source\portable\MemMang\*.o .\Libraries\*.s $(addsuffix \*.o, $(SRC_DIRS))
	