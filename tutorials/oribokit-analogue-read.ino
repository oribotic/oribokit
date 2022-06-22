// SENSOR
int LDRPin = A0;                            // PF7 pin connected to the Light Dependent Resistor
int LDRreading = 0;                         // integer variable to store the analog value

void setup() {
  Serial.begin(115200);                     // Start the Serial port for debugging
  pinMode(LDRPin, INPUT);                   // set the LDR pin as an input
}

void loop() {
  // put your main code here, to run repeatedly:
  LDRreading = analogRead(LDRPin);

  // let's calculate the resistor value of the LDR using Ohms Law
  // https://en.wikipedia.org/wiki/Voltage_divider
  // Vout = R2 / (R1 + R2) * Vin
  // we need to use float values to calculate the value of R1
  float R1;
  float Vin = 5;                                  // The value of voltage for the analogue read
  float R2 = 10000;                               // 10K resistor
  float Vout = float(LDRreading) / 1024 * 5;      // The value for LDRreading is mapped to a 10 bit unsigned integer (range 0 - 1024)
  R1 = (R2*Vin/Vout)-R2;                          // To solve for R1 = R2 * (Vin/Vout -1)

  // print our results to the serial port
  Serial.print(LDRreading);
  Serial.print(" / 1024 is ");
  Serial.print(Vout);                 
  Serial.print("V therefore R1 is ");
  Serial.print(R1);
  Serial.println(" ohms");
  delay (250);
}
