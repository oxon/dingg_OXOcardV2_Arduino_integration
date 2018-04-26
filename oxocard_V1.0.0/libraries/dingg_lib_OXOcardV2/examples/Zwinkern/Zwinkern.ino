#include "OXOcardRunnerV2.h"

void setup() {
  oxocard.matrix->setForeColor(rgb(51, 51, 255));
}

void loop() {
  oxocard.matrix->clearScreen();
  oxocard.matrix->drawImage(
    0b00000000,
    0b00100100,
    0b00100100,
    0b00000000,
    0b00000000,
    0b01000010,
    0b00111100,
    0b00000000, 0, 0);
  delay(1000);
  oxocard.matrix->clearScreen();
  oxocard.matrix->drawImage(
    0b00000000,
    0b00100000,
    0b00100100,
    0b00000000,
    0b00000000,
    0b01000010,
    0b00111100,
    0b00000000, 0, 0);
  delay(250);
}

