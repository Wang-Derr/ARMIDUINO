/*
 * This file is part of the ARMIDUINO project.
 *
 * ARMIDUINO is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ARMIDUINO is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with ARMIDUINO. If not, see <https://www.gnu.org/licenses/>. 
 */

#define DEBOUNCE 100

#define MSB_CC 0 // standard MSB bank MIDI control change number
#define LSB_CC 32 // standard LSB bank MIDI control change number
#define MAX_CC_VAL 127 // maximum MIDI control change value
#define MIN_CC_VAL 0 // minimum MIDI control change value

#define MAX_PC_VAL 128 // maximum MIDI program change value
#define MIN_PC_VAL 1 // minimum MIDI program change value

#define MAX_MIDI_CH_VAL 16 // maximum MIDI channel value
#define MIN_MIDI_CH_VAL 1 // minimum MIDI channel value

#define MAX_PRESET_VAL 16 // maximum number of presets, relatively arbitrary though
#define MIN_PRESET_VAL 1 // minimum number of presets

#define MESSAGE_DURATION 5000 // duration of the "MIDI message sent" message in milliseconds

#define MIDI_DELAY 300 // delay between MIDI messages measured in milliseconds

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

// Struct that contains the 4 MIDI parameters that get sent out
typedef struct program_change_properties {
  byte MSBVal = 0; // Most significant bit, it's a CC value
  byte LSBVal = 0; // Least significant bit, it's a CC value
  byte PCNum = 1; // Program change number
  byte chanNum = 1; // MIDI channel number
} program_change_properties_t;