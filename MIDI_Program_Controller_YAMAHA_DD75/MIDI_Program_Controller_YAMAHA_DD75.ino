/*
 * This file is part of the ARMIDUINO project.
 *
 * ARMIDUINO is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ARMIDUINO is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with ARMIDUINO. If not, see <https://www.gnu.org/licenses/>. 
 */

#include <LiquidCrystal_I2C.h> //https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <MIDI.h>
#include <Wire.h>
#include "MIDI_YAMAHA_DD75_def.h"

// Set the LCD address to 0x27 for a 20 chars and 4 line display, your LCD address may vary
LiquidCrystal_I2C lcd(0x27, MAX_LCD_COLUMN, MAX_LCD_ROW);

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

byte prevChanNum = 2;
byte chanNum = 1;

int prevVoiceArrayIndex = 1;
int voiceArrayIndex = 0;

int prevCategoryArrayIndex = 1;
int categoryArrayIndex = 0;

midi_voice_data_t currentVoice;

// Initialize MIDI default configuration
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  // Start MIDI, input channel does not matter so listening on all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // initialize the LCD
  lcd.begin();
  lcd.print("YAMAHA DD75 PC CTRLR");

  // configure the pins for each encoder
  pinMode(ENCODER_CLK0, INPUT);
  pinMode(ENCODER_DT0, INPUT);
  pinMode(ENCODER_SW0, INPUT_PULLUP);
  prevCLK0 = digitalRead(ENCODER_CLK0);

  pinMode(ENCODER_CLK1, INPUT);
  pinMode(ENCODER_DT1, INPUT);
  pinMode(ENCODER_SW1, INPUT_PULLUP);
  prevCLK1 = digitalRead(ENCODER_CLK1);

  pinMode(ENCODER_CLK2, INPUT);
  pinMode(ENCODER_DT2, INPUT);
  pinMode(ENCODER_SW2, INPUT_PULLUP);
  prevCLK2 = digitalRead(ENCODER_CLK2);

  pinMode(ENCODER_CLK3, INPUT);
  pinMode(ENCODER_DT3, INPUT);
  pinMode(ENCODER_SW3, INPUT_PULLUP);
  prevCLK3 = digitalRead(ENCODER_CLK3);

  LCDUpdater();
}

/*
 * Function: scrollMessage
 * Description: Used to scroll a text on the LCD
 * Input:
 *    row - LCD row to be displayed on
 *    message - the string to be displayed
 *    delayTime - time spent on each segment in ms
 */

void scrollMessage(int row, String message, int delayTime) {
  lcd.print(message.substring(0,MAX_LCD_COLUMN));
  delay(1000);
  message = message + " ";
  for (int position = 1; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + MAX_LCD_COLUMN));
    delay(delayTime);
  }
}

/*
 * Function: LCDUpdater
 * Description: Update the LCD with the relevant information
 */
void LCDUpdater() {
  if (chanNum != prevChanNum || voiceArrayIndex != prevVoiceArrayIndex) {
    lcd.setCursor(0, 1);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Voice #" + String(voiceArrayIndex) + " Chan #" + String(chanNum));
  }

  if (voiceArrayIndex != prevVoiceArrayIndex) {
    PROGMEM_readAnything (&voiceArray[voiceArrayIndex], currentVoice);
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    lcd.setCursor(0, 3);

    lcd.setCursor(0, 3);
    lcd.print(String(currentVoice.voiceName).substring(0, MAX_LCD_COLUMN));

    categoryArrayIndex = currentVoice.category;
  }

  if (categoryArrayIndex != prevCategoryArrayIndex) {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    lcd.print(categoryArray[categoryArrayIndex].categoryName);
  }

  prevVoiceArrayIndex = voiceArrayIndex;
  prevCategoryArrayIndex = categoryArrayIndex;
  prevChanNum = chanNum;
}

/*
* Function: CategoryEncoder
* Description: Encoder control for the voice category
*/
void CategoryEncoder() {
  currCLK0 = digitalRead(ENCODER_CLK0);
  if (currCLK0 != prevCLK0) {
    // clockwise rotation of encoder increases the value
    if (digitalRead(ENCODER_DT0) != currCLK0) {
      // If you try to exceed the max value, it will return to 0
      if (categoryArrayIndex == (MAX_DD75_VOICE_CATEGORY - 1)) {
        categoryArrayIndex = 0;
      } else {
        categoryArrayIndex++;
      }
      // counter-clockwise rotation of encoder increases the value
    } else {
      if (categoryArrayIndex == 0) {
        categoryArrayIndex = (MAX_DD75_VOICE_CATEGORY - 1);
      } else {
        categoryArrayIndex--;
      }
    }
    voiceArrayIndex = categoryArray[categoryArrayIndex].firstInstance;
    LCDUpdater();
    prevCLK0 = currCLK0;
  }
  // Pressing Encoder0 will print the entire name of a voice that exceeds the bounds of the LCD
  stateSW0 = digitalRead(ENCODER_SW0);
  if (currentVoice.voiceNameLen > MAX_LCD_COLUMN && stateSW0 == LOW) {
    PROGMEM_readAnything (&voiceArray[voiceArrayIndex], currentVoice);
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    lcd.setCursor(0, 3);
    scrollMessage(3, String(currentVoice.voiceName), 300);
    lcd.setCursor(0, 3);
    lcd.print(String(currentVoice.voiceName).substring(0, MAX_LCD_COLUMN));
    LCDUpdater();
  }
}

