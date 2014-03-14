### You only have to modify here when you add a new file.
OSPATH = src/OS src/OS/Stdout src/OS/FreeRTOS_DemoFile
DRVPATH = src/Drivers src/Drivers/PeriphLib
MIDDLEPATH = src/Middle src/Middle/Adis16488 src/Middle/Mpu-9250 src/Middle/AD7176-2
###

SRCPATH = src $(OSPATH) $(DRVPATH) $(MIDDLEPATH)

OBJDIR = objs
BINDIR = bin

ifeq ($(OS),Windows_NT)
	SHELL = cmd.exe
	REMOVE = del /f /q $(BINDIR)\* $(OBJDIR)\*
	TOOLDIR = ../../yagarto/bin/
	STARTUP_ASM = ./Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
	MKOBJDIR = $(shell if not exist $(OBJDIR) mkdir $(OBJDIR))
	MKBINDIR = $(shell if not exist $(BINDIR) mkdir $(BINDIR))
else
	SHELL = sh
	REMOVE = rm -f $(BINDIR)/* $(OBJDIR)/*
	TOOLDIR = 
	STARTUP_ASM = ./Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
	MKOBJDIR = $(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi;)
	MKBINDIR = $(shell if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR); fi;)
endif

FREERTOS_DIR = ./Libraries/FreeRTOS
VPATH = ./Libraries/STM32F4xx_StdPeriph_Driver/src/ \
 $(FREERTOS_DIR)/Source/portable/MemMang/ \
 $(FREERTOS_DIR)/Source/ \
 $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/ \
 $(FREERTOS_DIR)/Demo/Common/Minimal/ \
 $(SRCPATH)

TARGET_ARCH   = -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16
INCLUDE_DIRS  = 	-I ./Libraries/STM32F4xx_StdPeriph_Driver/inc \
					-I ./Libraries/CMSIS/Device/ST/STM32F4xx/Include \
					-I ./Libraries/CMSIS/Include \
					-I $(TOOLDIR)../arm-none-eabi/include \
					-I $(TOOLDIR)../arm-none-eabi/include/c++/4.6.2 \
					-I $(FREERTOS_DIR)/Source/include \
					-I $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F \
					-I $(FREERTOS_DIR)/Demo/Common/include \
					-I ./src/OS/FreeRTOS_DemoFile \
					-I ./src/OS \
					-I ./src

BOARD_OPTS = -DHSE_VALUE=8000000 -DSTM32F4XX -DSTM32F40_41xxx
FIRMWARE_OPTS = -DUSE_STDPERIPH_DRIVER -DconfigUSE_TRACE_FACILITY -DconfigUSE_STATS_FORMATTING_FUNCTIONS
COMPILE_OPTS  = -O2 -g3 -ffunction-sections -fdata-sections -fsigned-char -fno-rtti -fno-exceptions -Wall -fmessage-length=0 $(INCLUDE_DIRS) $(BOARD_OPTS) $(FIRMWARE_OPTS) -fpermissive -mfpu=fpv4-sp-d16

CC      = $(TOOLDIR)arm-none-eabi-g++
CXX	    = $(CC)
AS      = $(CC)
LD      = $(CC)
AR      = $(TOOLDIR)arm-none-eabi-ar
OBJCOPY = $(TOOLDIR)arm-none-eabi-objcopy
CFLAGS  = $(COMPILE_OPTS)
CXXFLAGS= $(COMPILE_OPTS)
ASFLAGS = -x assembler-with-cpp -c $(TARGET_ARCH) $(COMPILE_OPTS) 
LDFLAGS = -Wl,--gc-sections,-Map=$(BINDIR)/main.map,-cref -T stm32_flash.ld -lstdc++ -L $(TOOLDIR)../arm-none-eabi/lib/thumb -L $(OBJDIR)

SRCS = $(wildcard $(addsuffix /*.c, $(SRCPATH)))
CPPSRCS = $(wildcard $(addsuffix /*.cpp, $(SRCPATH)))
OBJS = $(notdir $(patsubst %.c,%.o,$(SRCS)))
OBJS += $(notdir $(patsubst %.cpp,%.o,$(CPPSRCS)))

LIB_SRCS = \
 $(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/src/*.c) \
 $(wildcard $(FREERTOS_DIR)/Source/portable/MemMang/heap_2.c) \
 $(wildcard $(FREERTOS_DIR)/Source/*.c) \
 $(wildcard $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/*.c) \
 $(FREERTOS_DIR)/Demo/Common/Minimal/GenQTest.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/BlockQ.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/blocktim.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/QPeek.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/PollQ.c
LIB_OBJS = $(notdir $(LIB_SRCS:.c=.o))


all: main

main: $(addprefix $(OBJDIR)/,$(OBJS)) $(OBJDIR)/libstm32f4xx.a $(OBJDIR)/startup_stm32f4xx.o
	$(MKBINDIR)
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ -o $(BINDIR)/main.elf 
	$(OBJCOPY) -O ihex $(BINDIR)/main.elf $(BINDIR)/main.hex
	$(OBJCOPY) -O binary $(BINDIR)/main.elf $(BINDIR)/main.bin

$(OBJDIR)/%.o : %.c
	$(MKOBJDIR)
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c -o $@ $<

$(OBJDIR)/%.o : %.cpp
	$(MKOBJDIR)
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c -o $@ $<

$(OBJDIR)/libstm32f4xx.a: $(addprefix $(OBJDIR)/,$(LIB_OBJS))
	$(AR) cr $(OBJDIR)/libstm32f4xx.a $^

$(OBJDIR)/startup_stm32f4xx.o:
	$(AS) -o $(OBJDIR)/startup_stm32f4xx.o $(ASFLAGS) $(STARTUP_ASM)

clean:
	$(REMOVE)
