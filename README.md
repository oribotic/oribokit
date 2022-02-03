# ORIBOKIT™
Firmware for the oribokit - robotic origami kit
available at https://oribokit.com or https://matthewgardiner.com/oribokit

We currently only support Arduino IDE for programming oribokit. Arduino is so 2000's it's like the Nirvana of microcontrolling!

# SETUP FOR ARDUINO

For Oribokit 1.0 and 1.1 see https://github.com/oribotic/oribokit/tree/master/getting-started-oribokit-STM32.md

For Oribokit 1.3 to 1.6 see https://github.com/oribotic/oribokit/tree/master/getting-started-oribokit-ATMEGA32.md

# BOARDS INFORMATION

Current versions are in https://github.com/oribotic/oribokit/tree/master/boards
[PDF containing Board information](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-boards-1.0-1.6.pdf)
![Oribokit™ Version 1.0 Collectors Edition](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.0@2x.jpg "Oribokit™ Version 1.0 Collectors Edition")
![Oribokit™ Version 1.1 Ars Electronica Edition](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-board-1.1@2x.jpg "Oribokit™ Version 1.1 Ars Electronica Edition")
![Oribokit™ Version 1.4 RGB Flower](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.4@2x.jpg "Oribokit™ Version 1.4 RGB Flower")
![Oribokit™ Version 1.5 RGB Flower II](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.5@2x.jpg "Oribokit™ Version 1.5 RGB Flower II")
![Oribokit™ Version 1.6 Flat Flex](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.6@2x.jpg "Oribokit™ Version 1.6 Flat Flex")

## notes and dependencies

### For versions 1.0 & 1.1
We use slightly modified board and tools from stm32duino https://github.com/stm32duino  
Version 1.0 uses a 16MhZ clock setting and Version 1.1 is a direct copy of STM32F103C8. 
Both only give the option to compile for a 48MhZ clock, and the 64k Flash to match the MCU.

### For versions 1.4 – 1.6
Our boards shifted to ATMEGA32u4 due to a global shortage and price hike of STM32 micros.
These boards are completely compatible with Arduino



