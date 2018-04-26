#include "OXOcardRunnerV2.h"

int i;

void setup() {

}

void loop() {
  for (i = 1; i <= 750; i++) {
    if (i <= 250) {
      oxocard.matrix->setForeColor(rgb(250 - i, i, 0));
    }
    if (i <= 500 && i > 250) {
      oxocard.matrix->setForeColor(rgb(0, 500 - i, i - 250));
    }
    if (i <= 750 && i > 500) {
      oxocard.matrix->setForeColor(rgb(i - 500, 0, 750 - i));
    }
    oxocard.matrix->fillScreen();
    delay(10);
  }
}

