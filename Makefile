########## You only have to modify here when you add a new file.
OSPATH = src/OS
MYLIBPATH = src/MyLib/CmdServo src/MyLib/CmdServo/Driver \
		    src/MyLib/Gains src/MyLib/Gains/Driver/Adis16488 src/MyLib/Gains/Driver/Gps src/MyLib/Gains/Driver/Mpu9250 src/MyLib/Gains/Driver/SupersonicHeight src/MyLib/Gains/Driver/DummyGps \
		    src/MyLib/Logger \
		    src/MyLib/Stdout src/MyLib/Stdout/Driver src/MyLib/Stdout/DriverCmm920 \
		    src/MyLib/Util src/MyLib/Util/Driver \
		    src/MyLib/MoterDriver src/MyLib/MoterDriver/Driver \
		    src/MyLib/CmdServo src/MyLib/CmdServo/Driver \
		    src/MyLib/SBusPropo src/MyLib/SBusPropo/Driver \
		    src/MyLib/Servo src/MyLib/Servo/Driver \
		    src/MyLib/Propo src/MyLib/Propo/Driver \
		    src/MyLib/Seeker src/MyLib/Seeker/Driver src/MyLib/Seeker/Driver/AD7176-2 \
		    src/MyLib/SignalGenerator src/MyLib/SignalGenerator/Driver \
		    src/MyLib/CAN src/MyLib/CAN/Driver
		    
COMPATH   = src/Common src/Common/FreeRTOS_DemoFile
APPPATH = src/App src/App/TankControl \
			src/App/GliderControl src/App/GliderControl/ControlState

TOOLDIR = ../../launchpad/bin/
G++VER = 4.9.3
USE_SH = 1
##########

SRCPATH = src $(OSPATH) $(MYLIBPATH) $(COMPATH) $(APPPATH)

OBJDIR = objs
BINDIR = bin

