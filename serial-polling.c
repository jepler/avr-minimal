#include <avr/io.h>
#include <stdio.h>
static int serial_putc(char c, FILE *stream) {
    if (c == '\n') {
        serial_putc('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

static int serial_getc(FILE *stream) {
    (void)stream;
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    int r = UDR0;
    serial_putc(r, NULL);
    return r;
}

__attribute__((constructor))
static void serial_install_stdio() {
    static FILE uart_stdio;
    fdev_setup_stream(&uart_stdio, serial_putc, serial_getc, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_stdio;
    
#ifndef BAUD
#define BAUD 57600
#endif
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

