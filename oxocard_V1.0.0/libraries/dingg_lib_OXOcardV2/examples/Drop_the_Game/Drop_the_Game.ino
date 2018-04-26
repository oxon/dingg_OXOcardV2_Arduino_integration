#include "OXOcardRunnerV2.h"

int xLuecke2;
int yLuecke;
int xLueckeAlt;
int xLuecke;
int x;
int y;
int lueckenGroesse;
int tempo;
int tempoSteigernNach;
int tempoSteigernUm;
int tempoSteigernZaehler;
int iy;
int ix;
int R;
int G;
int B;

void zeichneNeueLinie();
void zeichneGanzeKarte();
void zeichnePixel();
void warteAufEinenKnopfdruck();
void gazeKarteNachOpenBewegen();
void spielZuEnde();
void kollisionPr_C3_BCfen();
void tempoSteigernPr_C3_BCfen();

// Beschreibe diese Funktion …
void zeichneNeueLinie() {
  xLuecke2 = (xLuecke + lueckenGroesse) - 1;
  oxocard.matrix->setActiveFrameBuffer(1, false);
  oxocard.matrix->setForeColor(rgb(255, 0, 0));
  if (xLuecke > 0) {
    oxocard.matrix->drawLine(0, yLuecke, xLuecke - 1, yLuecke);
  }
  if (xLuecke2 < 7) {
    oxocard.matrix->drawLine(xLuecke2 + 1, yLuecke, 7, yLuecke);
  }
  yLuecke ++;
  if (yLuecke > 7) {
    yLuecke = 7;
  }
  xLueckeAlt = xLuecke;
  xLuecke = Random::getInt(0, 7 - lueckenGroesse+1);
  xLuecke2 = (xLuecke + lueckenGroesse) - 1;
  while (!(xLuecke >= xLueckeAlt - 1 && xLuecke2 <= xLueckeAlt + lueckenGroesse)) {
    xLuecke = Random::getInt(1, 8 - lueckenGroesse+1);
    xLuecke2 = (xLuecke + lueckenGroesse) - 1;
  }
}

// Beschreibe diese Funktion …
void zeichneGanzeKarte() {
  for (int count = 0; count < 8; count++) {
    zeichneNeueLinie();
  }
  oxocard.matrix->copyFrameBufferContent(0, 1);
}

// Beschreibe diese Funktion …
void zeichnePixel() {
  oxocard.matrix->setActiveFrameBuffer(0, false);
  oxocard.matrix->setForeColor(rgb(0, 153, 0));
  oxocard.matrix->drawPixel(x, y);
}

// Beschreibe diese Funktion …
void warteAufEinenKnopfdruck() {
  while (1) {
    if (oxocard.button->isPressed(L1) || oxocard.button->isPressed(L2)) {
      break;
    }
    if (oxocard.button->isPressed(L3) || oxocard.button->isPressed(R1)) {
      break;
    }
    if (oxocard.button->isPressed(R2) || oxocard.button->isPressed(R3)) {
      break;
    }
    delay(50);
  }
}

// Beschreibe diese Funktion …
void gazeKarteNachOpenBewegen() {
  oxocard.matrix->setActiveFrameBuffer(1, false);
  oxocard.matrix->clearScreen();
  oxocard.matrix->setForeColor(rgb(255, 0, 0));
  for (iy = 1; iy <= 7; iy++) {
    for (ix = 0; ix <= 7; ix++) {
      oxocard.matrix->setActiveFrameBuffer(0, false);
      R = oxocard.matrix->readPixel(ix, iy).r;
      G = oxocard.matrix->readPixel(ix, iy).g;
      B = oxocard.matrix->readPixel(ix, iy).b;
      if ((R || G) || B) {
        oxocard.matrix->setActiveFrameBuffer(1, false);
        oxocard.matrix->drawPixel(ix, iy - 1);
      }
    }
  }
}

// Beschreibe diese Funktion …
void spielZuEnde() {
  oxocard.matrix->setActiveFrameBuffer(0, false);
  oxocard.matrix->clearScreen();
  oxocard.matrix->setForeColor(rgb(255, 0, 0));
  oxocard.matrix->drawImage(
    0b10000001,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b10000001, 0, 0);
  oxocard.matrix->update();
  //oxocard.audio->playWavFile((uint8_t*)crash_wav, crash_wav_size, false);  //TODO: not working yet in Arduino
  delay(2000);
  oxocard.system->reboot();
}

// Beschreibe diese Funktion …
void kollisionPr_C3_BCfen() {
  oxocard.matrix->setActiveFrameBuffer(1, false);
  R = oxocard.matrix->readPixel(x, y).r;
  G = oxocard.matrix->readPixel(x, y).g;
  B = oxocard.matrix->readPixel(x, y).b;
  if ((R || G) || B) {
    spielZuEnde();
  }
}

// Beschreibe diese Funktion …
void tempoSteigernPr_C3_BCfen() {
  tempoSteigernZaehler ++;
  if (tempoSteigernZaehler >= tempoSteigernNach) {
    tempoSteigernZaehler = 0;
    tempo = tempo - tempoSteigernUm;
  }
}


void setup() {
  x = 3;
  y = 0;
  xLuecke = 2;
  yLuecke = 0;
  lueckenGroesse = 3;
  tempo = 500;
  tempoSteigernNach = 10;
  tempoSteigernUm = 50;
  tempoSteigernZaehler = 0;
  oxocard.accelerometer->setRootCursor(x,y);
  oxocard.matrix->disableAutoUpdate();
  zeichneGanzeKarte();
  zeichnePixel();
  oxocard.matrix->update();
  warteAufEinenKnopfdruck();
}

void loop() {
  x = oxocard.accelerometer->getCursorX();
  gazeKarteNachOpenBewegen();
  zeichneNeueLinie();
  oxocard.matrix->copyFrameBufferContent(0, 1);
  zeichnePixel();
  oxocard.matrix->update();
  kollisionPr_C3_BCfen();
  tempoSteigernPr_C3_BCfen();
  delay(tempo);
}

