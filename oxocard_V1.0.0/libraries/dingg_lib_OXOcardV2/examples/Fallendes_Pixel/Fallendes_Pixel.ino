#include "OXOcardRunnerV2.h"

int i;

void setup() {
  i = 0;
}

void loop() {
  oxocard.matrix->clearScreen();
  oxocard.matrix->setForeColor(Random::getColor());
  oxocard.matrix->drawPixel(3, i);
  delay(50);
  if (i < 7) {
    i ++;
  } else {
    i = 0;
  }
}

