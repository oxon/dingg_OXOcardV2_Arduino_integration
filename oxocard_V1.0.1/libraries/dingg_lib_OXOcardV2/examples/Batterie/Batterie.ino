#include "OXOcardRunnerV2.h"

float vBat;

void setup() {

}

void loop() {
  vBat = oxocard.battery->getVoltage();
  if (vBat > 3.8) {
    oxocard.matrix->clearScreen();
    oxocard.matrix->setPixel(3, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 2, rgb(51, 204, 0));
    oxocard.matrix->setPixel(3, 2, rgb(51, 204, 0));
    oxocard.matrix->setPixel(4, 2, rgb(51, 204, 0));
    oxocard.matrix->setPixel(5, 2, rgb(51, 204, 0));
    oxocard.matrix->setPixel(6, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 3, rgb(51, 204, 0));
    oxocard.matrix->setPixel(3, 3, rgb(51, 204, 0));
    oxocard.matrix->setPixel(4, 3, rgb(51, 204, 0));
    oxocard.matrix->setPixel(5, 3, rgb(51, 204, 0));
    oxocard.matrix->setPixel(6, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 4, rgb(51, 204, 0));
    oxocard.matrix->setPixel(3, 4, rgb(51, 204, 0));
    oxocard.matrix->setPixel(4, 4, rgb(51, 204, 0));
    oxocard.matrix->setPixel(5, 4, rgb(51, 204, 0));
    oxocard.matrix->setPixel(6, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 5, rgb(51, 204, 0));
    oxocard.matrix->setPixel(3, 5, rgb(51, 204, 0));
    oxocard.matrix->setPixel(4, 5, rgb(51, 204, 0));
    oxocard.matrix->setPixel(5, 5, rgb(51, 204, 0));
    oxocard.matrix->setPixel(6, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 6, rgb(51, 204, 0));
    oxocard.matrix->setPixel(3, 6, rgb(51, 204, 0));
    oxocard.matrix->setPixel(4, 6, rgb(51, 204, 0));
    oxocard.matrix->setPixel(5, 6, rgb(51, 204, 0));
    oxocard.matrix->setPixel(6, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 7, rgb(0, 0, 153));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
  } else if (vBat > 3.6) {
    oxocard.matrix->clearScreen();
    oxocard.matrix->setPixel(3, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 3, rgb(255, 255, 0));
    oxocard.matrix->setPixel(3, 3, rgb(255, 255, 0));
    oxocard.matrix->setPixel(4, 3, rgb(255, 255, 0));
    oxocard.matrix->setPixel(5, 3, rgb(255, 255, 0));
    oxocard.matrix->setPixel(6, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 4, rgb(255, 255, 0));
    oxocard.matrix->setPixel(3, 4, rgb(255, 255, 0));
    oxocard.matrix->setPixel(4, 4, rgb(255, 255, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 255, 0));
    oxocard.matrix->setPixel(6, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 5, rgb(255, 255, 0));
    oxocard.matrix->setPixel(3, 5, rgb(255, 255, 0));
    oxocard.matrix->setPixel(4, 5, rgb(255, 255, 0));
    oxocard.matrix->setPixel(5, 5, rgb(255, 255, 0));
    oxocard.matrix->setPixel(6, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 6, rgb(255, 255, 0));
    oxocard.matrix->setPixel(3, 6, rgb(255, 255, 0));
    oxocard.matrix->setPixel(4, 6, rgb(255, 255, 0));
    oxocard.matrix->setPixel(5, 6, rgb(255, 255, 0));
    oxocard.matrix->setPixel(6, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 7, rgb(0, 0, 153));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
  } else if (vBat > 3.4) {
    oxocard.matrix->clearScreen();
    oxocard.matrix->setPixel(3, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(3, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(4, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(3, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(4, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 6, rgb(255, 204, 0));
    oxocard.matrix->setPixel(3, 6, rgb(255, 204, 0));
    oxocard.matrix->setPixel(4, 6, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 6, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 7, rgb(0, 0, 153));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
  } else if (vBat <= 3.4) {
    oxocard.matrix->clearScreen();
    oxocard.matrix->setPixel(3, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 0, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 1, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 2, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 3, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 4, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 5, rgb(255, 0, 0));
    oxocard.matrix->setPixel(3, 5, rgb(255, 0, 0));
    oxocard.matrix->setPixel(4, 5, rgb(255, 0, 0));
    oxocard.matrix->setPixel(5, 5, rgb(255, 0, 0));
    oxocard.matrix->setPixel(6, 5, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 6, rgb(255, 0, 0));
    oxocard.matrix->setPixel(3, 6, rgb(255, 0, 0));
    oxocard.matrix->setPixel(4, 6, rgb(255, 0, 0));
    oxocard.matrix->setPixel(5, 6, rgb(255, 0, 0));
    oxocard.matrix->setPixel(6, 6, rgb(0, 0, 153));
    oxocard.matrix->setPixel(1, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(2, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(3, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(4, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(5, 7, rgb(0, 0, 153));
    oxocard.matrix->setPixel(6, 7, rgb(0, 0, 153));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
  }
  delay(5000);
}

