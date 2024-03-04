# ARMIDUINO

PROJECT IS WIP, this documentation is incomplete and will be updated as I make progress

An Arduino based MIDI<sup>0</sup> PC<sup>1</sup> controller.

## Description

The **ARMIDUINO** is a combination software and hardware project. Technically no sofware skills are required, but a rudimentary comprehension of Arduino programming is recommended. If everything works first try, then there is no need to touch the software; however, there may be differences in our hardware that require alterations to the code. A beginner-level understanding of circuitry is required because this project is self-assembled.

This project aims to provide a solution (or starting point) for any MIDI hobbyists that would like to build their own simple MIDI PC controller that can specify the MSB<sup>2</sup> bank value, the LSB<sup>3</sup> bank value, the PC number, as well as the MIDI channel.

This project was inspired by the fact that I was not able to find a simple standalone MIDI PC controller that was both relatively affordable and clear on how to send PC messages to a system that involved banks reliant on both a MSB and LSB value. There are plenty of commercially available MIDI controllers and pedals out there that can do what this device can do and so much more, but they didn't seem very intuitive to someone like me who was relatively inexperienced with MIDI (before I started this project) and were simply overkill for what I needed to do. Hopefully this project helps others who are in the same boat. 

I have provided two separate software implementations that rely on the exact same hardware: one generic and one device-specific (although this one can be tailored for your own use-cases); more on these further below.

### Brief explanation of Banks and MIDI PC Messages

Here is an aside to explain how Banks and MIDI PC messages work on a more abstract level before I get into the implementation details of the software itself. MIDI is a software protocol that is useful for controlling musical instruments whether thats through a DAW<sup>4</sup> or a Synthesizer. Its most common use-case is for sending musical note data across devices, but it can also send other messages for controlling specific parameters. The types of messages required for this project are MIDI CC<sup>5</sup> messages and MIDI PC messages. Specifically one MIDI CC message is required to set the MSB bank value, another MIDI CC message is required to set the LSB bank value, and one MIDI PC message is required to specify the PC number. Here is a table to visualize what you need in order to successfully specify a voice/program on a system that requires a MSB value, LSB value, as well as a PC number:

| Name      | MIDI message type | Control Number | Value | MIDI Channel | Comments                                                                 |
|-----------|-------------------|----------------|-------|--------------|--------------------------------------------------------------------------|
| MSB Value | CC                | 0x00           | 0-127 | 1-16         | These control numbers are standard, refer to the MIDI spec for more info |
| LSB Value | CC                | 0x20           | 0-127 | 1-16         | 0x20 is hexadecimal for 32                                               |
| PC Number | PC                | N/A            | 1-128 | 1-16         | PC is seperate from CC and therefore has no control number               |

*The order in which these are sent do not matter*


### MIDI Program Controller (Generic Implementation)

### MIDI Program Controller YAMAHA DD75 (Device-Specific Implementation)

[Yamaha DD-75 MIDI Reference guide](https://usa.yamaha.com/files/download/other_assets/3/892623/dd75_en_mr_a0_web.pdf)

## Required Parts

- 1x [Arduino Mega](https://store-usa.arduino.cc/products/arduino-mega-2560-rev3?selectedStore=us) (revision agnostic)
- 2x 220 ohm resistors
- 4x rotary encoders
- 1x 5 pin MIDI socket
- 1x 20x4 LCD display (model# 2004A) with a 4 pin input backpack
- Wires

## Optional (but recommended) Parts

- [3D printed enclosure](Thingiverse link)
- Assortment of standoffs (Measurements to come)
- 1x protoboard

## Assembly Instructions


## Features



## Known bugs

- Rotating the encoders too quickly in one direction will generate a signal that corresponds with a rotation in the opposite direction meaning the desired outcome may be reversed (e.g. incrementing a value to quickly may actually decrement it instead). This may be due to the lack of debouncing for the rotatry encoder output.

## Appendix
<sup>0</sup>*Musical Instrument Digital Interface* \
<sup>1</sup>*Program Change* \
<sup>2</sup>*Most Significant Bit* \
<sup>3</sup>*Least Significant Bit* \
<sup>4</sup>*Digital Audio Workstation* \
<sup>5</sup>[*Control Change*](https://www.midi.org/specifications-old/item/table-3-control-change-messages-data-bytes-2)

## Version Log