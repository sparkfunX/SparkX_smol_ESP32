/*

 smôl ESP32 WiFi Demo

 Select SparkFun ESP32 Thing as the board

 Make sure you have the correct drivers installed for the Silicon Labs
 CP210x USB to UART Bridge otherwise the automatic upload may fail
 https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

 Based on: WiFi Web Server LED Blink

 A simple web server that lets you set the color of an LED via the web.
 This sketch will print the IP address of your smôl ESP32 (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to set the color of the LED on pin 25.

 If the IP address of your smôl ESP32 is yourAddress:
 http://yourAddress/R sets the LED to Red
 http://yourAddress/G sets the LED to Green
 http://yourAddress/B sets the LED to Blue

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * smôl ESP32 which has a WS2812 LED attached to pin 25

 created for arduino 25 Nov 2012
 by Tom Igoe

 ported for sparkfun esp32 
 31.01.2017 by Jan Hendrik Berlin

 updated for SparkFun smôl ESP32
 July 20th 2021 by Paul Clark

*/

#include <WiFi.h>

const char* ssid     = "yourssid";
const char* password = "yourpasswd";

WiFiServer server(80);


#include <FastLED.h> // http://librarymanager/All#FastLED

#define LED_PIN     25 //GPIO25 on smôl ESP32 is connected to WS2812 LED
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS    1

#define BRIGHTNESS  50

CRGB leds[NUM_LEDS];


void setup()
{
    Serial.begin(115200);

    delay(10);

    // Initialize the LED

    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
    leds[0] = CRGB::White;
    FastLED.show();

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/R\">here</a> to set the LED to Red.<br>");
            client.print("Click <a href=\"/G\">here</a> to set the LED to Green.<br>");
            client.print("Click <a href=\"/B\">here</a> to set the LED to Blue.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /R" or "GET /G or "GET /B"":
        if (currentLine.endsWith("GET /R")) {
          leds[0] = CRGB::Red;
          FastLED.show();
        }
        if (currentLine.endsWith("GET /G")) {
          leds[0] = CRGB::Green;
          FastLED.show();
        }
        if (currentLine.endsWith("GET /B")) {
          leds[0] = CRGB::Blue;
          FastLED.show();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
