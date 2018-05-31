#include "OXOcardRunnerV2.h"
#include <esp_log.h>

#define START_NUMBER_OF_ROUNDS 3
#define MAX_NUMBER_OF_ROUNDS   30
#define COLOR_PLAY             rgb(0, 0, 255)
#define COLOR_OFF              rgb(255, 0, 0)
#define COLOR_MID              rgb(102,0,204)
#define COLOR_NUMBER           rgb(246, 168, 0)
#define TONE_DURATON           200

void showWelcome();
void initSet();
uint8_t getTiltPosition();
void drawUp();
void drawDown();
void drawRight();
void drawLeft();
void playTone(uint8_t toneId);

uint8_t tones[MAX_NUMBER_OF_ROUNDS] = {0};
uint8_t step = 0;
uint8_t maxstep = START_NUMBER_OF_ROUNDS-1;
uint8_t state = 0;
uint8_t lastTiltPosition = 0;

void setup() {
  showWelcome();
  oxocard.matrix->disableAutoUpdate();
  //oxocard.audio->playWavFile((uint8_t*)flute_wav, flute_wav_size, true);  //TODO: not working yet in Arduino
}

void loop() {
  switch (state)
  {
    /* Pause mode */
    case 0:
    {
      static uint32_t tRef = millis();
      static uint8_t introstate = 0;
      switch (introstate)
      {
        case 0:
        {
          oxocard.matrix->drawText("PLAY", false, false, 220, COLOR_PLAY);
          delay(200);
          introstate++;
          tRef = millis();
        } break;
        case 1:
        {
          oxocard.matrix->setPixel(0, 6, COLOR_PLAY);
          oxocard.matrix->setPixel(0, 7, COLOR_PLAY);
          oxocard.matrix->setPixel(1, 7, COLOR_PLAY);
          oxocard.matrix->update();
          if ((millis() - tRef) >= 600) introstate++;
        } break;
        case 2:
        {
          oxocard.matrix->drawText("OFF", false, false, 220, COLOR_OFF);
          delay(200);
          introstate++;
          tRef = millis();
        } break;
        case 3:
        {
          oxocard.matrix->setPixel(7, 6, COLOR_OFF);
          oxocard.matrix->setPixel(7, 7, COLOR_OFF);
          oxocard.matrix->setPixel(6, 7, COLOR_OFF);
          oxocard.matrix->update();
          if ((millis() - tRef) >= 600) introstate++;
        } break;
      }

      if (introstate > 3) introstate = 0;

      if (oxocard.button->isPressed(L1) || oxocard.button->isPressed(L2) ||
        oxocard.button->isPressed(L3))
      {
        initSet();
        oxocard.matrix->clear();
        oxocard.matrix->update();
        delay(50);
        oxocard.audio->noTone();
        state = 1;
      }

      if (oxocard.button->isPressed(R1) || oxocard.button->isPressed(R2) ||
        oxocard.button->isPressed(R3))
      {
        oxocard.system->turnOff();
      }
    } break;

    /* Show mode */
    case 1:
    {
      if (maxstep < MAX_NUMBER_OF_ROUNDS) maxstep++;
      tones[maxstep-1] = random(1, 5);
      delay(1000);
      for (int i = 0; i < maxstep; i++)
      {
        oxocard.matrix->clear();
        oxocard.matrix->update();
        playTone(tones[i]);
        delay(500);
      }
      oxocard.matrix->clear();
      oxocard.matrix->update();
      while(getTiltPosition() != 0);// delay(5);
      state = 2;
    } break;

    /* Listen mode */
    case 2:
    {
      uint8_t tiltPosition = getTiltPosition();
      if (tiltPosition != lastTiltPosition)
      {
        if (tiltPosition != 0)
        {
          if (tones[step] != tiltPosition)
          {
            step = 0;
            oxocard.matrix->clear();
            oxocard.matrix->drawImage(
              0b00000000,
              0b01000010,
              0b00100100,
              0b00011000,
              0b00011000,
              0b00100100,
              0b01000010,
              0b00000000, 0, 0, rgb(255, 0, 0));
            oxocard.matrix->update();
            oxocard.audio->tone(100, 1500);
            delay(1500);
            oxocard.matrix->drawNumber(maxstep-START_NUMBER_OF_ROUNDS, COLOR_NUMBER);
            oxocard.matrix->update();
            delay(2500);
            state = 0;
            oxocard.matrix->clear();
            //oxocard.audio->playWavFile((uint8_t*)flute_wav, flute_wav_size, true);  //TODO: not working yet in Arduino
            break;
          }
          step++;
          playTone(tiltPosition);
        }
        else
        {
          oxocard.matrix->clear();
          oxocard.matrix->update();
          if (step == maxstep)
          {
            step = 0;
            state = 1;
          }
        }
        delay(TONE_DURATON);
        lastTiltPosition = tiltPosition;
      }
    } break;
  }
}

