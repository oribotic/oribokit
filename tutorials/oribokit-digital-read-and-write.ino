// ORIBOKIT DIGITAL IO TEST

// BUTTONS
int minButtonPin    = 4;                                  // PD4 button pin to set the minimum LDR value
int maxButtonPin    = 8;                                  // PB4 button pin to set the maximum LDR value
int maxPushed       = 0;                                  // variable to store the state of the max button
int minPushed       = 0;                                  // variable to store the state of the min button

// LED
int LEDPin          = 17;                                 // PB0 info LED pin

void setup() {
  Serial.begin(115200);                                   // Start the Serial port for debugging
  pinMode(minButtonPin, INPUT);                           // MAX Push button as input
  pinMode(maxButtonPin, INPUT);                           // MAX Push button as input
  pinMode(LEDPin, OUTPUT);                                // LED PIN as a digital output
}

void loop() {
  // max (HI) button
  maxPushed = digitalRead(maxButtonPin);                  // read if button is pressed
  if (maxPushed==HIGH) {                                  // the max button must HIGH
    digitalWrite(LEDPin, LOW);                            // switch LED PIN LOW TO turn LED on
  }
  // min (LO) button
  minPushed = digitalRead(minButtonPin);                  // read if button is pressed
  if (minPushed==HIGH) {                                  // if the button is pressed
    digitalWrite(LEDPin, LOW);                            // switch LED PIN LOW TO turn LED on
  }

  // turn the LED off if no buttons are pushed
  if (!minPushed && !maxPushed) {                         // if maxbutton not pushed and minbutton not pushed
    digitalWrite(LEDPin, HIGH);                           // switch LED PIN LOW TO turn LED off  
  }
  
  // debug information
  Serial.print("MIN BUTTON is ");
  Serial.print(minPushed);
  Serial.print(" MAX BUTTON is ");
  Serial.println(maxPushed);
  delay (500);
}

