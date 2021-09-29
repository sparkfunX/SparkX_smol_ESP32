// smôl ESP32 Production Test
//
// Written by Paul Clark (PaulZC)
// September 27th, 2021
//
// Select SparkFun ESP32 Thing as the board
//
// Make sure you have the correct drivers installed for the Silicon Labs
// CP210x USB to UART Bridge otherwise the automatic upload may fail
// https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
//
// This is the code we use to test the board during production:
//   The LED will cycle through Red-Green-Blue, changing every second
//   The SPIFFS flash memory is tested - the memory is formatted and a file is created
//   The WiFi is checked - the board will report how many networks are available
//   The GPIO pins used by smôl are read periodically
//   Serial messages are generated and are read by the production test jig
//   The jig will toggle the GPIO pins and look for the correct response in the serial messages
//   The serial messages are sent at 115200 baud:
//     "SPIFFS:PASS" indicates the flash memory test was successful
//     "SPIFFS:FAIL" indicates the flash memory test failed
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

// SPIFFS
#include "FS.h"
#include "SPIFFS.h"
/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

//WiFi
#include <WiFi.h>

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
  toggler.attach(1.0, toggle); // Toggle the LED every second

  //Set all the GPIO pins to INPUT
  for (uint8_t i = 0; i < numPins; i++)
    pinMode(thePins[i], INPUT);

  //Test SPIFFS
  bool spiffsResult = spiffsTest();
  Serial.print(F("SPIFFS:"));
  if (spiffsResult) Serial.println(F("PASS"));
  else  Serial.println(F("FAIL"));

  //Test WiFi - report how many WiFi networks can be seen
  byte numNetworks = WiFi.scanNetworks();
  Serial.print(F("WIFI:"));
  Serial.println(numNetworks);
}

void loop()
{
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

  delay(100);
}

bool spiffsTest()
{
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    return (false);
  
  return(writeFile(SPIFFS, "/hello.txt", "Hello"));
}

bool writeFile(fs::FS &fs, const char * path, const char * message)
{
  bool result = false;
  File file = fs.open(path, FILE_WRITE);
  if(!file)
    return (result);
    
  result = file.print(message);
  file.close();
  return(result);
}
