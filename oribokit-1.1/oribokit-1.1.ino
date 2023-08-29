/*
 * ORIBOKIT - //oribokit.com
 * robotic origami starter kit
 * "firmware" for oribokit board 1.1
 * 3 blossom kit
 * 1 LDR
 * Synchonised servo movements (servo 1 moves fastest, then servo 2, then servo 3 moves slowest)
 * 
 * ********* TO PROGRAM **********
 * Connect to USB port, with jumper removed (see the bottom of the board. 
 * There's black jumper (it connects two pins) on G and BT1) REMOVE IT
 * Press the Reset button, the LED will keep flashing
 * This is known as the maple (after maple labs) bootloader mode
 * If this LED is not flashing when Arduino gets to the programming point, it will give a DFU Error
 * Just press RESET and try again if that happens
 * NOTE: Go to Preferences > Settings 
 * and check the two boxes next to "Show Vebose Output during" compiliation and upload
 * ********* TO RUN **************
 * Replace the jumper on pins G AND BT1 (last two pins on the header before the LDR)
 * Press RESET, the LED will flash a few times fast, and then switch off. 
 * LED will turn on when you press LO or HI
 */

// TODO
// @EEPROM use for max and min LDR values

#include <Servo.h>
#include <EEPROM.h>

// here are all the global variables, these can be accessed by the loop and setup functions

// SERVOS
Servo servo1;                                              // servo class instance for servo 1
Servo servo2;                                              // servo class instance for servo 2
Servo servo3;                                              // servo class instance for servo 3
int servoPin1 = PA10;                                      // PWM pin connected to servo 1
int servoPin2 = PA9;                                       // PWM pin connected to servo 2
int servoPin3 = PA8;                                       // PWM pin connected to servo 3
int servoPosition = 90;                                    // variable to store the servo position

// servo                                                   
int minAngle = 100;                                        // minimum angle for the servo 
int maxAngle = 180;                                        // maximum angle for the servo
uint16 minLight = 2000;                                    // minimum value for the LDR (this is the LO value)
uint16 maxLight = 6000;                                    // maximum angle for the LDR (this is the HI value)

// BUTTONS                                                 
int minButtonPin    = PB11;                                // button pin to set the minimum LDR value
int maxButtonPin    = PB10;                                // button pin to set the maximum LDR value
int maxPushed       = 0;                                   // variable to store the state of the max button
int minPushed       = 0;                                   // variable to store the state of the min button
bool saveConfigFlag = 0;                                   // boolean (true/false) to check if we need to save
bool readConfigFlag = 1;                                   // boolean (true/false) to check if we need to save

// SENSOR                           
int LDRPin = PA5;                                          // the pin connected to the Light Dependent Resistor

// SMOOTHING FOR SERVO 1
int readIndex1    = 0;                                     // the index of the current reading
const int numReadings1  = 10;                              // the number of readings to average over, more = slower
int readings1[numReadings1];                               // an array to store up to {numReadings} readings
int total1       = 0;                                      // the running sum total
int average1     = 0;                                      // the average

// SMOOTHING FOR SERVO 2
int readIndex2    = 0;                                     // the index of the current reading
const int numReadings2  = 20;                              // the number of readings to average over, more = slower
int readings2[numReadings2];                               // an array to store up to {numReadings} readings
int total2       = 0;                                      // the running sum total
int average2     = 0;                                      // the average

// SMOOTHING FOR SERVO 3
int readIndex3    = 0;                                     // the index of the current reading
const int numReadings3  = 40;                              // the number of readings to average over, more = slower
int readings3[numReadings3];                               // an array to store up to {numReadings} readings
int total3       = 0;                                      // the running sum total
int average3     = 0;                                      // the average

// LED                                                  
int LEDPin = PC13;                                         
int debug = 2;                                             // 0 = no messages, 1 = data messages, 2 = all messages printed to serial port

// -------------------------------------------------------------------------
// SETUP FUNCTION                                          
// this function runs once, before the first loop
// -------------------------------------------------------------------------
                                                           
void setup() {                                             
  Serial.begin(9600);                                      // start serial at baud rate 9600
  // set up the servos                                     
  servo1.attach(servoPin1);                                // attach the servo class instance to servoPin 1
  servo2.attach(servoPin2);                                // attach the servo class instance to servoPin 2
  servo3.attach(servoPin3);                                // attach the servo class instance to servoPin 3
  pinMode(LEDPin, OUTPUT);                                 
  // set up the buttons and sensorins                      
  pinMode(LDRPin, INPUT);                                  
  pinMode(minButtonPin, INPUT);                            // MAX Push button as input
  pinMode(maxButtonPin, INPUT);                            // MAX Push button as input
  // configure the eeprom 
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageSize  = 0x400;
}

/* 
 *  -------------------------------------------------------------------------
 *  CONFIG FUNCTIONS
 *  saveconfig - write the values to eeprom for min and max light
 *  readconfig - read the values from eeprom for min and max light
 *  writeValue - function to do the writing to eeprom
 *  readValue  - funcion that does to the job of reading from eeprom
// --------------------------------------------------------------------------
*/

void saveConfig () {
  writeValue (0x10, minLight);
  writeValue (0x11, maxLight);
}

void readConfig() {
  minLight = readValue(0x10);
  maxLight = readValue(0x11);
  if (debug > 0) {
    Serial.print(" minLight");
    Serial.print(minLight);
    Serial.print(" maxLight");
    Serial.println(maxLight);
  }
  readConfigFlag = 0;
}

