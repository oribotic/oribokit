// ORIBOKIT DIGITAL WRITE TEST

// LED
int LEDPin          = 17;                // PB0 info LED pin

// state variable
bool state = false;                      // a boolean can only be true or false

void setup() {
  Serial.begin(115200);                  // Start the Serial port for debugging
  pinMode(LEDPin, OUTPUT);               // LED PIN as a digital output
}

void loop() {
  if (state==false)                      // if the value of state is false
  {
     digitalWrite(LEDPin, LOW);          // write the LEDPin LOW
     state = true;                       // set state to true
  } else {                               // otherwise the value of state is true
    digitalWrite(LEDPin, HIGH);          // write the LEDPin HIGH
     state = false;                      // set state to false
  }
  delay(1000);                           // delay for 1000 milliseconds, 1 second
}


// SEE IF YOU CAN UNDERSTAND WHY THIS WORKS?
// void loop() {
//   digitalWrite(LEDPin, state);              // write the LEDPin to the value of state. boolean false = LOW, boolean true = HIGH
//   state = !state;                           // set the value of state to the opposite of state
//   delay(1000);                              // delay for 1000 milliseconds, 1 second
// }

