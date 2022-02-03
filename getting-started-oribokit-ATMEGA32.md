# ORIBOKIT 1.3 - 1.6 (ATMEGA32u4)

## SETUP INSTRUCTIONS for Arduino IDE

Arduino IDE is software that works on MacOS, Windows and Linux that lets you write, compile and upload new programs to a microcontroller such as the one built into your oribokit controller board. Follow these steps to get started with writing your own code for oribokit.

Get the starter code suitable for your board at:
https://github.com/oribotic/oribokit
You can either download the code or copy and paste the code into a new arduino sketch.
   
## TO PROGRAM

1. Download and run the latest Arduino IDE https://arduino.cc
2. Connect USB-C cable to oribokit and your computer USB port (automatically install or manually install drivers on windows https://www.arduino.cc/en/Guide/DriverInstallation)
3. Select Tools > Board > Arduino Micro
4. Select Tools > Port > [your USB or COM port]
5. Compile and upload
6. If the RESET LED flashes during programming, press RESET button (near USB connector)

For additional help on getting started, including installation of USB drivers, see https://docs.arduino.cc/retired/getting-started-guides/ArduinoLeonardoMicro

## Additional system related information

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


# Pin information

**EXP 1**
*2.54mm 6 pin header*
```
LABEL      FUNCTION             PIN
--------------------------------------
TX         TX Serial            Pin 1
RX         RX Serial            Pin 0
5V         5V
G          Ground
SDA        I2C Serial Data      Pin 2
SCL        I2C Serial Clock     Pin 3
```

**EXP 2** 
*2.54mm 6 pin header*
```
LABEL      FUNCTION             PIN
--------------------------------------
INT        Interrupt
A5         10 Bit Analog        Pin 5
A4         10 Bit Analog        Pin 4
A3         10 Bit Analog        Pin 3
A2         10 Bit Analog        Pin 2
A1         10 Bit Analog        Pin 1		
```

**RGB [1 - 3]** 
*2.54mm 3 pin header*
```
LABEL      FUNCTION             PIN
--------------------------------------
GND        Ground  
5V         5V DIGITAL
DATA       RGB1, RGB2, RGB3
RGB1       PWM PB6              Pin 10
RGB2       PWM PC6              Pin 5
RGB3       PWM PC7              Pin 13
```                             

**SERVO [1 - 3]**               
*2.54mm 3 pin header*           
```
LABEL      FUNCTION             PIN
--------------------------------------
GND        Ground               
5V         5V DIGITAL           
SERVO1     PWM PB5              Pin 9
SERVO2     PWM PD7              Pin 6
SERVO3     PWM PD6              Pin 12
```                             

**FPC [1 - 3]**                 
*0.5mm Flat Flex Cable*         

```
LABEL      FUNCTION             PIN
--------------------------------------
GND        Ground               
5V         5V DIGITAL           
RGB OUT    Not connected        
RGB IN     RGB1 RGB2 RGB3       
A4         10 Bit Analog        Pin 4
A3         10 Bit Analog        Pin 3
A2         10 Bit Analog        Pin 2
A1         10 Bit Analog        Pin 1
```

# Board information

![Oribokit™ Version 1.4 RGB Flower](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.4@2x.jpg "Oribokit™ Version 1.4 RGB Flower")
![Oribokit™ Version 1.5 RGB Flower II](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.5@2x.jpg "Oribokit™ Version 1.5 RGB Flower II")
![Oribokit™ Version 1.6 Flat Flex](https://github.com/oribotic/oribokit/blob/master/boards/oribokit-1.6@2x.jpg "Oribokit™ Version 1.6 Flat Flex")
