

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
extern int servoPosition2;
extern int servoPosition3;
extern int average1;
extern int average2;
extern int average3;
extern int minLight;
extern int maxLight;
extern uint8_t mode;
extern uint8_t minAngle1;
extern uint8_t maxAngle1;
extern uint8_t minAngle2;
extern uint8_t maxAngle2;
extern uint8_t minAngle3;
extern uint8_t maxAngle3;


void updateOSC();

void sendOSC(char oscAddress[20], uint8_t arg1=3333, uint16_t arg2=3333);
bool isNumber(OSCMessage &msg, int arg);
int getNumber(OSCMessage &msg, int arg);
void rxOSC();

void setLDRHigh (OSCMessage &msg, int arg);
void setLDRLow (OSCMessage &msg, int arg);
void setServoHigh (OSCMessage &msg, int arg);
void setServoLow (OSCMessage &msg, int arg);
