#include "OXOcardRunnerV2.h"

void setup() {
  oxocard.matrix->drawImage(
    0b00000000,
    0b00100100,
    0b00100100,
    0b00000000,
    0b00000000,
    0b01000010,
    0b00111100,
    0b00000000, 0, 0);
  delay(10000);
  oxocard.matrix->clearScreen();

}

void loop() {
  
}

