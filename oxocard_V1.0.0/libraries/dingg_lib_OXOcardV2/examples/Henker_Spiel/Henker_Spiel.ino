#include "OXOcardRunnerV2.h"

int Z_C3_A4hler;

void setup() {
  Z_C3_A4hler = 0;
}

void loop() {
  if (Z_C3_A4hler == 0) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 1, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 3, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    delay(3000);
    oxocard.matrix->drawText("Druecke ersten linken Knopf ", false);
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 1) {
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 2) {
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 3) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 4) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 5) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 1, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 6) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 1, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 3, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 7) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 1, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 3, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 8) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 1, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 3, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 9) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 1, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 3, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 10) {
    oxocard.matrix->setPixel(1, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(2, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(3, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 0, rgb(102, 0, 0));
    oxocard.matrix->setPixel(1, 1, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 1, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 2, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 2, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 3, rgb(102, 0, 0));
    oxocard.matrix->setPixel(5, 3, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 4, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(5, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 4, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 5, rgb(102, 0, 0));
    oxocard.matrix->setPixel(4, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(6, 5, rgb(255, 204, 0));
    oxocard.matrix->setPixel(1, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 6, rgb(0, 153, 0));
    oxocard.matrix->setPixel(0, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(1, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(2, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(3, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(4, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(5, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(6, 7, rgb(0, 153, 0));
    oxocard.matrix->setPixel(7, 7, rgb(0, 153, 0));
    if (oxocard.matrix->isAutoUpdate()) {
      oxocard.matrix->update();
    }
    Z_C3_A4hler ++;
    while (oxocard.button->isPressed(L1)) {
      delay(5);
    }
  } else if (oxocard.button->isPressed(L1) && Z_C3_A4hler == 11) {
    for (int count = 0; count < 10; count++) {
      oxocard.matrix->setPixel(0, 0, rgb(255, 0, 0));
      oxocard.matrix->setPixel(7, 0, rgb(255, 0, 0));
      oxocard.matrix->setPixel(1, 1, rgb(255, 0, 0));
      oxocard.matrix->setPixel(6, 1, rgb(255, 0, 0));
      oxocard.matrix->setPixel(2, 2, rgb(255, 0, 0));
      oxocard.matrix->setPixel(5, 2, rgb(255, 0, 0));
      oxocard.matrix->setPixel(3, 3, rgb(255, 0, 0));
      oxocard.matrix->setPixel(4, 3, rgb(255, 0, 0));
      oxocard.matrix->setPixel(3, 4, rgb(255, 0, 0));
      oxocard.matrix->setPixel(4, 4, rgb(255, 0, 0));
      oxocard.matrix->setPixel(2, 5, rgb(255, 0, 0));
      oxocard.matrix->setPixel(5, 5, rgb(255, 0, 0));
      oxocard.matrix->setPixel(1, 6, rgb(255, 0, 0));
      oxocard.matrix->setPixel(6, 6, rgb(255, 0, 0));
      oxocard.matrix->setPixel(0, 7, rgb(255, 0, 0));
      oxocard.matrix->setPixel(7, 7, rgb(255, 0, 0));
      if (oxocard.matrix->isAutoUpdate()) {
        oxocard.matrix->update();
      }
      delay(50);
      oxocard.matrix->clearScreen();
      delay(50);
      Z_C3_A4hler = 0;
      while (oxocard.button->isPressed(L1)) {
        delay(5);
      }
    }
  }
}

