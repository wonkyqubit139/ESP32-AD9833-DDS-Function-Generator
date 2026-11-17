#include "globals.h"

void sendToAD9833(){

if (waveType == "SINE") {

signalGenerator.setWave(AD9833_SINE);

}

else if (waveType == "SQUARE") {

signalGenerator.setWave(AD9833_SQUARE1);

}

else if (waveType == "TRIANGLE") {

signalGenerator.setWave(AD9833_TRIANGLE);

}

else if (waveType == "PULSE") {

signalGenerator.setWave(AD9833_SQUARE1);

}

signalGenerator.setFrequency(masterFrequency);

Serial.print("Freq sent to AD9833 = ");
Serial.print(masterFrequency);
Serial.println(" Hz");

}
