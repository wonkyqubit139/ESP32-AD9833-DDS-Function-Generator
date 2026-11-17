#ifndef GLOBALS_H
#define GLOBALS_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <AD9833.h>

// ---------------- LCD ----------------
extern LiquidCrystal_I2C lcd;

// ---------------- YOUR EXISTING PINS ----------------
#define WAVE_BTN 16
#define SEL_BTN 18
#define POT_PIN 34

// ---------------- AD9833 PINS ----------------
#define FSYNC 15
extern AD9833 signalGenerator;

// ---------------- State ----------------
extern volatile bool selectFlag;
extern volatile unsigned long lastPress;

extern volatile bool waveFlag;
extern volatile unsigned long lastWavePress;

extern int cursor;
extern bool ampMode;

extern char freqStr[7];

extern int amplitude;
extern int waveState;
extern String waveType;

extern bool blinkState;
extern unsigned long lastBlink;

extern long masterFrequency;

#endif
