/*
* Copyright (c) 2020-2023 Matthew Gardiner
*
* MIT License.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*
* See https://github.com/oribotic/oribokit/ for documentation
*
*/

/*
   ORIBOKIT® - https://oribokit.com  
   origami & robotics kit 
   ------------------------------------------------------
   Studio Matthew Gardiner 
   https://matthewgardiner.net      https://matthewgardiner.com
   
   Default Firmware for oribokit boards versions 1.4 - 1.6
    
   FEATURES
   ATMEGA32u4 http://www.atmel.com/Images/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf
   Bootloader: Arduino Micro https://www.arduino.cc/en/pmwiki.php?n=Main/arduinoBoardMicro
   1 Servo Ports 
   1 RGB Connectors - requires RGB add-on boards
   1 LDR
   V 1.6 has flat flex cable connectors for sensor extensions

   DESCRIPTION
   This software continuously reads light levels from the LDR (Light Dependent Resistor) and records a statistical variation over time.
   The statistical variation is recorded with two counters counterA and counterB, incremented in each loop.
   If the statistical variation is above LDR_STD_DEV_LIMIT then the Servos and RGB LEDS will animate according to light change. 
   If the statistical variation is below LDR_STD_DEV_LIMIT at the end of each counter cycle, then a random animation is triggered.
   Servo movements are synchonised (servo 1 moves fastest, then servo 2, then servo 3 moves slowest).
   RGB colours are shifted using HSV (hue value is changeD) in sync with accompanying servo.
   
   TO PROGRAM
   ------------------------------------------------------
   1. Download Arduino https://arduino.cc
   2. Connect USB-C cable
   3. Select Board > Arduino Micro
   4. Select Port > [your USB or COM port]
   5. Compile and upload
   6. If RESET LED flashes while searching for the Port during programming, press RESET button

   For additional help on getting started, including installation of drivers, see https://docs.arduino.cc/retired/getting-started-guides/ArduinoLeonardoMicro

   INTERAL ARDUINO LIBARIES
   ------------------------------------------------------
   Servo & EEPROM

   REQUIRED EXTERNAL LIBRARIES
   all available via Toos > Manage Libraries - search for the library name
   ------------------------------------------------------
   FastLED
   Author: Daniel Garcia
   https://github.com/FastLED/FastLED 
   Compiled with Version 3.4.0

   STATISTIC
   Author: Rob Tillaart
   https://github.com/RobTillaart/Statistic
   Compiled with Version 0.4.3
       
   MOVING AVERAGE LIBRARY
   Author: Alexandre Hiroyuki Yamauchi
   https://github.com/AlexandreHiroyuki/MovingAverage_ArduinoLibrary
   Compiled with 2.0.1
*/

#define ORIBOKIT_VERSION "1.0 mono"
#define OSC 1
#define __AVR_ATmega32U4__

// 
#if OSC == 1
  #include "oribokit.osc.h"
#endif 

#include <Servo.h>
#include <EEPROM.h>
#include "FastLED.h"
#include "Statistic.h"
#include "MovingAverage.h"

#include "oribokit.h"

uint8_t mode = MODE_ORIBOKIT;                             // mode for functionality

// SEE oribokit.h for #define variables 

// SERVO
Servo servo1;                                             // servo class instance for servo 1
int servoPosition1 = 0;                                   // variable to store the servo 1 position
int last_servoPosition1 = 0;                              // variable to store the servo 1 position
boolean servo_attached_1 = 0;                             // to track if the servo is attached, faster than reading the servo.attached property

// BLOOM COUNTER
uint8_t bloomCounter = 255;
uint8_t previousMode = mode;

// SERVO ANGLES
uint8_t minAngle1 = 0;                                    // minimum angle for servo1
uint8_t maxAngle1 = 179;                                  // maximum angle for servo1
int minLight  = 0;                                        // minimum value for the LDR (this is the LO value)
int maxLight  = 300;                                      // maximum value for the LDR (this is the HI value)

// BUTTONS
#define BUTTON_PIN_MIN    4                               // PD4 button pin to set the minimum LDR value
#define BUTTON_PIN_MAX    8                               // PB4 button pin to set the maximum LDR value
#define BUTTON_PIN_MODE   5                               // PC6 button pin to switch the mode
uint8_t maxPushed       = 0;                              // variable to store the state of the max button
uint8_t minPushed       = 0;                              // variable to store the state of the min button
uint8_t lastPushed      = 0;
uint8_t modePushed      = 0;                              // variable to store the state of the mode button
bool saveConfigFlag = 0;                                  // boolean (true/false) to check if we need to save
bool readConfigFlag = 1;                                  // boolean (true/false) to check if we need to save
bool switchConfigFlag = 0;

