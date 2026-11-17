#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <AD9833.h>
// LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define WAVE_BTN 16
#define SEL_BTN 18
#define POT_PIN 34
// AD9833 PINS 
#define FSYNC 15 // Chip select for AD9833
AD9833 signalGenerator(FSYNC);
// State 
volatile bool selectFlag = false;
volatile unsigned long lastPress = 0;
volatile bool waveFlag = false;
volatile unsigned long lastWavePress = 0;
int cursor = 0;
bool ampMode = false;
-
char freqStr[7] = {'0','1','0','0','0','0','0'}; // 10000 Hz default
int amplitude = 5;
int waveState = 0;
String waveType = "SINE";
bool blinkState = true;
unsigned long lastBlink = 0;

long masterFrequency = 10000; // integer Hz

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

void setup() {
pinMode(WAVE_BTN, INPUT_PULLUP);
pinMode(SEL_BTN, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(SEL_BTN), onSelect, FALLING);
attachInterrupt(digitalPinToInterrupt(WAVE_BTN), onWave, FALLING);
Wire.begin();
lcd.init();
lcd.backlight();
SPI.begin(14, -1, 13, 15); 
signalGenerator.begin();
signalGenerator.setWave(AD9833_SINE);
signalGenerator.setFrequency(masterFrequency);
Serial.begin(115200);
lcd.setCursor(0, 0);
lcd.print("Function Gen");
delay(1000);
lcd.clear();
}

void loop() {
handleWaveButton();
handleSelection();
handlePot();
handleBlink();
updateMasterFrequency();
drawLCD();
sendToAD9833();
}
// wave button cycle
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
//Cursor movement
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
// Pot edits digits or amplitude
void handlePot() {
static int lastDigit = -1;
static int lastPot = 0;
int pot = analogRead(POT_PIN);
// SIMPLE NOISE FILTER 
if (abs(pot - lastPot) < 30) { // <<<<< KEY LINE
return; // Ignore small fluctuations
}
lastPot = pot;

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
// Blinking cursor 
void handleBlink() {
if (millis() - lastBlink > 400) {
blinkState = !blinkState;
lastBlink = millis();
}
}
//  Convert string to int Hz
void updateMasterFrequency() {
masterFrequency = atol(freqStr);
if (masterFrequency < 1) masterFrequency = 1;
if (masterFrequency > 1000000) masterFrequency = 1000000;
}
// LCD display
void drawLCD() {
lcd.setCursor(0, 0);
lcd.print("Wave:");
lcd.print(waveType);
lcd.print(" ");
lcd.setCursor(0, 1);
lcd.print("F:");
for (int i = 0; i < 7; i++) {
if (!ampMode && i == cursor && blinkState) {
lcd.print("_");
} else {
lcd.print(freqStr[i]);
}
}
lcd.print(" Hz");
lcd.print(" A:");
lcd.print(amplitude);
}
//  Send to AD9833
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