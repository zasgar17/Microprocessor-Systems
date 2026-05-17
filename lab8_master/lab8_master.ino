#define F_CPU 16000000UL // CPU frequency for calculations
#define SLAVE_ADDR 0x08 // we only have 1 slave, therefore we only need 1 slave address. I chose 0x08 for conciseness

/* PROTOCOL VALUES:
    0x00 - do nothing
    0x01 - send
    0x02 - receive */

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

void TWI_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // send START
    while (!(TWCR & (1 << TWINT))); // wait for confirmation that START was sent
}

void TWI_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // send STOP
}

void TWI_write(uint8_t data) {
    TWDR = data; // set TWDR to the data that is to be sent over the bus
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

uint8_t TWI_read_nack() {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // wait for confirmation
    return TWDR; // return received data
}

void send_to_slave(uint8_t data) {
    TWI_start();
    TWI_write((SLAVE_ADDR << 1) | 0); // select address of the slave that we are sending to
    TWI_write(data); // send the data
    TWI_stop();
}

uint8_t request_from_slave() {
    uint8_t data;
    TWI_start();
    TWI_write((SLAVE_ADDR << 1) | 1); // select slave to be read from
    data = TWI_read_nack(); // read data from the slave
    TWI_stop();
    return data;
}

void setup() {
    TWSR = 0x00; // prescaler 1
    TWBR = 12; // with prescaler 1, SCL frequency is 100kHz
    TWCR = (1 << TWEN); // enable I2C

    PORTC |= (1 << PC4) | (1 << PC5); // enable pull-ups on SDA and SCL

    DDRB |= (1 << PB5); // use onboard LED
    DDRD &= ~(1 << PD2); // button (PD0 and PD1 are reseved for RX and TX)
    PORTD |= (1 << PD2);
}

void loop() {
    if (!(PIND & (1 << PD2))) {
        send_to_slave(0x01);
    } else {
        send_to_slave(0x00);
    }

    uint8_t received = request_from_slave(); // listen to the slave

    if (received == 0x02) { // if received value from the slave is 2, then light up LED
        PORTB |= (1 << PB5);
    } else {
        PORTB &= ~(1 << PB5);
    }

    _delay_ms(10);
}