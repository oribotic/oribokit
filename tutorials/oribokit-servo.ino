#include <Servo.h>

// SERVOS VARIABLES
Servo servo1;                                             // servo class instance for servo 1
Servo servo2;                                             // servo class instance for servo 2
Servo servo3;                                             // servo class instance for servo 3
int servoPin1      = 9;                                   // PWM PB5 D9  pin connected to servo 1
int servoPin2      = 6;                                   // PWM PD7 D6  pin connected to servo 2
int servoPin3      = 12;                                  // PWM PD6 D12 pin connected to servo 3
int servoPosition1 = 0;                                  // variable to store the servo 1 position
int servoPosition2 = 0;                                  // variable to store the servo 2 position
int servoPosition3 = 0;                                  // variable to store the servo 3 position

void setup() {
  Serial.begin(115200);                                     // start serial at baud rate 9600
  // attach the servos
  servo1.attach(servoPin1);                               // attach the servo class instance to servoPin 1
  servo2.attach(servoPin2);                               // attach the servo class instance to servoPin 2
  servo3.attach(servoPin3);                               // attach the servo class instance to servoPin 3
}

void loop() {
  
  servoPosition1 ++;
  servoPosition2 ++;
  servoPosition3 ++;

  if (servoPosition1 > 180)
  {
    servoPosition1 = 0;
  }
  if (servoPosition2 > 180)
  {
    servoPosition2 = 0;
  }
    if (servoPosition3 > 180)
  {
    servoPosition3 = 0;
  }

  delay(100);
  servo1.write(servoPosition1);
  servo2.write(servoPosition2);
  servo3.write(servoPosition3);

  Serial.print(servoPosition1);
  Serial.print("\t");
  Serial.print(servoPosition2);
  Serial.print("\t");
  Serial.print(servoPosition3);

}
