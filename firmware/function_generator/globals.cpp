#include "globals.h"

LiquidCrystal_I2C lcd(0x27,16,2);

AD9833 signalGenerator(FSYNC);

volatile bool selectFlag = false;
volatile unsigned long lastPress = 0;

volatile bool waveFlag = false;
volatile unsigned long lastWavePress = 0;

int cursor = 0;
bool ampMode = false;

char freqStr[7] = {'0','1','0','0','0','0','0'};

int amplitude = 5;
int waveState = 0;
String waveType = "SINE";

bool blinkState = true;
unsigned long lastBlink = 0;

long masterFrequency = 10000;
