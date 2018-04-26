#include "OXOcardRunnerV2.h"

int x;
int y;

void setup() {

}

void loop() {
  x = Random::getInt(0, 3+1);
  y = Random::getInt(0, 3+1);
  oxocard.matrix->setForeColor(Random::getColor());
  oxocard.matrix->drawPixel(x, y);
  oxocard.matrix->drawPixel(x, 7 - y);
  oxocard.matrix->drawPixel(7 - x, y);
  oxocard.matrix->drawPixel(7 - x, 7 - y);
  delay(50);
}

