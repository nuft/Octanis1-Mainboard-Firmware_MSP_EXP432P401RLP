#
# settings
################################################################################

PROJNAME = octanis1

BUILD_DIR = build

# dont show compiler calls, comment out for verbose build
# Q = @

# source file list
include source.mk

COBJS = $(CSRC:.c=.o)
CPPOBJS = $(CPPSRC:.cpp=.o)
ASMOBJS = $(ASMSRC:.s=.o)

OBJS = $(COBJS) $(CPPOBJS) $(ASMOBJS)

OPT = -O0 -ggdb -gdwarf-3 -gstrict-dwarf

TIRTOS_PATH = /Applications/ti/tirtos_msp43x_2_14_03_28
CCSV6_PATH =  /Applications/ti/ccsv6
XDCTOOLS_CORE_PATH = /Applications/ti/xdctools_3_31_00_24_core

# include paths
INCDIR = \
	-I./Debug \
	-I$(CCSV6_PATH)/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/arm-none-eabi/include \
	-I$(CCSV6_PATH)/ccs_base \
	-I$(CCSV6_PATH)/ccs_base/arm/include \
	-I$(CCSV6_PATH)/ccs_base/arm/include/CMSIS \
	-I$(TIRTOS_PATH)/products/bios_6_42_03_35/packages \
	-I$(TIRTOS_PATH)/products/uia_2_00_02_39/packages \
	-I$(TIRTOS_PATH)/packages/ \
	-I$(TIRTOS_PATH)/products/MSPWare_2_00_00_40c/driverlib/MSP432P4xx \
	-I$(XDCTOOLS_CORE_PATH)/packages

# defines
DEFS += -D__MSP432P401R__ -DTARGET_IS_FALCON -Dgcc -DMSP432WARE -Dtimegm=mktime \
	-Dxdc_target_types__=$(TIRTOS_PATH)/products/bios_6_42_03_35/packages/gnu/targets/arm/std.h \
	-Dxdc_target_name__=M4F \
	-Dxdc_cfg__xheader__="\"configPkg/package/cfg/app_pm4fg.h\""

LDSCRIPTS = MSP_EXP432P401RLP.lds Debug/configPkg/linker.cmd
LLDSCRIPTS = $(addprefix  -T, $(LDSCRIPTS))

# libraries
LIBDIR = -L$(TIRTOS_PATH)/products/MSPWare_2_00_00_40c/driverlib/MSP432P4xx/gcc
LIBS = -Wl,--start-group -l:msp432p4xx_driverlib.a -lgcc -lg -lc -lm -lnosys -lrdimon -Wl,--end-group

# Cortex-M4f
OPT += -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

# compiler flags
CFLAGS   += -ffunction-sections -fdata-sections -fomit-frame-pointer
#-fno-common
CFLAGS   += -Wall -Wextra -Wno-unused-parameter
#-Wstrict-prototypes
# CFLAGS   += -MD -MP -MF # dependency info
CFLAGS   += $(DEFS) $(INCDIR)
# C only flags
CCFLAGS  += $(OPT) $(CFLAGS) -std=c99
# C++ only flags
CPPFLAGS = $(OPT) $(CFLAGS) -fno-rtti -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit
# Assembler flags
ASFLAGS = $(OPT) $(DEFS)
# Linker flags
LDFLAGS += $(OPT) $(LLDSCRIPTS) -nostartfiles -Wl,-Map=$(PROJNAME).map
LDFLAGS += -Wl,--gc-sections -static
# objcopy flags
CPFLAGS = -Obinary -j .text -j .rodata -j .data

CC = arm-none-eabi-gcc
CPPC = arm-none-eabi-g++
AS = arm-none-eabi-gcc -x assembler-with-cpp
# use g++ for linking with cpp sources
LD = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
NM = arm-none-eabi-nm
SZ = arm-none-eabi-size

MKDIR = mkdir -p

COLOR = \033[1;31m
COLOR_CLEAR = \033[0m
PRINT = @printf "$(COLOR)%s$(COLOR_CLEAR)\n"

#
# targets:
################################################################################

.PHONY: all
all: $(PROJNAME).bin $(PROJNAME).list $(PROJNAME).size $(PROJNAME).addr Makefile
	$(PRINT) "> done: $(PROJNAME)"
	@ $(SZ) $(PROJNAME).elf

.PHONY: clean
clean:
	$(Q)-rm -f $(OBJS)
	$(Q)-rm -f $(OBJS:.o=.d)
	$(Q)-rm -f $(OBJS:.o=.lst)
	$(Q)-rm -f $(PROJNAME).elf
	$(Q)-rm -f $(PROJNAME).bin
	$(Q)-rm -f $(PROJNAME).list
	$(Q)-rm -f $(PROJNAME).map
	$(Q)-rm -f $(PROJNAME).size
	$(Q)-rm -f $(PROJNAME).addr

#
# file targets:
################################################################################

# binary
$(PROJNAME).bin: $(PROJNAME).elf
	$(PRINT) "> copying"
	$(Q) $(CP) $(CPFLAGS) ${<} ${@}

# assembly listing
$(PROJNAME).list: $(PROJNAME).elf
	$(PRINT) "> generating assembly listing"
	$(Q) $(OD) -d ${<} > ${@}

# linked elf-object
$(PROJNAME).elf: $(OBJS) $(LDSCRIPTS)
	$(PRINT) "> linking"
	$(Q) $(LD) $(LDFLAGS) $(OBJS) $(LIBDIR) $(LIBS) -o ${@}

# object from C
$(COBJS): %.o : %.c Makefile
	$(PRINT) "> compiling ("${<}")"
	$(Q) $(CC) -c $(CCFLAGS) -c ${<} -o ${@}

# object from C++
$(CPPOBJS): %.o : %.cpp Makefile
	$(PRINT) "> compiling ("${<}")"
	$(Q) $(CPPC) -c $(CPPFLAGS) -c ${<} -o ${@}

# object from assembly
$(ASMOBJS): %.o : %.s Makefile
	$(PRINT) "> assembling ("${<}")"
	$(Q) $(AS) -c $(ASFLAGS) -c ${<} -o ${@}

# symbol sizes
$(PROJNAME).size: $(PROJNAME).elf
	$(PRINT) "> ${@}"
	$(Q) $(NM) --size-sort --print-size ${<} > ${@}

# symbol addresses
$(PROJNAME).addr: $(PROJNAME).elf
	$(PRINT) "> ${@}"
	$(Q) $(NM) --numeric-sort --print-size ${<} > ${@}
