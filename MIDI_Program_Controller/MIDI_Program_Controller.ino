/*
 * This file is part of the ARMIDUINO project.
 *
 * ARMIDUINO is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ARMIDUINO is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with ARMIDUINO. If not, see <https://www.gnu.org/licenses/>. 
 */

#include <LiquidCrystal_I2C.h>
#include <MIDI.h>
#include <Wire.h>

#define MSB_CC 0 // Standard MSB bank MIDI CC number
#define MAX_MSB_VAL 127 // Your max MSB bank value

#define LSB_CC 32 // Standard LSB bank MIDI CC number
#define MAX_LSB_VAL 127 // Your max LSB bank value

#define encCLK0 39
#define encDT0 43
#define encSW0 47

#define encCLK1 2
#define encDT1 3
#define encSW1 4

#define encCLK2 8
#define encDT2 9
#define encSW2 10

#define encCLK3 11
#define encDT3 12
#define encSW3 13

// Set the LCD address to 0x27 for a 20 chars and 4 line display, your LCD address may vary
LiquidCrystal_I2C lcd(0x27, 20, 4);


// Initializing variables for tracking Encoders
int prevCLK0;
int currCLK0;
int stateSW0 = HIGH;
unsigned long lastSW0 = 0;

int prevCLK1;
int currCLK1;
int stateSW1 = HIGH;
unsigned long lastSW1 = 0;

int prevCLK2;
int currCLK2;
int stateSW2 = HIGH;
unsigned long lastSW2 = 0;

int prevCLK3;
int currCLK3;
int stateSW3 = HIGH;
unsigned long lastSW3 = 0;

byte MSBVal = 0;
byte LSBVal = 0;
byte PCNum = 1;
byte chanNum = 1;

// Initialize MIDI default configuration
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  // Start MIDI, input channel does not matter so listening on all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // initialize the LCD
  lcd.begin();
  lcd.print("MSB  LSB  PC#  CH#");
  lcd.setCursor(0, 1);
  lcd.print("0    0    1    1");

  // configure the pins for each encoder
  pinMode(encCLK0, INPUT);
  pinMode(encDT0, INPUT);
  pinMode(encSW0, INPUT_PULLUP);
  prevCLK0 = digitalRead(encCLK0);

  pinMode(encCLK1, INPUT);
  pinMode(encDT1, INPUT);
  pinMode(encSW1, INPUT_PULLUP);
  prevCLK1 = digitalRead(encCLK1);

  pinMode(encCLK2, INPUT);
  pinMode(encDT2, INPUT);
  pinMode(encSW2, INPUT_PULLUP);
  prevCLK2 = digitalRead(encCLK2);

  pinMode(encCLK3, INPUT);
  pinMode(encDT3, INPUT);
  pinMode(encSW3, INPUT_PULLUP);
  prevCLK3 = digitalRead(encCLK3);
}

/*
 * Function: MSBEncoder
 * Description: Encoder control for the MSB bank value
 */
void MSBEncoder() {
  currCLK0 = digitalRead(encCLK0);
  if (currCLK0 != prevCLK0) {
    // clockwise rotation of encoder increases the value
    if (digitalRead(encDT0) != currCLK0) {
      // If you try to exceed the max value, it will return to 0
      if (MSBVal == MAX_MSB_VAL) {
        MSBVal = 0;
      } else {
        MSBVal++;
      }
    // counter-clockwise rotation of encoder increases the value
    } else {
      if (MSBVal== 0) {
        MSBVal = MAX_MSB_VAL;
      } else {
        MSBVal--;
      }
    }
    // clear the existing on-screen value
    lcd.setCursor(0, 1);
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print(String(MSBVal));
    prevCLK0 = currCLK0;
  }
  // pressing the encoder will reset the value to 0
  stateSW0 = digitalRead(encSW0);
  if (stateSW0 == LOW && millis() - lastSW0 > 50) {
    MSBVal = 0;
    lcd.setCursor(0, 1);
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print(String(MSBVal));
    lastSW0 = millis();
  }
}

