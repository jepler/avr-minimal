ifeq ($(origin CC),default)
undefine CC
endif
ifeq ($(origin CXX),default)
undefine CXX
endif

# Place your settings here, such as overriding DEV
-include Makefile.local

SRCS ?= main.cc serial-polling.c
DEV ?= /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A9007N2c-if00-port0
MCU ?= atmega328
AMCU ?= m328p
F_CPU ?= 16000000
PBAUD ?= 57600
CBAUD ?= 57600
AVRDUDE ?= avrdude -p$(AMCU) -c arduino -P $(DEV) -b$(PBAUD)

CC ?= avr-gcc
CXX ?= avr-gcc
CFLAGS ?= -O3 -W -Wall -Wextra
CFLAGS += -DF_CPU=$(F_CPU)ull -DBAUD=$(CBAUD)
CFLAGS += -mmcu=$(MCU)
CXXFLAGS ?= $(CFLAGS) -fno-exceptions -fno-rtti
LFLAGS ?= -mmcu=$(MCU)
LIBS ?=
SIZE ?= avr-size

# End of customizable items

C_SRCS := $(filter %.c, $(SRCS))
CXX_SRCS := $(filter %.cc, $(SRCS))
OBJS := $(patsubst %.c, .o/%.o, $(C_SRCS))
OBJS += $(patsubst %.cc, .o/%.o, $(CXX_SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

# Beautify output
# ---------------------------------------------------------------------------
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#       $(Q)ln $@ :<
#
# If BUILD_VERBOSE equals 0 then the above command will be hidden.
# If BUILD_VERBOSE equals 1 then the above command is displayed.

ifeq ("$(origin V)", "command line")
  BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
  BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
  Q =
else
  Q = @
endif

ifeq "$(findstring s,$(filter-out --%, $(MAKEFLAGS)))" ""
ECHO=@echo
VECHO=echo
else
ECHO=@true
VECHO=true
endif

ifeq ($(shell [ -e $(DEV) ] && echo 1),1)
default: program-stamp
else
default: a.elf
	$(ECHO) "Board not detected, skipping programming step"
endif

# note: the arduino bootloader can't program eeprom
program-stamp: program
	$(Q)touch $@

program: a.hex
	$(ECHO) PROG $<
	$(Q)$(AVRDUDE) -q -q -D -U flash:w:$<:i

communicate: | program
	$(Q)screen $(DEV) 57600

.PRECIOUS: %.hex
%.hex: %.elf
	$(ECHO) "HEX " $@
	$(Q)avr-objcopy -O ihex -R eeprom $< $@

.PRECIOUS: %.elf
a.elf: $(OBJS)
	$(ECHO) LINK $@
	$(Q)$(CC) -o $@ $(LFLAGS) $^ $(LIBS)
	$(Q)$(SIZE) $@

clean:	
	$(ECHO) CLEAN
	$(Q)rm -rf a.elf a.hex .o

.o/%.o: %.c
	$(ECHO) "CC  " $<
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -o $@ $(CFLAGS) -c $< \
		-MP -MD -MF "${@:.o=.d}" -MT "$@"

.o/%.o: %.cc
	$(ECHO) "CXX " $<
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) -o $@ $(CFLAGS) -c $< \
		-MP -MD -MF "${@:.o=.d}" -MT "$@"

.PHONY: default program clean communicate
-include $(DEPS)

# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