// LEDS
uint8_t hue         = 0;                                  // Hue value used in HSV colours, variable
CRGB leds_1[NUM_LEDS_1];                                  // CRGB object 1
#define LED_MODE_SERVO   6                                // servo mode LED pin
#define LED_MODE_SENSOR 12                                // sensor mode LED pin
#define MODE_SENSOR 0                                     // value for mode sensor 
#define MODE_SERVO 1                                      // value for mode servo 
uint8_t settingsMode = MODE_SENSOR;                       // variable to store settings mode

// SENSOR
int LDRreading = 0;                                       // integer variable to store the analog value

// SMOOTHING VALUES
int average1              = 0;                            // the average value for sensor 1
MovingAverage<unsigned> smoothing1(10, 0);                // the rolling value record object servo 1

// ANIMATION VALUES used for periodic random movement
int animation1              = 0;                          // the average animation value for servo 1
MovingAverage<unsigned> smoothanimation1(10, 0);          // the rolling animation value record object servo 1
int animation_min           = 0;                          // animation target in a range of minlight > maxlight
int animation_max           = 0;                          // acceleration towards the new target

// PERIODIC RANDOM MOVEMENT
Statistic LDR;                                            // LDR is the statistical object for the LDR sensor
int loopDelay           = 25;                             // delay for doing servo and sensor loop actions
double counterA         = 0;                              // counter for first timing loop
double counterB         = 0;                              // counter for second timing loop
int limitA              = 5000 / loopDelay;               // limit for the first timing loop
int limitB              = random(1, 10);                  // limit for the second timing loop
boolean LDR_low_change  = 0;                              // LDR low change variable
int timer_state         = 0;
int loopTimer           = 0;

// 0 = no messages
// 1 = data messages
// 2 = all messages printed to serial port

// -------------------------------------------------------------------------
// SETUP FUNCTION
// this function runs once, before the first loop
// -------------------------------------------------------------------------

void setup() {
  #if OSC == 1
    SLIPSerial.begin(115200);
    #if ARDUINO >= 100
      while (!Serial1)
        ; // Leonardo "feature"
    #endif
  #else
    Serial.begin(115200);                                     // start serial at baud rate 9600
  #endif
  LDR.clear();
  // set up the servos
  servo1.attach(SERVOPIN1);                               // attach the servo class instance to servoPin 1
  pinMode(LEDPIN, OUTPUT);
  pinMode(LED_MODE_SERVO, OUTPUT);
  pinMode(LED_MODE_SENSOR, OUTPUT);
  // set the mode led correctly from the start
  digitalWrite(LED_MODE_SERVO, HIGH);
  digitalWrite(LED_MODE_SENSOR, LOW);
  pinMode(LEDPIN, OUTPUT);
  // set up the buttons and sensorins
  pinMode(LDRPIN, INPUT);
  pinMode(BUTTON_PIN_MIN, INPUT);                           // MAX Push button as input
  pinMode(BUTTON_PIN_MAX, INPUT);                           // MAX Push button as input
  // set up the LED objects
  FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds_1, NUM_LEDS_1).setCorrection( TypicalLEDStrip );
}

/*
    -------------------------------------------------------------------------
    CONFIG FUNCTIONS
    saveconfig - write the values to eeprom for min and max light
    readconfig - read the values from eeprom for min and max light
    writeValue - function to do the writing to eeprom
    readValue  - funcion that does to the job of reading from eeprom
    --------------------------------------------------------------------------
*/

void saveConfig () {
  writeIntEEPROM(EEPROM_MINLIGHT, minLight);
  writeIntEEPROM(EEPROM_MAXLIGHT, maxLight);
  writeIntEEPROM(EEPROM_MIN1, minAngle1);
  writeIntEEPROM(EEPROM_MAX1, maxAngle1);
  writeIntEEPROM(EEPROM_LOOP, loopDelay);
}

void initConfig () {
  int eeprom_init_value = readIntEEPROM(EEPROM_INIT);
  if (eeprom_init_value == EEPROM_INIT_VALUE)
  {
    readConfig();
  } else {
    // initialise the config with default values
    // set the eeprom_init
    writeIntEEPROM(EEPROM_INIT, EEPROM_INIT_VALUE);
    saveConfig();
  }
}

