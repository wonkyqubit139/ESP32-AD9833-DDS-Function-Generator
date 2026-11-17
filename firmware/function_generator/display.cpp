#include "globals.h"

void handleBlink() {

if (millis() - lastBlink > 400) {

blinkState = !blinkState;
lastBlink = millis();

}

}

void drawLCD() {

lcd.setCursor(0,0);

lcd.print("Wave:");
lcd.print(waveType);
lcd.print(" ");

lcd.setCursor(0,1);

lcd.print("F:");

for(int i=0;i<7;i++){

if(!ampMode && i==cursor && blinkState){

lcd.print("_");

}

else{

lcd.print(freqStr[i]);

}

}

lcd.print(" Hz");

lcd.print(" A:");
lcd.print(amplitude);

}
