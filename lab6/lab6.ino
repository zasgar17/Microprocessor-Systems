#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t digit = 0;
volatile uint8_t paused = 0;

const uint8_t seg[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static inline void show_digit(uint8_t n) {
    uint8_t x = seg[n];

    PORTB = (PORTB & ~0x3F) | (x & 0x3F);      // a-f -> D8-D13
    PORTC = (PORTC & ~0x01) | ((x >> 6) & 1); // g   -> A0
}

static inline void timer1_start(void) {
    TCCR1B = (1 << WGM12) | (1 << CS12);      // CTC mode, prescaler = 256
}

static inline void timer1_stop(void) {
    TCCR1B = (1 << WGM12);                    // keep CTC mode, stop clock
}

ISR(TIMER1_COMPA_vect) {
    if (!paused) {
        digit = (digit + 1) % 10;
        show_digit(digit);
    }
}

ISR(INT0_vect) {
    paused ^= 1;

    if (paused) {
        timer1_stop();
    } else {
        timer1_start();
    }
}

int main(void) {
    DDRB |= 0x3F;          // D8-D13 output for a-f
    DDRC |= (1 << PC0);    // A0 output for g

    DDRD &= ~(1 << PD2);   // D2 input for button
    PORTD |= (1 << PD2);   // internal pull-up

    show_digit(digit);

    TCCR1A = 0;
    timer1_start();
    OCR1A  = 31249;
    TIMSK1 = (1 << OCIE1A);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);

    sei();

    while (1) { }
}