void readConfig() {
  minLight  = readIntEEPROM(EEPROM_MINLIGHT);
  maxLight  = readIntEEPROM(EEPROM_MAXLIGHT);
  minAngle1 = (uint8_t)readIntEEPROM(EEPROM_MIN1);
  maxAngle1 = (uint8_t)readIntEEPROM(EEPROM_MAX1);
  loopDelay  = readIntEEPROM(EEPROM_LOOP);
  animation_min = minLight;
  animation_max = maxLight;
  if (DEBUG > 0)
  {
    Serial.print(" minLight ");
    Serial.print(minLight);
    Serial.print(" maxLight ");
    Serial.println(maxLight);
    Serial.print(" minAngle1 ");
    Serial.print(minAngle1);
    Serial.print(" maxAngle1 ");
    Serial.println(maxAngle1);
  }
  
  readConfigFlag = 0;
}

int readIntEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

void writeIntEEPROM(int address, int number)
{
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  if (DEBUG > 0) {
    Serial.print("EEPROM.write: addr: ");
    Serial.print(address, HEX);
    Serial.print("  val: ");
    Serial.print("0x");
    Serial.println(number, HEX);
  }
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}


void readSensors()
{
  LDRreading = analogRead(LDRPIN);                        // read from the sensor:
  LDR.add(LDRreading);                                    // add to the statistics object
}

void sensorAnimation()
{
  smoothing1.push(LDRreading);                            // add to the smoothing object
  average1 = smoothing1.get();
}


void randomAnimation()
{
  int new_target = lerp(animation_min, animation_max, counterB / limitB);
  smoothing1.push(new_target);                            // add to the smoothing object
  average1 = smoothing1.get();
}

int lerp(int min, int max, float fraction)
{
  return (max - min) * fraction + min;
}

int checkTimers(int state) {
  counterA++;                                             // increment the counter for the small cycle
  if (counterA > limitA) {
    counterB ++;                                          // increment the counter for the larger cycle
    counterA = 0;
    if (counterB > limitB)
    {
      state = !state; // flip the state
      // reset the counters and set a random number for the next cycle length
      limitB = random(1, 10);
      counterB = 0;
      randomSeed(LDRreading);
      animation_min = minLight + random(0, maxLight-minLight);
      randomSeed(LDRreading);
      animation_max = maxLight - random(0, maxLight-minLight);
      LDR.clear();
      #if OSC
        updateTimersOSC();
      #endif
    }
  }
  #if OSC
    updateCountersOSC();
  #endif
  return state;
}

void checkButtons()
{
  // ---------------------------
  // max (HI) button
  // --------------------------
  maxPushed = digitalRead(BUTTON_PIN_MAX);                  // read if button is pressed
  if (maxPushed) {                                          // set maximum LDR value
    digitalWrite(LEDPIN, LOW);                              // turn LED ON
    if (settingsMode == MODE_SERVO)
    {
      maxAngle1 ++;
      if (maxAngle1 > 179)
      {
        maxAngle1 = 179;
      }
      sendOSC("/ack/maxangle", maxAngle1);
      servoPosition1 = angleToServoPosition(maxAngle1);
    }
    if (settingsMode == MODE_SENSOR)
    {
      maxLight = average1;                                  // put current average reading into the max value
      lastPushed = 'max';
      if (DEBUG > 1) {
        Serial.print("--------------- SET MAX: ");          // output values for DEBUG information
        Serial.println(maxLight);                           // output values for DEBUG information
      }
      saveConfigFlag = true;
    }
  }
  // ---------------------------
  // min (LO) button
  // ---------------------------
  minPushed = digitalRead(BUTTON_PIN_MIN);                  // read if button is pressed
  if (minPushed) {                                          // if the button is pressed
    digitalWrite(LEDPIN, LOW);                              // turn LED ON// turn LED ON
    if (settingsMode == MODE_SENSOR)
    {
      maxAngle1 = average1;                                  // put current average reading into the max value
      lastPushed = 'min';
      if (DEBUG > 1) {
        Serial.print("--------------- SET MIN: ");          // output values for DEBUG information
        Serial.println(minLight);                           // output values for DEBUG information
      }
      saveConfigFlag = true;
    }
    if (settingsMode == MODE_SERVO)
    {
      maxAngle1 --;
      if (maxAngle1 < 2)
      {
        maxAngle1 = 1;
        sendOSC("/ack/maxAngle/LOWERLIMIT", maxAngle1, minAngle1);
      }
        
      sendOSC("/ack/maxAngle", maxAngle1, minAngle1);
      servoPosition1 = angleToServoPosition(maxAngle1);
    }
  }

  // MODE BUTTON SWITCHING
  modePushed = digitalRead(BUTTON_PIN_MODE);                // read if button is pressed
  if (modePushed) {             
    switchConfigFlag = true;                                // if the button is pressed
    sendOSC("/ack/button/mode", settingsMode);
  }

  if (!modePushed)
  {
    if (switchConfigFlag)
    {
      if (settingsMode == MODE_SENSOR)
      {
        settingsMode = MODE_SERVO;
        sendOSC("/ack/switchconfig/to/servo", settingsMode);
        digitalWrite(LED_MODE_SERVO, LOW);                 // turn LED ON// turn LED ON
        digitalWrite(LED_MODE_SENSOR, HIGH);                 // turn LED off
      } else 
      {
        settingsMode = MODE_SENSOR;
        sendOSC("/ack/switchconfig/to/sensor", settingsMode);
        digitalWrite(LED_MODE_SERVO, HIGH);                  // turn LED off
        digitalWrite(LED_MODE_SENSOR, LOW);                // turn LED ON// turn LED ON
        saveConfig();
        sendOSC("/ack/saved/data");
      }
      sendOSC("/ack/newconfig", settingsMode);
      switchConfigFlag = false;
    }
  }
  // turn the LED off if no buttons are pushed
  if (!minPushed && !maxPushed) {                           // if maxbutton not pushed and minbutton not pushed
    digitalWrite(LEDPIN, HIGH);                             // turn LED off
    if (settingsMode == MODE_SENSOR)
    {
      if (saveConfigFlag) {                                 // check if the save Config Flag was set
        saveConfig ();                                      // save the config
        readConfig ();                                      // read the config
        saveConfigFlag = false;                             // switch save config flag off
      }
    }
  }
}

