/*
 * Copyright (c) 2023 Matthew Gardiner
 *
 * MIT License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/oribotic/oribotic-instruments for documentation
 *
 */

#include <ArduinoUniqueID.h>
#include "oribokit.osc.h"
#include "oribokit.h"

#ifdef BOARD_HAS_USB_SERIAL
    #include <SLIPEncodedUSBSerial.h>
    SLIPEncodedUSBSerial SLIPSerial(thisBoardsSerialUSB);
  #else
    #include <SLIPEncodedSerial.h>
    SLIPEncodedSerial SLIPSerial(Serial1);
#endif

uint8_t oscTimer = 0;

void updateOSC()
{
  // report servo positions
  // report ldr readings

  sendOSC("/ldr", LDRreading);
  if (servoPosition1 != last_servoPosition1)
    sendOSC("/servo/pos", 1, servoPosition1);
  
  if (oscTimer == 255)
  {
    sendOSC("/servo/min", 1, minAngle1);
    sendOSC("/servo/max", 1, maxAngle1);
    sendOSC("/average", 1, average1);
  }
  oscTimer ++;
} 

void updateTimersOSC()
{
  sendOSC("/timers/A", limitA);
  sendOSC("/timers/B", limitB);
}

void updateCountersOSC()
{
  sendOSC("/counters/A", counterA);
  sendOSC("/counters/B", counterB);
}

// variant two (NEW) for sending instruments messages 
void sendOSC(char oscAddress[20], uint8_t arg1=3333, uint16_t arg2=3333)
{
    OSCMessage msg(oscAddress);
    // add argument 1
    if (arg1 != 3333)
        msg.add(arg1);
    // add argument 2
    if (arg2 != 3333)
        msg.add(arg2);
    SLIPSerial.beginPacket();
    msg.send(SLIPSerial);   // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    msg.empty();            // free space occupied by message
}

bool isNumber(OSCMessage &msg, int arg)
{
  return msg.isInt(arg) || msg.isFloat(arg);
}

int getNumber(OSCMessage &msg, int arg)
{
  return (msg.isFloat(arg) ? (int)msg.getFloat(arg) : msg.getInt(arg));
}

void setLDRHigh (OSCMessage &msg)
{
  maxLight = average1;
  sendOSC("/ack/ldr/hi", maxLight);
  saveConfig ();                                      // save the config
  readConfig ();                                      // read the config
}

void setLDRLow (OSCMessage &msg)
{
  minLight = average1;
  sendOSC("/ack/ldr/lo", minLight);
  saveConfig ();                                      // save the config
  readConfig ();                                      // read the config
}

void setServoAngleMax (OSCMessage &msg)
{
  if (isNumber(msg, 0))
  {
    uint8_t servo = getNumber(msg, 0);
    uint8_t value = getNumber(msg, 1);
    
    switch (servo)
    {
      case 1:
        maxAngle1 = value;
        writeIntEEPROM(EEPROM_MAX1, value);
        break;
    }
    sendOSC("/ack/servo/max", servo, value);
  }
    
}

void setServoAngleMin (OSCMessage &msg)
{
  if (isNumber(msg, 0))
  {
    uint8_t servo = getNumber(msg, 0);
    uint8_t value = getNumber(msg, 1);
    
    switch (servo)
    {
      case 1:
        minAngle1 = value;
        writeIntEEPROM(EEPROM_MIN1, value);
        break;
    }
    sendOSC("/ack/servo/min", servo, value);
  }
    
}

void setMode (OSCMessage &msg)
{
  if (isNumber(msg, 0))
  {
    uint8_t val = getNumber(msg, 0);
    mode = val;
    sendOSC("/ack/mode", val);
  }
} 

void setLoopDelay (OSCMessage &msg)
{
  if (isNumber(msg, 0))
  {
    uint8_t val = getNumber(msg, 0);
    loopDelay = val;
    writeIntEEPROM(40, loopDelay);
    sendOSC("/ack/loopdelay", val);
  }
}

void setServoPosition (OSCMessage &msg)
{
  if (isNumber(msg, 0))
  {
    uint8_t servo = getNumber(msg, 0);
    int value = getNumber(msg, 1);
    servoPosition1 = abs(180-(SERVODIRECTION*value))%180;
    sendOSC("/ack/servo/pos", servo, servoPosition1);
  }
}

void startBloom (OSCMessage &msg)
{
  // switch modes and save the previous mode
  previousMode = mode;
  mode = MODE_BLOOM;
  bloomCounter = 255;
}

void rxOSC()
{
  OSCMessage messageIN;
  int sizeb = 0;
  while (!SLIPSerial.endofPacket())
  {
    if ((sizeb = SLIPSerial.available()) > 0)
    {
      while (sizeb--)
      {
        messageIN.fill(SLIPSerial.read());
      }
    }
  }
  if (!messageIN.hasError())
  {
    // registered ".dispatch" functions need signature -> void dispatchAddress(OSCMessage &msg){ }
    // registered ".route" functions need signature -> void routeAddress(OSCMessage &msg, int addressOffset){ }

    // eeprom 
    // messageIN.dispatch("/readConfig", dispatchEepromRead);
    // messageIN.dispatch("/writeConfig", eepromWrite);

    // panel config methods
    //messageIN.dispatch("/sensor/light", sensorLight);
    messageIN.dispatch("/set/ldr/hi", setLDRHigh);
    messageIN.dispatch("/set/ldr/lo", setLDRLow);
    messageIN.dispatch("/set/servo/min", setServoAngleMin);
    messageIN.dispatch("/set/servo/max", setServoAngleMax);
    messageIN.dispatch("/set/servo/pos", setServoPosition);
    messageIN.dispatch("/set/mode", setMode);
    messageIN.dispatch("/set/loopDelay", setLoopDelay);
    messageIN.dispatch("/bloom", startBloom);
  }
}
