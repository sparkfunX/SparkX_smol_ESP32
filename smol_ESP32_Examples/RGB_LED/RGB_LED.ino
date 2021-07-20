// smôl ESP32 LED Demo
//
// Select SparkFun ESP32 Thing as the board
//
// Make sure you have the correct drivers installed for the Silicon Labs
// CP210x USB to UART Bridge otherwise the automatic upload may fail
// https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

#include <FastLED.h> // http://librarymanager/All#FastLED

#define LED_PIN     25 //GPIO25 on smôl ESP32 is connected to WS2812 LED
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS    1

#define BRIGHTNESS  50

CRGB leds[NUM_LEDS];

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() {
  delay(30); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
}

void loop()
{
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();
  
  EVERY_N_MILLISECONDS( 10 ) { gHue++; } // cycle the "base color" through the rainbow
}
