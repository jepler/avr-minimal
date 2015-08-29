#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define PIN_LED (5) // onboard LED is PORTD5


int main() {
    DDRD |= (1<<PIN_LED);
    printf_P(PSTR("HELLO AVR - press any key to toggle LED.  characters will be echoed\n"));
    while(1) { int c = getchar(); putchar(c); PINB = (1<<PIN_LED); }
}

/*
Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.  This file is offered as-is,
without any warranty.
*/
