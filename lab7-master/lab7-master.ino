void SPI_init_master() {
  // MOSI, SCK, SS → OUTPUT
  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);

  // MISO → INPUT
  DDRB &= ~(1 << PB4);

  // Enable SPI, Master mode, Clock = Fosc/16
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

  // SS HIGH (inactive)
  PORTB |= (1 << PB2);
}

void SPI_send(uint8_t data) {
  SPDR = data;                    // load data
  while (!(SPSR & (1 << SPIF)));  // wait until sent
}

void setup() {
  SPI_init_master();
}

void loop() {
  uint8_t values[3] = {12, 45, 0};

  for (int i = 0; i < 3; i++) {
    PORTB &= ~(1 << PB2);   // SS LOW → start

    SPI_send(values[i]);

    PORTB |= (1 << PB2);    // SS HIGH → end

    delay(1000);
  }
}