void showWelcome()
{
  uint8_t imageSmiley[8] = {
    0b00000000,
    0b00100100,
    0b00100100,
    0b00000000,
    0b00000000,
    0b01000010,
    0b00111100,
    0b00000000
  };
  oxocard.matrix->drawImage(imageSmiley, 0, 0);
  delay(500);
  oxocard.matrix->clear();
  oxocard.matrix->drawImage(
    0b00000000,
    0b00100000,
    0b00100100,
    0b00000000,
    0b00000000,
    0b01000010,
    0b00111100,
    0b00000000, 0, 0);
  //oxocard.audio->playWavFile((uint8_t*)knife_throw_wav, knife_throw_wav_size, false);  //TODO: not working yet in Arduino
  delay(300);
  oxocard.audio->noTone();
  oxocard.matrix->drawImage(imageSmiley, 0, 0);
  delay(700);
}

void initSet()
{
  step = 0;
  maxstep = 2;
  for (uint8_t i = 0; i < sizeof(tones); i++)
  {
    tones[i] = random(1, 5);
  }
}

void drawUp()
{
  for (int i = 3; i >= 0; i--) {
    oxocard.matrix->drawLine(0, i, 7, i, rgb((i+1)*50, (i+1)*50, 0));
    oxocard.matrix->update();
    delay(13);
  }
}

void drawDown()
{
  for (int i = 4; i <= 7; i++) {
    oxocard.matrix->drawLine(0, i, 7, i, rgb(0, 255-((i-4)*50), 0));
    oxocard.matrix->update();
    delay(13);
  }
}

void drawRight()
{
  for (int i = 4; i <= 7; i++) {
    oxocard.matrix->drawLine(i, 0, i, 7, rgb(255-((i-4)*50), 0, 0));
    oxocard.matrix->update();
    delay(13);
  }
}

void drawLeft()
{
  for (int i = 3; i >= 0; i--) {
    oxocard.matrix->drawLine(i, 0, i, 7, rgb(0,0,(i+1)*50));
    oxocard.matrix->update();
    delay(13);
  }
}

uint8_t getTiltPosition() {
  int8_t o = oxocard.accelerometer->getOrientation();
  switch (o)
  {
    case LIS3DE::HALF_UP:     o = 1; break; // up
    case LIS3DE::HALF_DOWN:   o = 2; break; // down
    case LIS3DE::HALF_RIGHT:  o = 3; break; // right
    case LIS3DE::HALF_LEFT:   o = 4; break; // left
    default: o = 0;
  }
  return o;
}

void playTone(uint8_t toneId) {
  // oxocard.matrix->drawRectangle(3, 3, 2, 2, COLOR_MID);
  if (toneId == 1) // up
  {
    drawUp();
    oxocard.audio->setGlobalOctave(Octave::C5);
    oxocard.audio->tone(Note::D, TONE_DURATON);
  }
  if (toneId == 2) // down
  {
    drawDown();
    oxocard.audio->setGlobalOctave(Octave::C5);
    oxocard.audio->tone(Note::C, TONE_DURATON);
  }
  if (toneId == 3) // right
  {
    drawRight();
    oxocard.audio->setGlobalOctave(Octave::C4);
    oxocard.audio->tone(Note::B, TONE_DURATON);
  }
  if (toneId == 4) // left
  {
    drawLeft();
    oxocard.audio->setGlobalOctave(Octave::C4);
    oxocard.audio->tone(Note::A, TONE_DURATON);
  }
}

