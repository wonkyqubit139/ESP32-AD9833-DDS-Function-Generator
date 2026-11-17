#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SW0     16   
#define SW1     17  
#define SEL_BTN 18   
#define POT_PIN 34   


volatile bool selectFlag = false;
volatile unsigned long lastPress = 0;

int cursor = 0;                     
bool ampMode = false;               
char freqStr[7] = {'1','0','0','0','0','0','0'};
int amplitude = 5;                  
String waveType = "SINE";


bool blinkState = true;
unsigned long lastBlink = 0;


void IRAM_ATTR onSelect() {
  unsigned long now = millis();
  if (now - lastPress > 200) {   
    selectFlag = true;
    lastPress = now;
  }
}


void setup() {
  pinMode(SW0, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SEL_BTN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SEL_BTN), onSelect, FALLING);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Function Gen");
  delay(1000);
  lcd.clear();
}


void loop() {
  updateWaveform();
  handleSelection();
  handlePot();
  handleBlink();
  drawLCD();
}



void updateWaveform() {
  int b0 = !digitalRead(SW0);
  int b1 = !digitalRead(SW1);
  int sel = (b1 << 1) | b0;

  switch (sel) {
    case 0: waveType = "SINE";      break;
    case 1: waveType = "SQUARE";    break;
    case 2: waveType = "TRIANGLE";  break;
    case 3: waveType = "PULSE";     break;
  }
}

void handleSelection() {
  if (selectFlag) {
    selectFlag = false;

    if (!ampMode) {
      cursor++;
      // if (cursor == 4) cursor++;     

      if (cursor >= 7) {
        cursor = 0;
        ampMode = true;             
      }
    } else {
      ampMode = false;            
      cursor = 0;
    }
  }
}

void handlePot() {
  static int lastValue = -1;
  int pot = analogRead(POT_PIN);

  if (!ampMode) {
    int digit = map(pot, 0, 4000, 0, 9);
    digit = constrain(digit, 0, 9);
    if (digit != lastValue ) {
      freqStr[cursor] = digit + '0';
      lastValue = digit;
    }
  } else {
    int amp = map(pot, 0, 4095, 0, 9);
    if (amp != amplitude) {
      amplitude = amp;
    }
  }
}

void handleBlink() {
  if (millis() - lastBlink > 400) {
    blinkState = !blinkState;
    lastBlink = millis();
  }
}

void drawLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Wave:");
  lcd.print(waveType);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("F:");

  for (int i = 0; i < 7; i++) {
    
    if (!ampMode && i == cursor && blinkState) {
      lcd.print("_");
    } else {
      int freq = 0;
      lcd.print(freqStr[i]);
    }
  }

  lcd.print(" A:");

  if (ampMode && blinkState) {
    lcd.print("_");
  } else {
    lcd.print(amplitude);
  }
}