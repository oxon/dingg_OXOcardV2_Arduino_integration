#include "OXOcardRunnerV2.h"

int x;
int y;
int setColorX;
int setColorY;
int r;
int g;
int b;

void initFarbauswahl();
void waehleFarbe();
void bewegePixel();

// Beschreibe diese Funktion …
void initFarbauswahl() {
  setColorX = 6;
  setColorY = 4;
  oxocard.matrix->setActiveFrameBuffer(3, false);
  oxocard.matrix->setPixel(1, 0, rgb(255, 204, 204));
  oxocard.matrix->setPixel(2, 0, rgb(255, 102, 102));
  oxocard.matrix->setPixel(3, 0, rgb(255, 0, 0));
  oxocard.matrix->setPixel(4, 0, rgb(204, 0, 0));
  oxocard.matrix->setPixel(5, 0, rgb(153, 0, 0));
  oxocard.matrix->setPixel(6, 0, rgb(102, 0, 0));
  oxocard.matrix->setPixel(7, 0, rgb(51, 0, 0));
  oxocard.matrix->setPixel(1, 1, rgb(255, 204, 153));
  oxocard.matrix->setPixel(2, 1, rgb(255, 153, 102));
  oxocard.matrix->setPixel(3, 1, rgb(255, 153, 0));
  oxocard.matrix->setPixel(4, 1, rgb(255, 102, 0));
  oxocard.matrix->setPixel(5, 1, rgb(204, 102, 0));
  oxocard.matrix->setPixel(6, 1, rgb(153, 51, 0));
  oxocard.matrix->setPixel(7, 1, rgb(102, 51, 0));
  oxocard.matrix->setPixel(1, 2, rgb(255, 255, 204));
  oxocard.matrix->setPixel(2, 2, rgb(255, 255, 51));
  oxocard.matrix->setPixel(3, 2, rgb(255, 255, 0));
  oxocard.matrix->setPixel(4, 2, rgb(255, 204, 0));
  oxocard.matrix->setPixel(5, 2, rgb(153, 153, 0));
  oxocard.matrix->setPixel(6, 2, rgb(102, 102, 0));
  oxocard.matrix->setPixel(7, 2, rgb(51, 51, 0));
  oxocard.matrix->setPixel(1, 3, rgb(153, 255, 153));
  oxocard.matrix->setPixel(2, 3, rgb(102, 255, 153));
  oxocard.matrix->setPixel(3, 3, rgb(51, 255, 51));
  oxocard.matrix->setPixel(4, 3, rgb(51, 204, 0));
  oxocard.matrix->setPixel(5, 3, rgb(0, 153, 0));
  oxocard.matrix->setPixel(6, 3, rgb(0, 102, 0));
  oxocard.matrix->setPixel(7, 3, rgb(0, 51, 0));
  oxocard.matrix->setPixel(1, 4, rgb(204, 255, 255));
  oxocard.matrix->setPixel(2, 4, rgb(102, 255, 255));
  oxocard.matrix->setPixel(3, 4, rgb(51, 204, 255));
  oxocard.matrix->setPixel(4, 4, rgb(51, 102, 255));
  oxocard.matrix->setPixel(5, 4, rgb(51, 51, 255));
  oxocard.matrix->setPixel(6, 4, rgb(0, 0, 153));
  oxocard.matrix->setPixel(7, 4, rgb(0, 0, 102));
  oxocard.matrix->setPixel(1, 5, rgb(204, 204, 255));
  oxocard.matrix->setPixel(2, 5, rgb(153, 153, 255));
  oxocard.matrix->setPixel(3, 5, rgb(102, 102, 204));
  oxocard.matrix->setPixel(4, 5, rgb(102, 51, 255));
  oxocard.matrix->setPixel(5, 5, rgb(102, 0, 204));
  oxocard.matrix->setPixel(6, 5, rgb(51, 51, 153));
  oxocard.matrix->setPixel(7, 5, rgb(51, 0, 153));
  oxocard.matrix->setPixel(1, 6, rgb(255, 204, 255));
  oxocard.matrix->setPixel(2, 6, rgb(255, 153, 255));
  oxocard.matrix->setPixel(3, 6, rgb(204, 102, 204));
  oxocard.matrix->setPixel(4, 6, rgb(204, 51, 204));
  oxocard.matrix->setPixel(5, 6, rgb(153, 51, 153));
  oxocard.matrix->setPixel(6, 6, rgb(102, 51, 102));
  oxocard.matrix->setPixel(7, 6, rgb(51, 0, 51));
  if (oxocard.matrix->isAutoUpdate()) {
    oxocard.matrix->update();
  }
  r = oxocard.matrix->readPixel(setColorX, setColorY).r;
  g = oxocard.matrix->readPixel(setColorX, setColorY).g;
  b = oxocard.matrix->readPixel(setColorX, setColorY).b;
}

