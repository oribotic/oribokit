#include "FastLED.h"

// LED VARIABLES 
#define LED_PIN_1   10                                    // PWM PB6 D10 pin connected to LED data 1
#define LED_PIN_2   5                                     // PWM PC6 D5  pin connected to LED data 2
#define LED_PIN_3   13                                    // PWM PC7 D13 pin connected to LED data 3
#define NUM_LEDS_1  3                                     // LEDs connected to each pin
#define NUM_LEDS_2  3                                     // LEDs connected to each pin
#define NUM_LEDS_3  3                                     // LEDs connected to each pin
#define LED_TYPE    WS2812                                // chip type of LED
#define COLOR_ORDER GRB                                   // colour order of LEDs
#define HUE_HI      180                                   // in HSV space the highest hue value mapped to average sensor value
#define HUE_LO      0                                     // in HSV space the lowest hue value mapped to average sensor value
int hue             = 0;                                  // Hue value used in HSV colours, variable
CRGB leds_1[NUM_LEDS_1];                                  // CRGB object 1 needed for FastLED library
CRGB leds_2[NUM_LEDS_2];                                  // CRGB object 2 needed for FastLED library
CRGB leds_3[NUM_LEDS_2];                                  // CRGB object 3 needed for FastLED library

// COLOUR VARIABLES
int R = 0;
int G = 0;
int B = 0;

void setup() {
  // set up the three FASTLED channels
  // this syntax is complex, and honestly there is no reason to understand it unless we are using different LED strips
  FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds_1, NUM_LEDS_1).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds_2, NUM_LEDS_2).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_3, COLOR_ORDER>(leds_3, NUM_LEDS_3).setCorrection( TypicalLEDStrip );
}

void loop() {

  // let's make a random RGB color
  R = random(255);
  G = random(255);
  B = random(255);

  // 1.  loop through all of the LEDs connected to panel RGB1
  int i;
  for (i=0; i<NUM_LEDS_1; i++)
  {
    // set hue, SATURATION, BRIGHTNESS
    leds_1[i] = CRGB(R, G, B);
  }
  
  // 2.  loop through all of the LEDs connected to panel RGB2
  for (i=0; i<NUM_LEDS_2; i++)
  {
    // set hue, SATURATION, BRIGHTNESS
    leds_2[i] = CRGB(R, G, B);
  }
  // 3.  loop through all of the LEDs connected to panel RGB3
  for (i=0; i<NUM_LEDS_3; i++)
  {
    // set hue, SATURATION, BRIGHTNESS
    leds_3[i] = CRGB(R, G, B);
  }
  // lastly let's update the LEDs
  delay(500);
  FastLED.show();

  Serial.print(R);    // print the R value
  Serial.print("\t"); // print a tab space
  Serial.print(G);    // print the G value
  Serial.print("\t"); // print a tab space
  Serial.println(B);  // print the B value

}