void writeValue (uint16 addr, uint16 val) {
  uint16 Status;
  Status = EEPROM.write(addr, val);
  if (debug > 0) {
    Serial.print("EEPROM.write: ");
    Serial.print(addr, HEX);
    Serial.print("0x");
    Serial.print(val, HEX);
    Serial.print(" : Status : ");
    Serial.println(Status, HEX);
  }
}

uint16 readValue (uint16 addr) {
  uint16 data;
  uint16 Status;
  Status = EEPROM.read(addr, &data);
  if (debug > 0) {
    Serial.print("EEPROM.read: ");
    Serial.print(addr, HEX);
    Serial.print("0x");
    Serial.print(data, HEX);
    Serial.print(" : Status : ");
    Serial.println(Status, HEX);
  }
  return data;
}


// -------------------------------------------------------------------------
// LOOP FUNCTION
// this function repeat forever, as fast as the processor can go!
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


void loop() {
  // --------------------------
  // 0. read config from eeprom
  // --------------------------
  if (readConfigFlag) {
    readConfig();
  }
  // --------------------------
  // 1. read and average sensors
  // --------------------------
  // smoothing 1
	total1 = total1 - readings1[readIndex1];                // subtract the old value at this index:
  readings1[readIndex1] = analogRead(LDRPin);             // read from the sensor:
  total1 = total1 + readings1[readIndex1];                // add the reading to the total:
  readIndex1 ++;                                          // advance to the next position in the array:
  if (readIndex1 >= numReadings1) {                       // if we're at the end of the array.
    readIndex1 = 0;                                       // reset the readIndex back to zero
  }
  average1 = total1 / numReadings1;                       // calculate the average. Average = sum/number
  // smoothing 2
  total2 = total2 - readings2[readIndex2];                // subtract the old value at this index:
  readings2[readIndex2] = analogRead(LDRPin);             // read from the sensor:
  total2 = total2 + readings2[readIndex2];                // add the reading to the total:
  readIndex2 ++;                                          // advance to the next position in the array:
  if (readIndex2 >= numReadings2) {                       // if we're at the end of the array.
    readIndex2 = 0;                                       // reset the readIndex back to zero
  }
  average2 = total2 / numReadings2;                       // calculate the average. Average = sum/number
  // smoothing 3
  total3 = total3 - readings3[readIndex3];                // subtract the old value at this index:
  readings3[readIndex3] = analogRead(LDRPin);             // read from the sensor:
  total3 = total3 + readings3[readIndex3];                // add the reading to the total:
  readIndex3 ++;                                          // advance to the next position in the array:
  if (readIndex3 >= numReadings3) {                       // if we're at the end of the array.
    readIndex3 = 0;                                       // reset the readIndex back to zero
  }
  average3 = total3 / numReadings3;                       // calculate the average. Average = sum/number
  
	// --------------------------
	// 2. allow for button presses
	// max (HI) button
  // --------------------------
  maxPushed = digitalRead(maxButtonPin);                  // read if button is pressed
  if (maxPushed) {                                        // set maximum LDR value
    digitalWrite(LEDPin, HIGH);                           // turn LED ON
    maxLight = average3;                                  // put current average reading into the max value 
    if (debug > 1) {
      Serial.print("--------------- SET MAX: ");          // output values for debug information
      Serial.println(maxLight);                           // output values for debug information
    }
    saveConfigFlag = true;
  }
  
  // min (LO) button
  minPushed = digitalRead(minButtonPin);                  // read if button is pressed
  if (minPushed) {                                        // if the button is pressed 
    digitalWrite(LEDPin, HIGH);                           // turn LED ON// turn LED ON
    minLight = average3;                                  // put current average reading into the max value
    if (debug > 1) {
      Serial.print("--------------- SET MIN: ");          // output values for debug information
      Serial.println(minLight);                           // output values for debug information
    }
    saveConfigFlag = true;
  }
  
  // turn the LED off if no buttons are pushed
  if (!minPushed && !maxPushed) {                         // if maxbutton not pushed and minbutton not pushed
    digitalWrite(LEDPin, LOW);                            // turn LED off
    if (saveConfigFlag) {                                 // check if the save Config Flag was set
      saveConfig ();                                      // save the config
      readConfig ();                                      // read the config
      saveConfigFlag = false;                             // switch save config flag off
    }
  }
  
  // --------------------------
	// 3. position the servos
  // position the servo based on the current LDR average reading
	// mapped between the min and max LDR settings
	// mapped to the min and max angle settings
  // --------------------------
  servoPosition = map(average1, minLight, maxLight, minAngle, maxAngle);
  servoPosition = constrain(servoPosition, minAngle, maxAngle);
  servo1.write(servoPosition); 

  servoPosition = map(average2, minLight, maxLight, minAngle, maxAngle);
  servoPosition = constrain(servoPosition, minAngle, maxAngle);
	servo2.write(servoPosition); 

  servoPosition = map(average3, minLight, maxLight, minAngle, maxAngle);
  servoPosition = constrain(servoPosition, minAngle, maxAngle);
	servo3.write(servoPosition);
  
  if (debug > 1) {
    Serial.print(servoPosition);  
    Serial.print("\t");  
    Serial.print(average1); 
    Serial.print("\t");  
    Serial.print(average2); 
    Serial.print("\t");  
    Serial.println(average3); 
  }

  // changing the delay changes how many miliseconds pass between each loop
  // 1000 is one second, to make your oribokit response slow, set this high
  delay(30);

}
