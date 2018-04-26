#include "OXOcardRunnerV2.h"

float Temperatur;
float Niederschlag;
float Luftfeuchtigkeit;
float Wind;
float Taupunkt;

void setup() {

}

void loop() {
  oxocard.wifi->autoConnect();
  oxocard.weather->downloadWeatherForTown("66310");
  oxocard.matrix->clear();
  oxocard.matrix->drawImage(
    oxocard.weather->getDrawableIcon(), 0, 0,
    oxocard.weather->getDrawableIconColor()
  );
  if (oxocard.button->isPressed(L1)) {
    oxocard.matrix->clearScreen();
    Temperatur = oxocard.weather->getTemparature();
    oxocard.matrix->setForeColor(rgb(255, 0, 0));
    oxocard.matrix->drawNumber(Temperatur);
    delay(2000);
    oxocard.matrix->clearScreen();
  }
  if (oxocard.button->isPressed(L2)) {
    oxocard.matrix->clearScreen();
    Niederschlag = oxocard.weather->getPrecipitation();
    oxocard.matrix->setForeColor(rgb(51, 51, 255));
    oxocard.matrix->drawNumber(Niederschlag);
    delay(2000);
    oxocard.matrix->clearScreen();
  }
  if (oxocard.button->isPressed(L3)) {
    oxocard.matrix->clearScreen();
    Luftfeuchtigkeit = oxocard.weather->getHumidity();
    oxocard.matrix->setForeColor(rgb(51, 204, 255));
    oxocard.matrix->drawNumber(Luftfeuchtigkeit);
    delay(2000);
    oxocard.matrix->clearScreen();
  }
  if (oxocard.button->isPressed(R1)) {
    oxocard.matrix->clearScreen();
    Wind = oxocard.weather->getWind();
    oxocard.matrix->setForeColor(rgb(192, 192, 192));
    oxocard.matrix->drawNumber(Wind);
    delay(2000);
    oxocard.matrix->clearScreen();
  }
  if (oxocard.button->isPressed(R2)) {
    oxocard.matrix->clearScreen();
    Taupunkt = oxocard.weather->getDewPoint();
    oxocard.matrix->setForeColor(rgb(255, 102, 0));
    oxocard.matrix->drawNumber(Taupunkt);
    delay(2000);
    oxocard.matrix->clearScreen();
  }
}

