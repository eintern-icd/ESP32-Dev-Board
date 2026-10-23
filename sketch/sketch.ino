#include <Adafruit_NeoPixel.h>

#define PIN        18 // Onboard RGB LED pin on ESP32-S2-DevKitM-1 / ESP32-S2 Mini boards
#define NUMPIXELS 1   // Typically 1 onboard WS2812 RGB LED

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  // Set to green
  pixels.setPixelColor(0, pixels.Color(0, 150, 0));
  pixels.show();   
  delay(1000); 

  // Set to blue
  pixels.setPixelColor(0, pixels.Color(0, 0, 150));
  pixels.show();
  delay(1000);

  // Set to red
  pixels.setPixelColor(0, pixels.Color(150, 0, 0));
  pixels.show();
  delay(1000);
}
