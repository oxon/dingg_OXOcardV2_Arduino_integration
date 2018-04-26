#include "OXOcardRunnerV2.h"

void setup() {

}

void loop() {
  while (oxocard.button->isPressed(L1)) {
    oxocard.audio->tone(Note::C);
    oxocard.matrix->fillScreen();
  }
  while (oxocard.button->isPressed(L2)) {
    oxocard.audio->tone(Note::D);
    oxocard.matrix->fillScreen();
  }
  while (oxocard.button->isPressed(L3)) {
    oxocard.audio->tone(Note::E);
    oxocard.matrix->fillScreen();
  }
  while (oxocard.button->isPressed(R1)) {
    oxocard.audio->tone(Note::F);
    oxocard.matrix->fillScreen();
  }
  while (oxocard.button->isPressed(R2)) {
    oxocard.audio->tone(Note::G);
    oxocard.matrix->fillScreen();
  }
  while (oxocard.button->isPressed(R3)) {
    oxocard.audio->tone(Note::A);
    oxocard.matrix->fillScreen();
  }
  oxocard.audio->noTone();
  oxocard.matrix->clearScreen();
}

