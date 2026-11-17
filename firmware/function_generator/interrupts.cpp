#include "globals.h"

void IRAM_ATTR onSelect() {

unsigned long now = millis();

if (now - lastPress > 200) {

selectFlag = true;
lastPress = now;

}

}

void IRAM_ATTR onWave() {

unsigned long now = millis();

if (now - lastWavePress > 200) {

waveFlag = true;
lastWavePress = now;

}

}