boolean checkLDRChange()
{
  boolean low = 0;
  if (LDR.pop_stdev() < LDR_STD_DEV_LIMIT)
  {
    low = 1;
  }
  return low;
}

void updateLEDs (int value1)
{
  // -------------------------
  // 4. set the LED values
  // set the hsv values of each LED according to the servo positon averaages.
  // map and then constrain the hue value for each servo
  // update each LED element in each object
  // -------------------------
  // 1
  value1 = constrain(value1, minLight, maxLight);
  hue = map(value1, minLight, maxLight, HUE_LO, HUE_HI);
  hue = constrain(hue, HUE_LO, HUE_HI);
  int i;
  for (i=0; i<NUM_LEDS_1; i++)
  {
    leds_1[i] = CHSV(hue, SATURATION, BRIGHTNESS);
    leds_1[i] = CHSV(hue, SATURATION, BRIGHTNESS);
  }
  
  FastLED.show();
}

void manualLEDs (int value1)
{
  // -------------------------
  // 4. set the LED values
  // set the hsv values of each LED according to the servo positon averaages.
  // map and then constrain the hue value for each servo
  // update each LED element in each object
  // -------------------------
  // 1
  value1 = constrain(value1, 0, 180);
  hue = map(value1, 0, 180, HUE_LO, HUE_HI);
  hue = constrain(hue, HUE_LO, HUE_HI);
  int i;
  for (i=0; i<NUM_LEDS_1; i++)
  {
    leds_1[i] = CHSV(hue, SATURATION, BRIGHTNESS);
    leds_1[i] = CHSV(hue, SATURATION, BRIGHTNESS);
  }

  FastLED.show();
}

int angleToServoPosition(int angle)
{
  return abs(180-(SERVODIRECTION*angle))%180;
}

void updateServos(int value1)
{
  // --------------------------
  // 3. update the servo position values
  // position the servo based on the current LDR average reading
  // mapped between the min and max LDR settings
  // mapped to the min and max angle settings
  // --------------------------
  
  value1 = constrain(value1, minLight, maxLight);
  servoPosition1 = (int)map(value1, minLight, maxLight, minAngle1, maxAngle1);
  servoPosition1 = constrain(servoPosition1, minAngle1, maxAngle1);
  servoPosition1 = angleToServoPosition(servoPosition1);

  // then position them
  positionServos();
}

void bloomServos(int value1)
{
  // --------------------------
  // 3. update the servo position values
  // sweep from max to min value
  // --------------------------

  servoPosition1 = (int)map(value1, 0, 255, minAngle1, maxAngle1);
  servoPosition1 = constrain(servoPosition1, minAngle1, maxAngle1);
  servoPosition1 = abs(180-(SERVODIRECTION*servoPosition1))%180;
  
  // then position them
  positionServos();
}

