
#ifndef COMMS_OSC_H
    #define COMMS_OSC_H
    #include <OSCMessage.h>
    #include <OSCBoards.h>
    
    #ifdef BOARD_HAS_USB_SERIAL
      #include <SLIPEncodedUSBSerial.h>
      extern SLIPEncodedUSBSerial SLIPSerial;
    #else
      #include <SLIPEncodedSerial.h>
      extern SLIPEncodedSerial SLIPSerial;
    #endif
#endif

#include <ArduinoUniqueID.h>

extern int LDRreading;
extern int servoPosition1;
extern int last_servoPosition1;
extern int average1;
extern int minLight;
extern int maxLight;
extern uint8_t mode;
extern uint8_t previousMode;
extern uint8_t minAngle1;
extern uint8_t maxAngle1;

extern double counterA;
extern double counterB;
extern int limitA;
extern int limitB;
extern int loopDelay;
extern uint8_t bloomCounter;

extern void writeIntEEPROM(int address, int number);

void updateOSC();
void updateTimersOSC();
void updateCountersOSC();

void sendOSC(char oscAddress[20], uint8_t arg1=3333, uint16_t arg2=3333);
bool isNumber(OSCMessage &msg, int arg);
int getNumber(OSCMessage &msg, int arg);
void rxOSC();

void setLDRHigh (OSCMessage &msg, int arg);
void setLDRLow (OSCMessage &msg, int arg);
void setServoHigh (OSCMessage &msg, int arg);
void setServoLow (OSCMessage &msg, int arg);

void setServoAngleMax (OSCMessage &msg);
void setServoAngleMin (OSCMessage &msg);

void readConfig();
void saveConfig();