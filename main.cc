#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define PIN_LED (5) // onboard LED is PORTD5

static int serial_putc(char c, FILE *stream) {
    if (c == '\n') {
        serial_putc('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

static int serial_getc(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}

void serial_install_stdio() {
    static FILE uart_stdio;
    fdev_setup_stream(&uart_stdio, serial_putc, serial_getc, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_stdio;
    
#define BAUD 57600
#include <util/setbaud.h>
    UBRR0 = UBRR_VALUE;
#if USE_2X    
    UCSR0A |= (1<<U2X0);
#else
    UCSR0A &= ~(1<<U2X0);
#endif
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);   /* enable transmitter and receiver */
}

int main() {
    serial_install_stdio();
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
