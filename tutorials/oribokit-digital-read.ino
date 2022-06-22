// ORIBOKIT DIGITAL IO TEST

// BUTTONS
int minButtonPin    = 4;                                  // PD4 button pin to set the minimum LDR value
int maxButtonPin    = 8;                                  // PB4 button pin to set the maximum LDR value
int maxPushed       = 0;                                  // variable to store the state of the max button
int minPushed       = 0;                                  // variable to store the state of the min button

void setup() {
  Serial.begin(115200);                                   // Start the Serial port for debugging
  pinMode(minButtonPin, INPUT);                           // MAX Push button as input
  pinMode(maxButtonPin, INPUT);                           // MAX Push button as input
}

void loop() {
  // max (HI) button
  maxPushed = digitalRead(maxButtonPin);                  // read if button is pressed
  // min (LO) button
  minPushed = digitalRead(minButtonPin);                  // read if button is pressed
  Serial.print("MIN BUTTON is ");
  Serial.print(minPushed);
  Serial.print(" MAX BUTTON is ");
  Serial.println(maxPushed);
  delay (250);                                            // a short delay to slow the serial output
}

