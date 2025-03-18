/*
 * This file is part of the ARMIDUINO project.
 *
 * ARMIDUINO is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ARMIDUINO is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with ARMIDUINO. If not, see <https://www.gnu.org/licenses/>. 
 */

#include <LiquidCrystal_I2C.h> // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <MIDI.h>
#include <Wire.h>
#include "MIDI_Program_Controller.h"

// Set the LCD address to 0x27 for a 20 chars and 4 line display, your LCD address may vary
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Initializing variables for tracking Encoders
byte prevCLK0;
byte currCLK0;
byte prevStateSW0 = HIGH;
byte stateSW0 = HIGH;
unsigned long lastSW0 = 0;

byte prevCLK1;
byte currCLK1;
byte prevStateSW1 = HIGH;
byte stateSW1 = HIGH;
unsigned long lastSW1 = 0;

byte prevCLK2;
byte currCLK2;
byte prevStateSW2 = HIGH;
byte stateSW2 = HIGH;
unsigned long lastSW2 = 0;

byte prevCLK3;
byte currCLK3;
byte prevStateSW3 = HIGH;
byte stateSW3 = HIGH;
unsigned long lastSW3 = 0;

program_change_properties_t activeProgChng; // Parameters of the selected MIDI message in the default menu

program_change_properties_t presetProgChngArray[MAX_PRESET_VAL]; // stores MIDI message info for presets

byte presetNum = 1;
byte menuMode = 0; // 0 == default menu, 1 == preset menu
byte sendFlag = false; // toggled to 'true' when a MIDI message is sent

unsigned long sendTime = millis(); // Tracks how long since a MIDI message was sent

byte autoSendFlag = false; // toggled to 'true' if auto sending is enabled

// Initialize MIDI default configuration
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  // Start MIDI, input channel does not matter so listening on all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // initialize the LCD
  lcd.begin();
  DefaultDisp();

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
 * Function: LCDClearLine
 * Description: Clears a line from the LCD
 * Input:
 *   yPos - the line that needs clearing, expects value of 0 to 3
 */
void LCDClearLine(byte yPos) {
  if (yPos > 3) { // in a 4 line display, the possible range of values is 0-3
    return;
  }
  lcd.setCursor(0, yPos);
  lcd.print("                    ");
}

/*
 * Function: DispValues
 * Description: Writes the MIDI CC, PC, and channel info to the LCD
 * Inputs:
 *   MSB - most significant bit value, expects value of 0 to 127
 *   LSB - least significant bit value, expects value of 0 to 127
 *   PC - program change number, expects value of 1 to 128
 *   chan - channel number, expects value of 1 to 16
 */
void DispValues(byte MSB, byte LSB, byte PC, byte chan) {
  if (MSB > MAX_CC_VAL || LSB > MIN_CC_VAL || PC < MIN_PC_VAL || PC > MAX_PC_VAL || chan < MIN_MIDI_CH_VAL || chan > MAX_MIDI_CH_VAL) {
    return;
  }
  LCDClearLine(1);
  lcd.setCursor(0, 1);
  lcd.print(MSB);
  lcd.setCursor(5, 1);
  lcd.print(LSB);
  lcd.setCursor(10, 1);
  lcd.print(PC);
  lcd.setCursor(15, 1);
  lcd.print(chan);
}

/*
 * Function: DefaultDisp
 * Description: Display the default menu
 */
void DefaultDisp() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MSB  LSB  PC#  CH#");
  DispValues(activeProgChng.MSBVal, activeProgChng.LSBVal, activeProgChng.PCNum, activeProgChng.chanNum);
}

/*
 * Function: PresetMenu
 * Description: Display the preset menu
 */
void PresetMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Preset #");
  lcd.print(presetNum);
  DispValues(presetProgChngArray[presetNum - 1].MSBVal, presetProgChngArray[presetNum - 1].LSBVal, presetProgChngArray[presetNum - 1].PCNum, presetProgChngArray[presetNum - 1].chanNum);
}

/*
 * Function: printMIDIMsgSnt
 * Description: Provides visual feedback to users
 */
void printMIDIMsgSnt() {
  LCDClearLine(2);
  lcd.setCursor(0, 2);
  lcd.print("MIDI message sent");
  sendFlag = true;
  sendTime = millis();
}

/*
 * Function: SendMIDIMessage
 * Description: Sends the two MIDI CC values and single PC value to the provided MIDI channel
 * Inputs:
 *   MSB - most significant bit value, expects value of 0 to 127
 *   LSB - least significant bit value, expects value of 0 to 127
 *   PC - program change number, expects value of 1 to 128
 *   chan - channel number, expects value of 1 to 16
 */
