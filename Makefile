DEV := /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A9007N2c-if00-port0
MCU := atmega328
AMCU := m328p
AVRDUDE := avrdude
AVRDUDE += -p${AMCU} -c arduino -P ${DEV} -b57600

# Place your settings here, such as overriding DEV
-include Makefile.local

ifeq ($(shell [ -e ${DEV} ] && echo 1),1)
default: program-stamp
else
default: main.elf
	echo "Board not detected, skipping programming step"
endif

# note: the arduino bootloader can't program eeprom
program-stamp: program
	touch $@

program: main.hex
	${AVRDUDE} -q -q -D -U flash:w:main.hex:i

communicate:
	screen ${DEV} 57600

.PRECIOUS: %.hex
%.hex: %.elf
	avr-objcopy -O ihex -R eeprom $< $@

.PRECIOUS: %.elf
%.elf: %.cc
	avr-gcc -funit-at-a-time -finline-functions-called-once -fno-exceptions -fno-rtti -mmcu=${MCU} -DF_CPU=16000000ull -O3 -g $< -o $@

clean:	
	rm -f main.elf main.hex

.PHONY: default program clean communicate

# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
