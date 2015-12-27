#
# settings
################################################################################

PROJNAME = octanis1

BUILDDIR = build

XDC_CFG_DIR = configPkg

# comment out for verbose build
Q = @

# source file list
include source.mk

COBJS = $(CSRC:.c=.o)
CPPOBJS = $(CPPSRC:.cpp=.o)
ASMOBJS = $(ASMSRC:.s=.o)

OBJS = $(COBJS) $(CPPOBJS) $(ASMOBJS)

OUTFILES = $(BUILDDIR)/$(PROJNAME).elf \
		   $(BUILDDIR)/$(PROJNAME).bin \
		   $(BUILDDIR)/$(PROJNAME).list \
		   $(BUILDDIR)/$(PROJNAME).map \
		   $(BUILDDIR)/$(PROJNAME).size \
		   $(BUILDDIR)/$(PROJNAME).addr

OPT = -O0 -ggdb -gdwarf-3 -gstrict-dwarf
OPT += -Wno-old-style-declaration -Wno-unused-variable

TIRTOS_PATH = /Applications/ti/tirtos_msp43x_2_14_03_28
CCSV6_PATH =  /Applications/ti/ccsv6
XDC_PATH = /Applications/ti/xdctools_3_31_00_24_core
GCC_TOOLCHAIN_PATH = $(CCSV6_PATH)/tools/compiler/gcc-arm-none-eabi-4_8-2014q3

XDC_PATH_ARG = /Applications/ti/tirtos_msp43x_2_14_03_28/packages;/Applications/ti/tirtos_msp43x_2_14_03_28/products/bios_6_42_03_35/packages;/Applications/ti/tirtos_msp43x_2_14_03_28/products/uia_2_00_02_39/packages;/Applications/ti/ccsv6/ccs_base;
# XDC_PATH_ARG = $(TIRTOS_PATH)/packages;$(TIRTOS_PATH)/products/bios_6_42_03_35/packages;$(TIRTOS_PATH)/products/uia_2_00_02_39/packages;$(CCSV6_PATH)/ccs_base;

# XDC_CC = "/Users/mi/arm-toolchain/gcc-arm-none-eabi-4_9-2014q4"
XDC_CC = "$(GCC_TOOLCHAIN_PATH)"

# include paths
INCDIR = \
	-I./ \
	-I$(GCC_TOOLCHAIN_PATH)/arm-none-eabi/include \
	-I$(CCSV6_PATH)/ccs_base \
	-I$(CCSV6_PATH)/ccs_base/arm/include \
	-I$(CCSV6_PATH)/ccs_base/arm/include/CMSIS \
	-I$(TIRTOS_PATH)/products/bios_6_42_03_35/packages \
	-I$(TIRTOS_PATH)/products/uia_2_00_02_39/packages \
	-I$(TIRTOS_PATH)/packages/ \
	-I$(TIRTOS_PATH)/products/MSPWare_2_00_00_40c/driverlib/MSP432P4xx \
	-I$(XDC_PATH)/packages

# defines
DEFS += -D__MSP432P401R__ -DTARGET_IS_FALCON -Dgcc -DMSP432WARE -Dtimegm=mktime \
	-Dxdc_target_types__=$(TIRTOS_PATH)/products/bios_6_42_03_35/packages/gnu/targets/arm/std.h \
	-Dxdc_target_name__=M4F \
	-Dxdc_cfg__xheader__="\"$(XDC_CFG_DIR)/package/cfg/app_pm4fg.h\""

LDSCRIPT = MSP_EXP432P401RLP.lds

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
LDFLAGS += $(OPT) -nostartfiles -Wl,-Map=$(BUILDDIR)/$(PROJNAME).map
LDFLAGS += -Wl,--script=$(LDSCRIPT),--script=$(XDC_CFG_DIR)/linker.cmd
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

XDCARGS = --xdcpath="$(XDC_PATH_ARG)" xdc.tools.configuro -o configPkg \
		  -t gnu.targets.arm.M4F -p ti.platforms.msp432:MSP432P401R \
		  -r release -c $(XDC_CC) --compileOptions "$(OPT) $(DEFS) $(INCDIR) -std=c99"

COLOR = \033[1;34m
COLOR_CLEAR = \033[0m
PRINT = @printf "$(COLOR)%s$(COLOR_CLEAR)\n"

#
# targets:
################################################################################

.PHONY: all
all: $(XDC_CFG_DIR) $(BUILDDIR) $(OUTFILES) Makefile
	$(PRINT) "> done: $(PROJNAME)"
	@ $(SZ) $(BUILDDIR)/$(PROJNAME).elf

.PHONY: clean
clean:
	$(Q)-rm -f $(OBJS)
	$(Q)-rm -f $(OBJS:.o=.d)
	$(Q)-rm -f $(OBJS:.o=.lst)
	$(Q)-rm -rf $(BUILDDIR)
	$(Q)-rm -rf $(XDC_CFG_DIR)

#
# file targets:
################################################################################

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(XDC_CFG_DIR): app.cfg Makefile
	$(XDC_PATH)/xs $(XDCARGS) app.cfg
#	 "/Applications/ti/xdctools_3_31_00_24_core/xs" --xdcpath="/Applications/ti/tirtos_msp43x_2_14_03_28/packages;/Applications/ti/tirtos_msp43x_2_14_03_28/products/bios_6_42_03_35/packages;/Applications/ti/tirtos_msp43x_2_14_03_28/products/uia_2_00_02_39/packages;/Applications/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4F -p ti.platforms.msp432:MSP432P401R -r release -c "/Applications/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3" --compileOptions "-mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -D__MSP432P401R__ -DTARGET_IS_FALCON -Dgcc -DMSP432WARE -Dtimegm=mktime -I\"/Applications/ti/ccsv6/ccs_base/arm/include\" -I\"/Applications/ti/ccsv6/ccs_base/arm/include/CMSIS\" -I\"/Applications/ti/tirtos_msp43x_2_14_03_28/products/MSPWare_2_00_00_40c/driverlib/MSP432P4xx\" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall  " app.cfg

# binary
%.bin: %.elf
	$(PRINT) "> copying"
	$(Q) $(CP) $(CPFLAGS) ${<} ${@}

# assembly listing
%.list: %.elf
	$(PRINT) "> generating assembly listing"
	$(Q) $(OD) -d ${<} > ${@}

# linked elf-object
%.elf: $(OBJS) $(LDSCRIPT)
	$(PRINT) "> linking"
	$(Q) $(LD) $(OBJS) $(LDFLAGS) $(LIBDIR) $(LIBS) -o ${@}

# symbol sizes
%.size: %.elf
	$(PRINT) "> ${@}"
	$(Q) $(NM) --size-sort --print-size ${<} > ${@}

# symbol addresses
%.addr: %.elf
	$(PRINT) "> ${@}"
	$(Q) $(NM) --numeric-sort --print-size ${<} > ${@}

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
