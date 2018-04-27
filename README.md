# [![OXOcard][OXOcard_logo]][OXOcard]

## Intro
Du möchtest einmal hinter die Kulissen deines Computers oder Handys schauen und verstehen, wie alles funktioniert? Die Oxocard ist Teil eines Programmierkurses für Jugendliche und Junggebliebene, welche gerne mit unserem [Blockly][Blockly] oder mit [Arduino][Arduino] programmieren lernen möchten.

Das hier ersichtliche Repository ist als Zwischenglied für die Integration der Oxocard in die Arduino-ESP32-Umgebung gedacht.

## Installation
1. Folge der Installations-Anleitung für den Arduino core für ESP32 auf: https://github.com/espressif/arduino-esp32
2. Lade dieses Repository als .zip-Datei herunter und entpacke es. Dann verschiebe den "oxocard_V1.0.0"-Ordner nach:
[ARDUINO_SKETCHBOOK_DIR]/hardware/espressif/
3. Starte deine Arduino IDE neu
4. Wähle die Oxocard unter: Tools->Board->Oxocard
5. Wähle den korrekten Port unter: Tools->Port
6. Wähle ein Beispiel unter: File->Examples->dingg_lib_OXOcardV2

Weitere Informationen zur Oxocard findest du auf unserer [Homepage][OXOcard].

## Loslegen
Um die Oxocard nach der Installation ansteuern zu können, muss lediglich deren Runner in dein Projekt integriert werden:
```Arduino
#include "OXOcardRunnerV2.h"
```
Dieser Runner erzeugt ein oxocard-Objekt und verlinkt die einzelnen Libraries der Hardware-Module der Oxocard.
Nachfolgend einige Beispiele für die Ansteuerung einiger solcher Hardware-Module der Oxocard:
```Arduino
oxocard.accelerometer->getCursorX();    // 0... 7
oxocard.audio->tone(440);               // 440Hz
oxocard.audio->noTone();
oxocard.button->isPressed(L1);          // L1, L2, L3, R1, R2, R3
oxocard.matrix->drawPixel(x, y);
oxocard.system->turnOff();
```
Alle verfügbaren Funktionen sind im [Blockly][Blockly] oder direkt in den entsprechenden [Header-Files der Library][Header-Files] ersichtlich.

## Tipps
Ein [Blockly][Blockly]-Programm kann relativ einfach ins Arduino integriert werden. Es muss lediglich der Runner und die user_main()-Funktion angepasst werden:
```Arduino
#include "OXOcardRunner.h"    ->    #include "OXOcardRunnerV2.h"
void user_main() {            ->    void setup() {
    // setup...                     }
    while(1) {
        // loop...                  void loop() {
    }                               }
}
```

## Bekannte Probleme
Die im [Blockly][Blockly] verfügbaren Sound-Effekte konnten bisher noch nicht erfolgreich in die Arduino-ESP32-Umgebung integriert werden. Der folgende Funktionsaufruf erzeugt also noch einen Kompilier-Fehler und kann daher nicht verwendet werden.
```Arduino
oxocard.audio->playWavFile((uint8_t*)alarm_wav, alarm_wav_size, false);
```

<!-- [![Hello][OXOcard_gif]][OXOcard] -->

## License
This project is licensed under the terms of the [GNU Lesser General Public License][License].



[OXOcard_logo]:http://www.oxocard.ch/wp-content/uploads/2017/04/logo_oxocard_inverted.png
[OXOcard]:http://www.oxocard.ch/
[Blockly]:https://blockly.oxocard.ch/
[Arduino]:http://www.arduino.cc/
[Header-Files]:https://github.com/oxon/dingg_OXOcardV2_Arduino_integration/tree/master/oxocard_V1.0.0/libraries/dingg_lib_OXOcardV2
[OXOcard_gif]:http://www.oxocard.ch/wp-content/uploads/2017/04/oxocard_animiert.gif
[License]:https://github.com/oxon/dingg_OXOcardV2_Arduino_integration/blob/master/LICENSE
