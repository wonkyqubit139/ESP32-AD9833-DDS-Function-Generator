#include "globals.h"

void handleWaveButton() {

if (waveFlag) {

waveFlag = false;

waveState = (waveState + 1) % 4;

switch (waveState) {

case 0: waveType = "SINE"; break;
case 1: waveType = "SQUARE"; break;
case 2: waveType = "TRIANGLE"; break;
case 3: waveType = "PULSE"; break;

}

}

}

void handleSelection() {

if (selectFlag) {

selectFlag = false;

if (!ampMode) {

cursor++;

if (cursor >= 7) {

cursor = 0;
ampMode = true;

}

}

else {

ampMode = false;
cursor = 0;

}

}

}

void handlePot() {

static int lastDigit = -1;
static int lastPot = 0;

int pot = analogRead(POT_PIN);

if (abs(pot - lastPot) < 30) {

return;

}

lastPot = pot;

if (!ampMode) {

int digit = map(pot,0,4000,0,9);

if (digit != lastDigit) {

freqStr[cursor] = digit + '0';
lastDigit = digit;

}

}

else {

amplitude = map(pot,0,4000,0,9);

}

}
