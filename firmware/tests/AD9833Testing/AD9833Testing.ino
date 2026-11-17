#include <SPI.h>
#include <AD9833.h>

#define FSYNC 5

AD9833 gen(FSYNC);

float frequency = 1000;   // default 1 kHz
int currentWave = 0;      // 0 = none, 1 = sine, 2 = square, 3 = triangle
bool waveInitialized = false;  // state flag

void setup() {
  Serial.begin(115200);
  SPI.begin();

  gen.begin();

  Serial.println("Wave Generator Ready");
  Serial.println("Type:");
  Serial.println("1 → Sine");
  Serial.println("2 → Square");
  Serial.println("3 → Triangle");
}

void loop() {

  // Run only once to set default wave
  if (!waveInitialized) {
    gen.setFrequency(frequency);
    gen.setWave(AD9833_SINE);
    currentWave = 1;
    waveInitialized = true;
    Serial.println("Default Sine wave started");
  }

  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == '1' && currentWave != 1) {
      gen.setWave(AD9833_SINE);
      currentWave = 1;
      Serial.println("Sine wave selected");
    }
    else if (cmd == '2' && currentWave != 2) {
      gen.setWave(AD9833_SQUARE1);
      currentWave = 2;
      Serial.println("Square wave selected");
    }
    else if (cmd == '3' && currentWave != 3) {
      gen.setWave(AD9833_TRIANGLE);
      currentWave = 3;
      Serial.println("Triangle wave selected");
    }
  }
}