void SendMIDIMessage(byte MSB, byte LSB, byte PC, byte chan) {
  if (MSB > MAX_CC_VAL || LSB > MIN_CC_VAL || PC < MIN_PC_VAL || PC > MAX_PC_VAL || chan < MIN_MIDI_CH_VAL || chan > MAX_MIDI_CH_VAL) {
    return;
  }
  // Send a MIDI CC message to set the MSB value
  MIDI.sendControlChange(MSB_CC, MSB, chan);
  delay(MIDI_DELAY);
  // Send a MIDI CC message to set the LSB value
  MIDI.sendControlChange(LSB_CC, LSB, chan);
  delay(MIDI_DELAY);
  // Send a MIDI PC message to set the Program Number
  MIDI.sendProgramChange(PC, chan);
  printMIDIMsgSnt();  
}

/*
 * Function: EncoderFunc
 * Description: Reads changes in encoder state and sets values accordingly
 */
void EncoderFunc(byte encDT, byte currCLK, byte *val, byte maxVal, byte minVal, byte xPos, byte yPos, byte *prevCLK) {
  if (digitalRead(encDT) != currCLK) {
      if (*val == maxVal) {
        *val = minVal;
      } else {
        *val += 1;
      }
    } else {
      if (*val == minVal) {
        *val = maxVal;
      } else {
        *val -= 1;
      }
    }
    lcd.setCursor(xPos, yPos);
    lcd.print("   ");
    lcd.setCursor(xPos, yPos);
    lcd.print(*val);
    *prevCLK = currCLK;
}

/*
 * Function: Encoder0
 * Description: Encoder control for the MSB bank value and the Preset menu
 */
void Encoder0() {
  currCLK0 = digitalRead(encCLK0);
  if (currCLK0 != prevCLK0) {
    if (menuMode) { // Preset Menu
      EncoderFunc(encDT0, currCLK0, &presetNum, MAX_PRESET_VAL, MIN_PRESET_VAL, 8, 0, &prevCLK0);
      DispValues(presetProgChngArray[presetNum - 1].MSBVal, presetProgChngArray[presetNum - 1].LSBVal, presetProgChngArray[presetNum - 1].PCNum, presetProgChngArray[presetNum - 1].chanNum);
      if (autoSendFlag) {
        SendMIDIMessage(presetProgChngArray[presetNum - 1].MSBVal, presetProgChngArray[presetNum - 1].LSBVal, presetProgChngArray[presetNum - 1].PCNum, presetProgChngArray[presetNum - 1].chanNum);
        printMIDIMsgSnt();
      }
    } else { // Default Menu
      EncoderFunc(encDT0, currCLK0, &activeProgChng.MSBVal, MAX_CC_VAL, MIN_CC_VAL, 0, 1, &prevCLK0);
      if (autoSendFlag) {
        MIDI.sendControlChange(MSB_CC, activeProgChng.MSBVal, activeProgChng.chanNum);
        printMIDIMsgSnt();
      }
    }
  }
  // pressing the encoder will toggle between the default menu and the preset menu
  stateSW0 = digitalRead(encSW0);
  if ((stateSW0 == LOW && (stateSW0 != prevStateSW0)) && ((millis() - lastSW0) > DEBOUNCE)) {
    lastSW0 = millis();
    prevStateSW0 = LOW;
    if (menuMode) {
      DefaultDisp();
      menuMode = 0;
    } else {
      PresetMenu();
      menuMode = 1;
    }
  } else if (stateSW0 == HIGH) {
    prevStateSW0 = HIGH;
  }
}

/*
 * Function: Encoder1
 * Description: Encoder control for the LSB bank value and setting Presets
 */