/*
 * Function: HundredVoiceEncoder
 * Description: Encoder control for hundreds place of voiceArrayIndex
 */
void HundredVoiceEncoder() {
  currCLK1 = digitalRead(ENCODER_CLK1);
  if (currCLK1 != prevCLK1) {
    if (digitalRead(ENCODER_DT1) != currCLK1) {
      if (voiceArrayIndex >= 500) {
        voiceArrayIndex = voiceArrayIndex % 100;
      } else {
        voiceArrayIndex += 100;
        if (voiceArrayIndex >= MAX_DD75_VOICES) {
          voiceArrayIndex = MAX_DD75_VOICES - 1;
        }
      }
    } else {
      if (voiceArrayIndex < 100) {
        voiceArrayIndex += 500;
        if (voiceArrayIndex >= MAX_DD75_VOICES) {
          voiceArrayIndex = MAX_DD75_VOICES - 1;
        }
      } else {
        voiceArrayIndex -= 100;
      }
    }
    LCDUpdater();
    prevCLK1 = currCLK1;
  }
  stateSW1 = digitalRead(ENCODER_SW1);
  if (stateSW1 == LOW && millis() - lastSW1 > 50) {
    voiceArrayIndex = voiceArrayIndex % 100;
    LCDUpdater();
    lastSW1 = millis();
  }
}

/*
 * Function: SingleVoiceEncoder
 * Description: Encoder control for the voiceArrayIndex
 */
void SingleVoiceEncoder() {
  currCLK2 = digitalRead(ENCODER_CLK2);
  if (currCLK2 != prevCLK2) {
    if (digitalRead(ENCODER_DT2) != currCLK2) {
      if (voiceArrayIndex == (MAX_DD75_VOICES - 1)) {
        voiceArrayIndex = 0;
      } else {
        voiceArrayIndex++;
      }
    } else {
      if (voiceArrayIndex == 0) {
        voiceArrayIndex = (MAX_DD75_VOICES - 1);
      } else {
        voiceArrayIndex--;
      }
    }
    LCDUpdater();
    prevCLK2 = currCLK2;
  }
  stateSW2 = digitalRead(ENCODER_SW2);
  if (stateSW2 == LOW && millis() - lastSW2 > 50) {
    voiceArrayIndex = 0;
    LCDUpdater();
    lastSW2 = millis();
  }
}

/*
 * Function: ChanNumEncoder
 * Description: Encoder control for the MIDI channel number
 */
void ChanNumEncoder() {
  currCLK3 = digitalRead(ENCODER_CLK3);
  if (currCLK3 != prevCLK3) {
    if (digitalRead(ENCODER_DT3) != currCLK3) {
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
    LCDUpdater();
    prevCLK3 = currCLK3;
  }
  // pressing this encoder will send the required MIDI messages to activate the listed voice
  stateSW3 = digitalRead(ENCODER_SW3);
  if (stateSW3 == LOW && millis() - lastSW3 > 50) {
    PROGMEM_readAnything (&voiceArray[voiceArrayIndex], currentVoice);
    // Send a MIDI CC message to set the MSB value
    MIDI.sendControlChange(MSB_CC, currentVoice.MSBValue, chanNum);
    delay(300);
    // Send a MIDI CC message to set the LSB value
    MIDI.sendControlChange(LSB_CC, currentVoice.LSBValue, chanNum);
    delay(300);
    // Send a MIDI PC message to set the Program Number
    MIDI.sendProgramChange((currentVoice.PCNumber - 1), chanNum);
    /*
    The MIDI spec states that PC numbers are 1-128, but for the DD75 I've observed an issue
    where the actual voice that is chosen is the PC number +1. So when I send PC #2, it
    actually selects the voice that is associated with PC #3. I'm not sure as to why, but
    that is why I decrement the PCNumber by 1 when I send it above.
    */

    lcd.setCursor(0, 1);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Sending Voice #" + String(voiceArrayIndex));
    lcd.setCursor(0, 2);
    lcd.print("on MIDI channel #" + String(chanNum));
    delay(1500);

    // change the stored previous values to trigger LCDUpdater to reset the text
    prevChanNum++;
    prevCategoryArrayIndex++;
    LCDUpdater();
  }
}

void loop() {
  // Loop will constantly detect changes to the encoder states and react accordingly
  CategoryEncoder();
  HundredVoiceEncoder();
  SingleVoiceEncoder();
  ChanNumEncoder();
}