void positionServos() {
  int servopos;
  // servo 1
  if (servoPosition1 != last_servoPosition1)                // this runs once, to see if the servos new position has been registere
  { 
    servo1.attach(SERVOPIN1);                               // attach the servo class instance to servoPin 1
    servo_attached_1 = 1;
    servo1.write(servoPosition1);
    last_servoPosition1 = servoPosition1;
  } else {
    if (servo_attached_1)
    {
      servopos = servo1.read();
      //sendOSC("/ack/servopos/1", servopos);
      if (servopos == servoPosition1)                     // servo has reached its destination
      {   
        servo_attached_1 = 0;             
        servo1.detach();                                  // we can detach
        //sendOSC("/ack/detach", 1);
      }
    }
  }
}

void printDebug ()
{
  if (DEBUG > 0) {

    Serial.print("\tC-A ");
    Serial.print(counterA);
    Serial.print("\t C-B ");
    Serial.print(counterB);
    Serial.print("\t LDR ");
    Serial.print(LDRreading);
    Serial.print("\t");
    if (DEBUG > 1) {
      //      Serial.print(minLight);
      //      Serial.print("\t");
      //      Serial.print(maxLight);
      //      Serial.print("\t");
      //      Serial.print(hue);
      //      Serial.print("\t");
      //      Serial.print(longTimeNowA);
      //      Serial.print("\t");
      //      Serial.print(longTimeNowB);
      //      Serial.print("\t");
      //      Serial.print(longReadingVariance);
      Serial.print("\t SDEV ");
      Serial.print(LDR.pop_stdev());
      
    }
    Serial.print("\t S1 ");
    Serial.print(servoPosition1);
    Serial.print("\t AMIN ");
    Serial.print(animation_min);
    Serial.print("\t AMAX ");
    Serial.print(animation_max);
    Serial.print("\t A1 ");
    Serial.print(average1);
    Serial.print("\t LC ");
    Serial.print(LDR_low_change);
    Serial.print("\t T ");
    Serial.print(timer_state);
    Serial.println();
  }
}


// ------------------------------------------------------------------------- 
// ORIBOKIT MODE
// this function repeats forever, as fast as the processor can go!
// 1. read the sensor and average out the readings
//    averaging smooths out spikes in the readings
//    making the servo movement less jittery
//    try changing the number of readings
// 2. allow for button presses
//    pressing the min or max button allows setting of the
//    state that determines the LO or HI position of the servo
//    at that light level
// 3. position the servos
// --------------------------------------------------------------------------


void mode_oribokit ()
{
  // 0. read config from eeprom
  if (readConfigFlag) {
    initConfig();
    readConfig();
  }

  // 1. check for button presses
  checkButtons();

  if (settingsMode == MODE_SENSOR)
  {
    // 1. read get average sensor values average sensor values & process animation value
    readSensors();
    // 2. always poll LDR low change
    LDR_low_change = checkLDRChange();

    // 3. update the smoothing arrays with values from either random or from sensor data
    if (LDR_low_change)
    {
      randomAnimation();
    } else {
      sensorAnimation();
    }
    
    // 5. check the timers and statistics object
    // timer_state flips if both counters exceed limits
    timer_state = checkTimers(timer_state);

    // 6. use a different set of values based on LDR_low_change
    updateLEDs(average1);
    updateServos(average1);
  }

   // 7. position the servo at max angle
  if (settingsMode == MODE_SERVO)
  {
    manualLEDs(servoPosition1);
    positionServos();
  }
  
}

void mode_user_control()
{
  manualLEDs(servoPosition1);
  positionServos();
}

void bloom()
{
  bloomCounter --;
  sendOSC("/counters/bloom", bloomCounter);
  bloomServos(bloomCounter);
  manualLEDs(bloomCounter);
  if (bloomCounter == 0)
  {
    mode = previousMode;
    sendOSC("/ack/mode", mode);  
  }
}

void loop() {
  // check for OSC input
  #if OSC == 1
    updateOSC();
    if (SLIPSerial.available() > 0)
      rxOSC();
  #endif


  loopTimer ++;
  if (loopTimer > loopDelay)
  {
    loopTimer = 0;
    switch (mode)
    {
        case MODE_ORIBOKIT:
          mode_oribokit();
          break;
        case MODE_USER_CONTROL:
          mode_user_control();
          break;
        case MODE_BLOOM:
          bloom();
          break;
    }
  }
  
  

  printDebug();
  // LOOP DELAY
  // changing the delay changes how many miliseconds pass between each loop
  // 1000 is one second, to make your oribokit response slow, set this to a high number
  // delay(loopDelay);
}