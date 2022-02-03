# ORIBOKIT 1.0 - 1.1 (STM32F103 micro)

## SETUP INSTRUCTIONS for Arduino IDE

Arduino IDE is software that works on MacOS, Windows and Linux that lets you write, compile and upload new programs to a microcontroller such as the one built into your oribokit controller board. Follow these steps to get the oribokit board definitions installed on your system.

### macos 
The instructions below work perfectly on MacOS, as (generally) no additional drivers are required. 
If you are having upload problems, enable verbose debugging in the Arduino preferences.

You may need to install libusb if you get this error:

_dyld: Library not loaded: /opt/local/lib/libusb-1.0.0.dylib_

A fix is to use brew to install the library using this command in the terminal. If you don’t have brew, you can install it from https://brew.sh/

> brew install libusb

### windows
If you are using windows you need to follow the instructions here: https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Maple-drivers using the windows drivers downloadable here: https://github.com/rogerclarkmelbourne/Arduino_STM32/tree/master/drivers/win

### linux
I've not yet investigated how to use the STMduino bootloader under Linux yet, please get in touch if you need this support and we can find a solution together.

## instructions
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