ifeq ($(USE_SH), 1)
	SHELL = sh
	REMOVE = rm -f $(BINDIR)/* $(OBJDIR)/*
	MKOBJDIR = $(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi;)
	MKBINDIR = $(shell if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR); fi;)
else
	SHELL = cmd.exe
	REMOVE = del /f /q $(BINDIR)\* $(OBJDIR)\*
	MKOBJDIR = $(shell if not exist $(OBJDIR) mkdir $(OBJDIR))
	MKBINDIR = $(shell if not exist $(BINDIR) mkdir $(BINDIR))
endif

STARTUP_ASM = ./Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f429_439xx.s

FREERTOS_DIR = ./Libraries/FreeRTOS
FREERTOS_PLUS_DIR = ./Libraries/FreeRTOS-Plus

TARGET_ARCH   = -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16
INCLUDE_DIRS  = 	-I ./Libraries/STM32F4xx_StdPeriph_Driver/inc \
					-I ./Libraries/CMSIS/Device/ST/STM32F4xx/Include \
					-I ./Libraries/CMSIS/Include \
					-I ./Libraries/ff \
					-I $(TOOLDIR)../arm-none-eabi/include \
					-I $(TOOLDIR)../arm-none-eabi/include/c++/$(G++VER) \
					-I $(FREERTOS_DIR)/Demo/Common/include \
					-I $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F \
					-I ./src/include \
					-I ./src \
					-I ./src/pch \
					-I $(FREERTOS_PLUS_DIR)/Source/FreeRTOS-Plus-Trace/Include \
					-I $(FREERTOS_PLUS_DIR)/Source/FreeRTOS-Plus-Trace/ConfigurationTemplate \
					-I $(FREERTOS_DIR)/Source/include

BOARD_OPTS = -DHSE_VALUE=12000000 -DSTM32F4XX -DSTM32F40_41xxx -DSTM32F429 -DUSE_32F429IDISCOVERY
FIRMWARE_OPTS = -DUSE_STDPERIPH_DRIVER
COMPILE_OPTS  = -O3 -g3 -ffunction-sections -fdata-sections -fsigned-char -fno-exceptions \
					-Wall -fmessage-length=0 $(INCLUDE_DIRS) $(BOARD_OPTS) $(FIRMWARE_OPTS) -mfpu=fpv4-sp-d16

CC      = $(TOOLDIR)arm-none-eabi-gcc
CXX     = $(TOOLDIR)arm-none-eabi-g++
AS      = $(CXX)
LD      = $(CXX)
AR      = $(TOOLDIR)arm-none-eabi-ar
OBJCOPY = $(TOOLDIR)arm-none-eabi-objcopy
CFLAGS  = $(COMPILE_OPTS) -std=gnu99
CXXFLAGS= $(COMPILE_OPTS)  -fno-rtti -fpermissive -fno-threadsafe-statics
ASFLAGS = -x assembler-with-cpp -c $(TARGET_ARCH) $(COMPILE_OPTS) 
LDFLAGS = -Wl,--gc-sections,-Map=$(BINDIR)/main.map,-cref -T stm32_flash.ld -lstdc++ -L $(TOOLDIR)../arm-none-eabi/lib/thumb -L $(OBJDIR)

SRCS = $(wildcard $(addsuffix /*.c, $(SRCPATH)))
CPPSRCS = $(wildcard $(addsuffix /*.cpp, $(SRCPATH)))
OBJS = $(patsubst %.c,%.o,$(SRCS))
OBJS += $(patsubst %.cpp,%.o,$(CPPSRCS))
DEPS = $(OBJS:.o=.d)

LIB_SRCS = \
 $(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/src/*.c) \
 $(wildcard ./Libraries/ff/*.c) \
 $(wildcard ./Libraries/ff/option/cc932.c) \
 $(wildcard $(FREERTOS_DIR)/Source/portable/MemMang/heap_3.c) \
 $(wildcard $(FREERTOS_DIR)/Source/*.c) \
 $(wildcard $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/*.c) \
 $(FREERTOS_DIR)/Demo/Common/Minimal/GenQTest.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/BlockQ.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/blocktim.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/QPeek.c \
 $(FREERTOS_DIR)/Demo/Common/Minimal/PollQ.c \
 $(wildcard $(FREERTOS_PLUS_DIR)/Source/FreeRTOS-Plus-Trace/*.c)
LIB_OBJS = $(LIB_SRCS:.c=.o)
LIB_DEPS = $(LIB_OBJS:.o=.d)

PCHS = $(OBJDIR)/pch.gch 

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
-include $(PCHS:.gch=.d)
endif

all: main

main: $(OBJS) $(OBJDIR)/libstm32f4xx.a $(OBJDIR)/startup_stm32f4xx.o
	$(MKBINDIR)
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ -o $(BINDIR)/main.elf 
	$(OBJCOPY) -O ihex $(BINDIR)/main.elf $(BINDIR)/main.hex
	$(OBJCOPY) -O binary $(BINDIR)/main.elf $(BINDIR)/main.bin

$(OBJDIR)/pch.h.gch: src/pch/pch.h
	$(MKOBJDIR)
	$(CXX) $(CXXFLAGS) -x c++-header src/pch/pch.h -o $(OBJDIR)/pch.h.gch -MMD

%.o : %.c $(OBJDIR)/pch.h.gch
	$(MKOBJDIR)
	$(CC) $(CFLAGS) -Wno-unused-but-set-variable -Wno-unused-variable -MMD $(TARGET_ARCH) -c -o $@ $<

%.o : %.cpp $(OBJDIR)/pch.h.gch
	$(MKOBJDIR)
	$(CXX) $(CXXFLAGS) -MMD $(TARGET_ARCH) -c -o $@ $<

$(OBJDIR)/libstm32f4xx.a: $(LIB_OBJS)
	$(AR) cr $(OBJDIR)/libstm32f4xx.a $^

$(OBJDIR)/startup_stm32f4xx.o:
	$(AS) -o $(OBJDIR)/startup_stm32f4xx.o $(ASFLAGS) $(STARTUP_ASM)

clean:
	$(REMOVE) $(OBJS) $(LIB_OBJS) $(DEPS) $(LIB_DEPS)
