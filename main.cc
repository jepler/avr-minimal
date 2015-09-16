#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define PIN_LED (5) // onboard LED is PORTD5

#define PUTS_PP(x, ...)   puts_P(PSTR(x), ## __VA_ARGS__)
#define PRINTF_PP(x, ...) printf_P(PSTR(x), ## __VA_ARGS__)
#define SCANF_PP(x, ...)  scanf_P(PSTR(x), ## __VA_ARGS__)

#define FREQ_BASE (120e6)

#define W_CLK (5) // PORTB5, SCK   = pin 13
#define FQ_UD (4) // PORTB4        = pin 12
#define DATA7 (3) // PORTB3, MOSI  = pin 11
#define TRIG  (2) // PORTB2        = pin 10

void setup() {
    // set up SPI interface @ 8MHz
    SPCR = (1<<SPE) | (1<<MSTR);
    SPSR = (1<<SPI2X); // fOSC/2
    // enable outputs
    DDRB = (1<<W_CLK) | (1<<FQ_UD) | (1<<DATA7) << (1<<TRIG);

    // strobe once to put device in serial mode
    PORTD |= (1<<FQ_UD);
    PORTD &= ~(1<<FQ_UD);
}

void shift_byte(unsigned char b) {
    loop_until_bit_is_clear(SPSR, SPIF);
    SPDR = b;
}

void set_freq_raw(unsigned long f) {
    PRINTF_PP("set_freq_raw %lu\n", f);
    shift_byte(0x0);
    shift_byte(f & 0xff); f >>= 8;
    shift_byte(f & 0xff); f >>= 8;
    shift_byte(f & 0xff); f >>= 8;
    shift_byte(f & 0xff);
    loop_until_bit_is_clear(SPSR, SPIF);
    PORTD |= (1<<FQ_UD);
    PORTD &= ~(1<<FQ_UD);
    // strobe FQ_UP
}

unsigned long raw_freq(unsigned long khz) {
    return (unsigned long)(khz * (1000. * 4294967295. / FREQ_BASE));
}

void set_freq(unsigned long khz) {
    set_freq_raw(raw_freq(khz));
}

void sweep(unsigned long lo, unsigned long hi, int steps) {
    lo = raw_freq(lo);
    hi = raw_freq(hi);
    // rounding error assumed unimportant
    unsigned long delta = (hi - lo) / steps;
    PORTB |= (1<<TRIG);  // trigger your scope
    for(int u = 0; u < steps; u++ ) {
        set_freq_raw(lo);
        lo += delta;
    }
    PORTB &= ~(1<<TRIG);
}

int main() {
    unsigned long freq, freq_end;
    int steps;

    DDRD |= (1<<PIN_LED);
usage:
    PUTS_PP(
        "f ### - set frequency ###kHz\n"
        "s #1# #2# #3# - sweep from frequency #1#kHz to #2#kHz in #3# steps"
    );
    while(1) {
        int op = getchar();
        switch(op) {
        case 'f':
            SCANF_PP("%lu", &freq);
            set_freq(freq); break;
        case 's':
            SCANF_PP("%lu %lu %i", &freq, &freq_end, &steps);
            PUTS_PP("press any key to cancel");
        // fallthrough
            do {
                sweep(freq, freq_end, steps);
            } while(!(UCSR0A & (1<<RXC0)));

            break;
        case '?':
            goto usage;
        }
    }
}

/*
Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.  This file is offered as-is,
without any warranty.
*/
