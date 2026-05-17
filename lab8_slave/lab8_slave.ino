#define F_CPU 16000000UL
#include <avr/io.h>

#define SLAVE_ADDR 0x08

volatile uint8_t state = 0x00;

void setup() {
  TWAR = (SLAVE_ADDR << 1); // set address to the SLAVE_ADDR. Offset by 1 bit because LSM of TWAR is GSE.
  TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); // Enable TWI, send ACK, write 1 to TWINT to continue

  PORTC |= (1 << PC4) | (1 << PC5);

  DDRB |= (1 << PB5); // use onboard LED on PB5
  DDRD &= ~(1 << PD2); // button is on PD2
  PORTD |= (1 << PD2);
}

void loop() {
      if (!(PIND & (1 << PD2))) { // check if button is pressed
          state = 0x02;
      }

      if (TWCR & (1 << TWINT)) {
          uint8_t status = TWSR & 0b11111000; // mask 3 LSB of the TWSR (they are reserved 0 and prescalers)

          if (status == 0x60) { // received own ADDR + Write
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
          } else if (status == 0x80) { // received data after receiving own address
              uint8_t received = TWDR; // read TWDR
              if (received == 0x01) { // check if the received protocol message is to turn LED on
                  PORTB |= (1 << PB5);
              } else {
                  PORTB &= ~(1 << PB5);
              }
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
          } else if (status == 0xA8) { // received own ADDR + Read
              TWDR = state; // send current state
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
              state = 0x00; // nullify the state after sending it
          } else { // in any other signal, ignore it, send ACK to continue communication and set TWINT to 1 to proceed
              TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
          }
      }
}