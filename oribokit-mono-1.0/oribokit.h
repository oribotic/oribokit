// here are all the global variables, these can be accessed by the loop and setup functions
#define DEBUG             0                               // DEBUG level
#define MODE_ORIBOKIT     0                               // user mode setting
#define MODE_USER_CONTROL 1                               // user mode setting for user control
#define MODE_BLOOM        2                               // user mode setting for user control

#define SERVOPIN1        9                                // PWM PB5 D9  pin connected to servo 1
#define SERVOPIN2        6                                // PWM PD7 D6  pin connected to servo 2
#define SERVOPIN3        12                               // PWM PD6 D12 pin connected to servo 3
#define SERVODIRECTION   1                               // set to -1 or 1, to flip the servo direction

// eeprom address settings
#define EEPROM_MINLIGHT 0
#define EEPROM_MAXLIGHT 5
#define EEPROM_MIN1 10
#define EEPROM_MIN2 15
#define EEPROM_MIN3 20
#define EEPROM_MAX1 25
#define EEPROM_MAX2 30
#define EEPROM_MAX3 35
#define EEPROM_LOOP 40
#define EEPROM_INIT 45
#define EEPROM_INIT_VALUE 16                              // same as release value

// LEDS
#define LEDPIN      17                                    // PB0 info LED pin
#define LED_PIN_1   10                                    // PWM PB6 D10 pin connected to LED data 1
#define LED_PIN_2   5                                     // PWM PC6 D5  pin connected to LED data 2
#define LED_PIN_3   13                                    // PWM PC7 D13 pin connected to LED data 3
#define NUM_LEDS_1  3                                     // LEDs connected to each pin
#define NUM_LEDS_2  3                                     // LEDs connected to each pin
#define NUM_LEDS_3  3                                     // LEDs connected to each pin
#define BRIGHTNESS  255                                   // Brightness used in HSV colours, static
#define SATURATION  255                                   // Saturation used in HSV colours, static
#define LED_TYPE    WS2812                                // chip type of LED
#define COLOR_ORDER GRB                                   // colour order of LEDs
#define HUE_HI      170                                   // in HSV space the highest hue value mapped to average sensor value
#define HUE_LO      48                                    // in HSV space the lowest hue value mapped to average sensor value

// LDR
#define LDRPIN A0                                         // PF7 pin connected to the Light Dependent Resistor
#define LDR_STD_DEV_LIMIT 2                               // Standard Deviation limit low change of LDR
