// smôl ESP32 Production Test
//
// Written by Paul Clark (PaulZC)
// October 11th, 2021
//
// Select SparkFun ESP32 Thing as the board
//
// Make sure you have the correct drivers installed for the Silicon Labs
// CP210x USB to UART Bridge otherwise the automatic upload may fail
// https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
//
// This is the code we use to test the board during production:
//   The WiFi is checked - the board will report how many networks are available
//   The LED will cycle through Red-Green-Blue, changing every second
//   The GPIO pins used by smôl are read periodically
//   Serial messages are generated and are read by the production test jig
//   The jig will toggle the GPIO pins and look for the correct response in the serial messages
//   The serial messages are sent at 115200 baud:
//     "WIFI:n" indicates how many WiFi networks have been found
//     "GPIO:nnnnnnnnnn" indicates the state of the GPIO pins
//       n will be "0" or "1" showing whether the pin is LOW or HIGH
//       The pin order is:
//         SCLK (GPIO18)
//         COPI (GPIO23)
//         CIPO (GPIO19)
//         CS0 (GPIO5)
//         CS1 (GPIO14)
//         CS2 (GPIO13)
//         GPIO0 (GPIO27)
//         GPIO1 (GPIO26)
//         SDA (GPIO21)
//         SCL (GPIO22)

// Drive the LED using FastLED
#include <FastLED.h> // http://librarymanager/All#FastLED
#define LED_PIN     25 //GPIO25 on smôl ESP32 is connected to WS2812 LED
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS    1
#define BRIGHTNESS  50
CRGB leds[NUM_LEDS];
unsigned int RGBarray[] = {CRGB::Red, CRGB::Green, CRGB::Blue};
volatile uint8_t rgbStep = 0;

// The GPIO pins
const uint8_t numPins = 10;
const uint8_t thePins[numPins] = {18, 23, 19, 5, 14, 13, 27, 26, 21, 22 };

//WiFi
#include <WiFi.h>

//Globals
byte numNetworks = 0;

//Ticker for LED updates
#include <Ticker.h>
Ticker toggler;
void toggle()
{
  leds[0] = RGBarray[rgbStep]; // Cycle through RGB
  FastLED.show();
  rgbStep = (rgbStep + 1) % 3;
}

void setup() {
  delay(1000);

  Serial.begin(115200);
  Serial.println(F("SparkFun smôl ESP32 Production Test"));

  //Set up the LED
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  leds[0] = CRGB::White;
  FastLED.show();

  //Set all the GPIO pins to INPUT
  for (uint8_t i = 0; i < numPins; i++)
    pinMode(thePins[i], INPUT);

  //Test WiFi - report how many WiFi networks can be seen
  numNetworks = WiFi.scanNetworks();

  //Toggle the LED color now that SPIFFS and WiFi tests are complete
  toggler.attach(1.0, toggle); // Toggle the LED every second
}

void loop()
{
  Serial.print(F("WIFI:"));
  Serial.println(numNetworks);

  //Read the GPIO pins
  Serial.print(F("GPIO:"));
  for (uint8_t i = 0; i < numPins; i++)
  {
    if (digitalRead(thePins[i]) == HIGH)
      Serial.print(F("1"));
    else
      Serial.print(F("0"));
  }
  Serial.println();

  delay(50);
}