/*
 * Function: LSBEncoder
 * Description: Encoder control for the LSB bank value
 */
void LSBEncoder() {
  currCLK1 = digitalRead(encCLK1);
  if (currCLK1 != prevCLK1) {
    if (digitalRead(encDT1) != currCLK1) {
      if (LSBVal == MAX_LSB_VAL) {
        LSBVal = 0;
      } else {
        LSBVal++;
      }
    } else {
      if (LSBVal == 0) {
        LSBVal = MAX_LSB_VAL;
      } else {
        LSBVal--;
      }
    }
    lcd.setCursor(5, 1);
    lcd.print("   ");
    lcd.setCursor(5, 1);
    lcd.print(String(LSBVal));
    prevCLK1 = currCLK1;
  }
  stateSW1 = digitalRead(encSW1);
  if (stateSW1 == LOW && millis() - lastSW1 > 50) {
    LSBVal = 0;
    lcd.setCursor(5, 1);
    lcd.print("   ");
    lcd.setCursor(5, 1);
    lcd.print(String(LSBVal));
    lastSW1 = millis();
  }
}

/*
 * Function: PCNumEncoder
 * Description: Encoder control for the Program Control value
 */
void PCNumEncoder() {
  currCLK2 = digitalRead(encCLK2);
  if (currCLK2 != prevCLK2) {
    if (digitalRead(encDT2) != currCLK2) {
      // Program Change values range from 1-128
      if (PCNum == 128) {
        PCNum = 1;
      } else {
        PCNum++;
      }
    } else {
      if (PCNum == 1) {
        PCNum = 128;
      } else {
        PCNum--;
      }
    }
    lcd.setCursor(10, 1);
    lcd.print("   ");
    lcd.setCursor(10, 1);
    lcd.print(String(PCNum));
    prevCLK2 = currCLK2;
  }
  stateSW2 = digitalRead(encSW2);
  if (stateSW2 == LOW && millis() - lastSW2 > 50) {
    PCNum = 0;
    lcd.setCursor(10, 1);
    lcd.print("   ");
    lcd.setCursor(10, 1);
    lcd.print(String(PCNum));
    lastSW2 = millis();
  }
}

/*
 * Function: ChanNumEncoder
 * Description: Encoder control for the MIDI channel number
 */
void ChanNumEncoder() {
  currCLK3 = digitalRead(encCLK3);
  if (currCLK3 != prevCLK3) {
    if (digitalRead(encDT3) != currCLK3) {
      // MIDI channel numbers range from 1-16
      if (chanNum == 16) {
        chanNum = 1;
      } else {
        chanNum++;
      }
    } else {
      if (chanNum == 1) {
        chanNum = 16;
      } else {
        chanNum--;
      }
    }
    lcd.setCursor(15, 1);
    lcd.print("   ");
    lcd.setCursor(15, 1);
    lcd.print(String(chanNum));
    prevCLK3 = currCLK3;
  }
  // pressing this encoder will send the displayed values over MIDI
  stateSW3 = digitalRead(encSW3);
  if (stateSW3 == LOW && millis() - lastSW3 > 50) {
    // Send a MIDI CC message to set the MSB value
    MIDI.sendControlChange(MSB_CC, MSBVal, chanNum);
    delay(300);
    // Send a MIDI CC message to set the LSB value
    MIDI.sendControlChange(LSB_CC, LSBVal, chanNum);
    delay(300);
    // Send a MIDI PC message to set the Program Number
    MIDI.sendProgramChange(PCNum, chanNum);

    lcd.setCursor(0, 2);
    lcd.print(String("                    "));
    lcd.setCursor(0, 2);
    lcd.print(String(MSBVal) + String("-") + String(LSBVal) + String("-") + String(PCNum) + String("-") + String(chanNum) + String(" sent"));
    lastSW3 = millis();
  }
}

void loop() {
  // Loop will constantly detect changes to the encoder states and react accordingly
  MSBEncoder();
  LSBEncoder();
  PCNumEncoder();
  ChanNumEncoder();
}
