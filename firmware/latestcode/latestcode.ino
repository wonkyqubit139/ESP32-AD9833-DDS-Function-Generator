#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <AD9833.h>

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- YOUR EXISTING PINS (UNCHANGED) ----------------
#define WAVE_BTN 16
#define SEL_BTN 18
#define POT_PIN 34

// ---------------- AD9833 PINS (NEW, SAFE) ----------------
#define FSYNC 15   // Chip select for AD9833

AD9833 signalGenerator(FSYNC);

// ---------------- State ----------------
volatile bool selectFlag = false;
volatile unsigned long lastPress = 0;

volatile bool waveFlag = false;
volatile unsigned long lastWavePress = 0;

int cursor = 0;
bool ampMode = false;

// ---- NEW: 7 DIGITS, NO DECIMAL ----
char freqStr[7] = {'0','1','0','0','0','0','0'};  // 10000 Hz default
int amplitude = 5;

int waveState = 0;
String waveType = "SINE";

bool blinkState = true;
unsigned long lastBlink = 0;

const int pinV1 = 32;
const int pinV2 = 33;

// ============================================================
// ================ VPP & OFFSET VARIABLES (NEW) ==============
// ============================================================

const float SCALE_FACTOR = 20.0;
float measuredVpp = 0.0;
float measuredOffset = 0.0;
unsigned long lastVppRead = 0;
const int VPP_READ_INTERVAL = 100;

// ============================================================

// ---- MASTER FREQUENCY (SYNC SOURCE) ----
long masterFrequency = 10000;   // integer Hz

float readVoltage(int pin) {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  return (sum / 10.0) * 3.3 / 4095.0;
}

// ============================================================
// ================ VPP & OFFSET FUNCTION (NEW) ===============
// ============================================================

void readVppAndOffset() {
  if (millis() - lastVppRead < VPP_READ_INTERVAL) {
    return;
  }
  lastVppRead = millis();
  
  float V1 = readVoltage(pinV1);
  float V2 = readVoltage(pinV2);
  
  measuredVpp = (V1 + V2) * SCALE_FACTOR;
  measuredOffset = ((V1 - V2) / 2.0) * SCALE_FACTOR;
  
  Serial.print("V1:"); Serial.print(V1, 2);
  Serial.print(" V2:"); Serial.print(V2, 2);
  Serial.print(" Vpp:"); Serial.print(measuredVpp, 1);
  Serial.print(" Off:"); Serial.println(measuredOffset, 1);
}

// ============================================================

// ---------------- INTERRUPTS ----------------
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

// ---------------- SETUP ----------------
void setup() {
  pinMode(WAVE_BTN, INPUT_PULLUP);
  pinMode(SEL_BTN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SEL_BTN), onSelect, FALLING);
  attachInterrupt(digitalPinToInterrupt(WAVE_BTN), onWave, FALLING);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  SPI.begin(14, -1, 13, 15); // SCK=14, MOSI=13, CS=15
  signalGenerator.begin();

  signalGenerator.setWave(AD9833_SINE);
  signalGenerator.setFrequency(masterFrequency);

  Serial.begin(115200);

  lcd.setCursor(0, 0);
  lcd.print("Function Gen");
  delay(1000);
  lcd.clear();
  
  // ============ VPP SETUP (NEW) ============
  pinMode(pinV1, INPUT);
  pinMode(pinV2, INPUT);
  // =========================================
}

// ---------------- LOOP ----------------
void loop() {
  handleWaveButton();
  handleSelection();
  handlePot();
  handleBlink();
  updateMasterFrequency();
  
  // ============ VPP READING (NEW) ============
  readVppAndOffset();
  // ===========================================
  
  drawLCD();
  sendToAD9833();
}

// ---------------- FUNCTIONS ----------------

// ---- Wave button cycles 4 states ----
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

// ---- Cursor movement ----
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

// ---- Pot edits digits or amplitude ----
void handlePot() {
  static int lastDigit = -1;
  static int lastPot = 0;

  int pot = analogRead(POT_PIN);

  // ---- SIMPLE NOISE FILTER (CRITICAL FIX) ----
  if (abs(pot - lastPot) < 30) {   // <<<<< KEY LINE
    return; // Ignore small fluctuations
  }
  lastPot = pot;
  // -------------------------------------------

  if (!ampMode) {
    int digit = map(pot, 0, 4000, 0, 9);

    if (digit != lastDigit) {
      freqStr[cursor] = digit + '0';
      lastDigit = digit;
    }
  } 
  else {
    amplitude = map(pot, 0, 4000, 0, 9);
  }
}


// ---- Blinking cursor ----
void handleBlink() {
  if (millis() - lastBlink > 400) {
    blinkState = !blinkState;
    lastBlink = millis();
  }
}

// ---- Convert string to integer Hz ----
void updateMasterFrequency() {
  masterFrequency = atol(freqStr);

  if (masterFrequency < 1) masterFrequency = 1;
  if (masterFrequency > 1000000) masterFrequency = 1000000;
}

// ---- LCD display (MODIFIED) ----
void drawLCD() {
  // ========== ROW 0: Wave type + Offset ==========
  lcd.setCursor(0, 0);
  lcd.print("W:");
  lcd.print(waveType);
  lcd.print("        ");  // Clear remaining chars
  
  // Offset at fixed position (column 11)
  lcd.setCursor(11, 0);
  lcd.print("O:");
  if (measuredOffset < 10.0 && measuredOffset >= 0) {
    lcd.print(measuredOffset, 1);
  } else {
    lcd.print((int)measuredOffset);
    lcd.print(" ");
  }

  // ========== ROW 1: Frequency + Vpp ==========
  lcd.setCursor(0, 1);
  lcd.print("F:");

  for (int i = 0; i < 7; i++) {
    if (!ampMode && i == cursor && blinkState) {
      lcd.print("_");
    } else {
      lcd.print(freqStr[i]);
    }
  }

  // Vpp at fixed position (column 10)
  lcd.setCursor(10, 1);
  lcd.print("Vpp:");
  if (measuredVpp < 10.0) {
    lcd.print(measuredVpp, 1);
  } else {
    lcd.print((int)measuredVpp);
  }
}

// ---- Send to AD9833 ----
void sendToAD9833() {

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
