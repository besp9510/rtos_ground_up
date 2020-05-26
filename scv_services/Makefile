# Compiler:
CC := arm-none-eabi-gcc

# Translate ELF to hex and binary:
OC := arm-none-eabi-objcopy

# Common build file name:
TARGET := program

# Root directories:
ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# Directories:
SRCDIR    := $(ROOT)/src
INCDIR    := $(ROOT)/include
BUILDDIR  := $(ROOT)/obj
TARGETDIR := $(ROOT)/build
RESDIR    := $(ROOT)/res

# Extensions:
C_SRCEXT   := c
ASM_SRCEXT := s
DEPEXT     := d
OBJEXT     := o

# Flag options:
CPU       := -mcpu=cortex-m4
FPU       := -mfpu=fpv4-sp-d16
FLOAT-ABI := -mfloat-abi=hard
MCU       := $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
C_DEFS    := -DSTM32F411xE

# Libraries:
LIB :=

# Compile and link flags:
CFLAGS  := $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -g -fdata-sections -ffunction-sections -Og -O0
LDFLAGS := -specs=nosys.specs -TSTM32F411VETx_FLASH.ld $(MCU)

# Include paths:
INC := -I$(INCDIR) -I/usr/local/include/ -I/usr/local/include/cmsis/ \
	-I/usr/local/include/stm32f4xx_peripheral_drivers/

INCDEP := -I$(INCDIR)

# Find source files:
C_SOURCES := $(shell find $(SRCDIR) -type f -name *.$(C_SRCEXT))
ASM_SOURCES := $(shell find $(SRCDIR) -type f -name *.$(ASM_SRCEXT))

# Find object files:
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%, $(C_SOURCES:.$(C_SRCEXT)=.$(OBJEXT))) \
	$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%, $(ASM_SOURCES:.$(ASM_SRCEXT)=.$(OBJEXT)))

# -------------------------------------------------------------------------- #
# Rules (DO NOT EDIT)
# -------------------------------------------------------------------------- #

# Default make:
all: $(TARGET).elf $(TARGET).bin $(TARGET).hex

# Remake:
remake: cleaner all

# Make the directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# Clean objects only:
clean:
	@$(RM) -rf $(BUILDDIR)

# Full clean (objects and binaries):
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

# Pull in dependency info for *existing* .o files:
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Translate to binary:
$(TARGET).bin: $(TARGETDIR)/$(TARGET).elf
	$(OC) -O binary $^ $(TARGETDIR)/$(TARGET).bin

# Translate to hex:
$(TARGET).hex: $(TARGETDIR)/$(TARGET).elf
	$(OC) -O ihex $^ $(TARGETDIR)/$(TARGET).hex

# Link:
$(TARGET).elf: $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET).elf $^ $(LIB) $(CFLAGS) $(LDFLAGS)

# Compile C:
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(C_SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -Wall -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(C_SRCEXT) > \
		$(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' \
		< $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp \
		| fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# Compile assembly:
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(ASM_SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -Wall -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(ASM_SRCEXT) > \
		$(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' \
		< $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp \
		| fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# Non-file targets:
.PHONY: all remake clean cleaner