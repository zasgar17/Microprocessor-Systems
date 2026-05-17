void SPI_init_slave() {
  // MISO → OUTPUT
  DDRB |= (1 << PB4);

  // MOSI, SCK, SS → INPUT
  DDRB &= ~((1 << PB3) | (1 << PB5) | (1 << PB2));

  // Enable SPI
  SPCR = (1 << SPE);
}

void setup() {
  Serial.begin(9600);
  SPI_init_slave();
}

void loop() {
  if (SPSR & (1 << SPIF)) {
    uint8_t received = SPDR;
    Serial.println(received);
  }
}