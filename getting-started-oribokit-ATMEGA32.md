# ORIBOKIT 1.3 - 1.6 (ATMEGA32u4)


**Contents** 

1. [programming instructions](#programming-instructions)
2. [system information](#additional-system-information)
3. [pinouts](#pinouts)
4. [board information](#board-information)

## SETUP INSTRUCTIONS for Arduino IDE

Arduino IDE is software that works on MacOS, Windows and Linux that lets you write, compile and upload new programs to a microcontroller such as the one built into your oribokit controller board. Follow these steps to get started with writing your own code for oribokit.

Get the starter code suitable for your board at:
https://github.com/oribotic/oribokit
You can either download the code or copy and paste the code into a new arduino sketch.
   
## Programming Instructions

1. Download and run the latest Arduino IDE https://arduino.cc
2. Connect USB-C cable to oribokit and your computer USB port (automatically install or manually install drivers on windows https://www.arduino.cc/en/Guide/DriverInstallation)
3. Select Tools > Board > Arduino Micro
4. Select Tools > Port > [your USB or COM port]
5. Compile and upload
6. If the RESET LED flashes during programming, press RESET button (near USB connector)

For additional help on getting started, including installation of USB drivers, see https://docs.arduino.cc/retired/getting-started-guides/ArduinoLeonardoMicro

## Additional system information

### macos 
The instructions above work perfectly on MacOS, as (generally) no additional drivers are required. 
If you are having upload problems, enable verbose debugging in the Arduino preferences.

You may need to install libusb if you get this error:

_dyld: Library not loaded: /opt/local/lib/libusb-1.0.0.dylib_

A fix is to use brew to install the library using this command in the terminal. If you don’t have brew, you can install it from https://brew.sh/

> brew install libusb

### windows
If you are using windows you need to follow the instructions here: 
https://docs.arduino.cc/retired/getting-started-guides/ArduinoLeonardoMicro

# Pinouts

**LDR**
```
LABEL      FUNCTION             ARDUINO
--------------------------------------
LDR        Analog PF7           A0
```
**EXP 1**

*2.54mm 6 pin header*
```
LABEL      FUNCTION             ARDUINO
--------------------------------------
TX         TX Serial            1
RX         RX Serial            0
5V         5V
G          Ground
SDA        I2C Serial Data      2
SCL        I2C Serial Clock     3
```

**EXP 2**

*2.54mm 6 pin header*
```
LABEL      FUNCTION             ARDUINO
--------------------------------------
INT        Interrupt            7
A5         Analog PF0           A5
A4         Analog PF1           A4
A3         Analog PF4           A3
A2         Analog PF5           A2
A1         Analog PF6           A1		
```

**RGB [1 - 3]**

*3 x 2.54mm 3 pin headers*
```
LABEL      FUNCTION             ARDUINO
--------------------------------------
GND        Ground  
5V         5V DIGITAL
DATA       RGB1, RGB2, RGB3
RGB1       Digital PWM PB6      10
RGB2       Digital PWM PC6      5
RGB3       Digital PWM PC7      13
```                             

**SERVO [1 - 3]**

*3 x 2.54mm 3 pin headers*           
```
LABEL      FUNCTION             ARDUINO
--------------------------------------
GND        Ground               
5V         5V DIGITAL           
SERVO1     Digital PWM PB5      9
SERVO2     Digital PWM PD7      6
SERVO3     Digital PWM PD6      2
```                             

**FPC [1 - 3]**

*3 x 0.5mm Flat Flex Cable connectors*

```
LABEL      FUNCTION             PIN
--------------------------------------
GND        Ground               
5V         5V DIGITAL           
RGB OUT    Not connected (can be connected to another RGB input as a daisy chain)      
RGB IN     RGB1 RGB2 RGB3       
A4         Analog PF1           A4
A3         Analog PF4           A3
A2         Analog PF5           A2
A1         Analog PF6           A1
```

# Board information

![Oribokit™ Version 1.4 RGB Flower](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.4@2x.jpg "Oribokit™ Version 1.4 RGB Flower")
![Oribokit™ Version 1.5 RGB Flower II](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.5@2x.jpg "Oribokit™ Version 1.5 RGB Flower II")
![Oribokit™ Version 1.6 Flat Flex](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.6@2x.jpg "Oribokit™ Version 1.6 Flat Flex")
