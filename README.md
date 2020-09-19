# oribokit
Firmware for the oribokit - robotic origami kit
available at https://oribokit.com or https://matthewgardiner.com/oribokit

We currently only support Arduino IDE for programming oribokit. Arduino is great!

# instructions for Arduino IDE
Arduino IDE is software that works on MacOS, Windows and Linux that lets you write, compile and upload new programs to a microcontroller such as the one built into your oribokit controller board. Follow these steps to get the additional board definitions installed on your system.

1. download Arduino IDE from https://www.arduino.cc/en/Main/Software

2. Run Arduino and open PREFERENCES

3. Click the small box at the end of the field for: Additional Boards Manager URLs
Add a new line with:
https://matthewgardiner.com/downloads/firmware/package_oribokit_index.json

4. Then download the board:
Go TOOLS > Board > Board Manager
Board Manager opens, it will download information on the latest boards.
Enter oribokit in the search field, and press INSTALL
The board will be installed

5. Select the correct board version in Tools

- If you have version 1.0 
  - Go TOOLS > Board > ORIBOKIT Boards > Oribokit 1.0 Collectors Edition
  
- If you have version 1.1
  - Go TOOLS > Board > ORIBOKIT Boards > Oribokit 1.1 Ars Electronica 2020 Edition

6. Get the starter code for your board at:
https://github.com/oribotic/oribokit
You can either download the code or copy and paste the code into a new arduino sketch.

7. Read and follow the instructions at the top of the starter code about how to enable the bootloader to allow the board to be flashed, by default the upload method, under Tools > upload method, should be "STM32duino bootloader", that is correct!

8. Verify the code. Go SKETCH > VERIFY (or press the tick in the top left of the arduino window)

9. Upload the code (with the board connected by USB, in bootloader mode) Go SKETCH > UPLOAD (or press the arrow in the top left of the arduino window)


## notes and dependencies

We use slightly modified board and tools from stm32duino https://github.com/stm32duino  
Version 1.0 uses a 16MhZ clock setting and Version 1.1 is a direct copy of STM32F103C8. 
Both only give the option to compile for a 48MhZ clock, and the 64k Flash to match the MCU.