// Beschreibe diese Funktion …
void waehleFarbe() {
  while (!(oxocard.button->isPressed(R1))) {
    if (oxocard.button->isPressed(L1)) {
      setColorY --;
      if (setColorY < 0) {
        setColorY = 6;
      }
    } else if (oxocard.button->isPressed(L2)) {
      setColorY ++;
      if (setColorY > 6) {
        setColorY = 0;
      }
    } else if (oxocard.button->isPressed(R2)) {
      setColorX ++;
      if (setColorX > 7) {
        setColorX = 1;
      }
    } else if (oxocard.button->isPressed(R3)) {
      setColorX --;
      if (setColorX < 1) {
        setColorX = 7;
      }
    }
    r = oxocard.matrix->readPixel(setColorX, setColorY).r;
    g = oxocard.matrix->readPixel(setColorX, setColorY).g;
    b = oxocard.matrix->readPixel(setColorX, setColorY).b;
    oxocard.matrix->setForeColor(rgb(r, g, b));
    oxocard.matrix->setActiveFrameBuffer(3, false);
    oxocard.matrix->drawPixel(0, setColorY);
    oxocard.matrix->drawPixel(setColorX, 7);
    oxocard.matrix->update();
    delay(50);
    oxocard.matrix->clearPixel(0, setColorY);
    oxocard.matrix->clearPixel(setColorX, 7);
    oxocard.matrix->update();
    delay(50);
  }
}

// Bewegt das Pixel mit den Knöpfen L1, L2, R2 & R3
void bewegePixel() {
  if (oxocard.button->isPressed(L1)) {
    y --;
    if (y < 0) {
      y = 7;
    }
  } else if (oxocard.button->isPressed(L2)) {
    y ++;
    if (y > 7) {
      y = 0;
    }
  } else if (oxocard.button->isPressed(R2)) {
    x ++;
    if (x > 7) {
      x = 0;
    }
  } else if (oxocard.button->isPressed(R3)) {
    x --;
    if (x < 0) {
      x = 7;
    }
  }
}


void setup() {
  x = 0;
  y = 0;
  oxocard.matrix->disableAutoUpdate();
  initFarbauswahl();
  oxocard.matrix->setActiveFrameBuffer(2, false);
}

void loop() {
  bewegePixel();
  if (oxocard.button->isPressed(L3)) {
    waehleFarbe();
  }
  if (oxocard.button->isPressed(R1)) {
    oxocard.matrix->setForeColor(rgb(r, g, b));
    oxocard.matrix->setActiveFrameBuffer(2, false);
    if (oxocard.matrix->isPixel(x, y)) {
      oxocard.matrix->clearPixel(x, y);
    } else {
      oxocard.matrix->drawPixel(x, y);
    }
    delay(100);
  }
  oxocard.matrix->copyFrameBufferContent(1, 2);
  if (oxocard.matrix->isPixel(x, y)) {
    oxocard.matrix->setForeColor(rgb(r, g, b));
  } else {
    oxocard.matrix->setForeColor(rgb(255, 0, 0));
  }
  oxocard.matrix->setActiveFrameBuffer(1, false);
  oxocard.matrix->drawPixel(x, y);
  oxocard.matrix->update();
  delay(50);
  oxocard.matrix->clearPixel(x, y);
  oxocard.matrix->update();
  delay(50);
}