void Encoder1() {
  currCLK1 = digitalRead(encCLK1);
  if (currCLK1 != prevCLK1 && !menuMode) {
    if (!menuMode) {
      EncoderFunc(encDT1, currCLK1, &activeProgChng.LSBVal, MAX_CC_VAL, MIN_CC_VAL, 5, 1, &prevCLK1);
      if (autoSendFlag) {
        MIDI.sendControlChange(LSB_CC, activeProgChng.LSBVal, activeProgChng.chanNum);
        printMIDIMsgSnt();
      }
    }
  }
  stateSW1 = digitalRead(encSW1);
  if ((stateSW1 == LOW && stateSW1 != prevStateSW1) && ((millis() - lastSW1) > DEBOUNCE) && !menuMode) {
    lastSW1 = millis();
    prevStateSW1 = LOW;
    // Set as preset, enter dialogue that allows user to choose which preset slot to fill
    byte tmpPreset = 1;
    lcd.setCursor(0, 2);
    lcd.print("Choose preset slot:");
    lcd.setCursor(0, 3);
    lcd.print(tmpPreset);
    while(true) {
      currCLK1 = digitalRead(encCLK1);
      if (currCLK1 != prevCLK1) {
        EncoderFunc(encDT1, currCLK1, &tmpPreset, MAX_PRESET_VAL, MIN_PRESET_VAL, 0, 3, &prevCLK1);
      }
      stateSW1 = digitalRead(encSW1);
      stateSW0 = digitalRead(encSW0);
      if ((stateSW1 == LOW && (stateSW1 != prevStateSW1)) && ((millis() - lastSW1) > DEBOUNCE)) { // save and exit
        lastSW1 = millis();
        prevStateSW1 = LOW;
        presetProgChngArray[tmpPreset - 1].MSBVal = activeProgChng.MSBVal;
        presetProgChngArray[tmpPreset - 1].LSBVal = activeProgChng.LSBVal;
        presetProgChngArray[tmpPreset - 1].PCNum = activeProgChng.PCNum;
        presetProgChngArray[tmpPreset - 1].chanNum = activeProgChng.chanNum;
        LCDClearLine(2);
        LCDClearLine(3);
        break;
      } else if (stateSW1 == HIGH) {
        prevStateSW1 = HIGH;
      }
      if ((stateSW0 == LOW && (stateSW0 != prevStateSW0)) && ((millis() - lastSW0) > DEBOUNCE)) { // exit without saving
        lastSW0 = millis();
        prevStateSW0 = LOW;
        LCDClearLine(2);
        LCDClearLine(3);
        break;
      } else if (stateSW0 == HIGH) {
        prevStateSW0 = HIGH;
      }
    }
  } else if (stateSW1 == HIGH) {
    prevStateSW1 = HIGH;
  }
}

/*
 * Function: Encoder2
 * Description: Encoder control for the Program Control value and toggle for auto sending MIDI messages
 */
void Encoder2() {
  currCLK2 = digitalRead(encCLK2);
  if (currCLK2 != prevCLK2) {
    if (!menuMode) {
      EncoderFunc(encDT2, currCLK2, &activeProgChng.PCNum, MAX_PC_VAL, MIN_PC_VAL, 10, 1, &prevCLK2);
      if (autoSendFlag) {
        MIDI.sendProgramChange(activeProgChng.PCNum, activeProgChng.chanNum);
        printMIDIMsgSnt();
      }
    }
  }
  stateSW2 = digitalRead(encSW2);
  if ((stateSW2 == LOW && stateSW2 != prevStateSW2) && ((millis() - lastSW2) > DEBOUNCE)) {
    // Toggle automatic message send VS manual message send
    lastSW2 = millis();
    prevStateSW2 = LOW;
    if (autoSendFlag) {
      autoSendFlag = false;
      if (sendFlag) {
        sendFlag = false;
        LCDClearLine(2);
        sendFlag = false;
      }
    } else {
      autoSendFlag = true;
    }
  } else if (stateSW2 == HIGH) {
    prevStateSW2 = HIGH;
  }
}

/*
 * Function: Encoder3
 * Description: Encoder control for the MIDI channel number and sending MIDI messages
 */
void Encoder3() {
  currCLK3 = digitalRead(encCLK3);
  if (currCLK3 != prevCLK3) {
    if (!menuMode) {
      EncoderFunc(encDT3, currCLK3, &activeProgChng.chanNum, MAX_MIDI_CH_VAL, MIN_MIDI_CH_VAL, 15, 1, &prevCLK3);
    }
  }
  // pressing this encoder will send the displayed values over MIDI
  stateSW3 = digitalRead(encSW3);
  if ((stateSW3 == LOW && stateSW3 != prevStateSW3) && ((millis() - lastSW3) > DEBOUNCE)) {
    if (menuMode) {
      SendMIDIMessage(presetProgChngArray[presetNum - 1].MSBVal, presetProgChngArray[presetNum - 1].LSBVal, presetProgChngArray[presetNum - 1].PCNum, presetProgChngArray[presetNum - 1].chanNum);
    } else {
      SendMIDIMessage(activeProgChng.MSBVal, activeProgChng.LSBVal, activeProgChng.PCNum, activeProgChng.chanNum);
    }
    lastSW3 = millis();
    prevStateSW3 = LOW;
  } else if (stateSW3 == HIGH) {
    prevStateSW3 = HIGH;
  }
}

void loop() {
  // Loop will constantly detect changes to the encoder states and react accordingly
  Encoder0();
  Encoder1();
  Encoder2();
  Encoder3();
  // Wipes the "MIDI message sent" notification
  if (sendFlag) {
    if (millis() - sendTime >= MESSAGE_DURATION) {
      LCDClearLine(2);
      sendFlag = false;
    }
  }
}
