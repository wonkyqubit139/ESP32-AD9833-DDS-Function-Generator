#include "globals.h"
#include "interrupts.h"
#include "inputHandler.h"
#include "display.h"
#include "frequency.h"
#include "ad9833control.h"

void setup() {

pinMode(WAVE_BTN, INPUT_PULLUP);
pinMode(SEL_BTN, INPUT_PULLUP);

attachInterrupt(digitalPinToInterrupt(SEL_BTN), onSelect, FALLING);
attachInterrupt(digitalPinToInterrupt(WAVE_BTN), onWave, FALLING);

Wire.begin();

lcd.init();
lcd.backlight();

SPI.begin(14,-1,13,15);

signalGenerator.begin();
signalGenerator.setWave(AD9833_SINE);
signalGenerator.setFrequency(masterFrequency);

Serial.begin(115200);

lcd.setCursor(0,0);
lcd.print("Function Gen");

delay(1000);

lcd.clear();

}

void loop(){

handleWaveButton();
handleSelection();
handlePot();
handleBlink();
updateMasterFrequency();
drawLCD();
sendToAD9833();

}
