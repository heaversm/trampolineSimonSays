
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        7 

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 172 
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
int ledBrightness = 30; //0 - 255 - set the overall brightness of our strip

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(ledBrightness);
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
  delay(DELAYVAL); // Pause before next pass through loop
  pixels.fill(pixels.Color(0,150,0),0,172);
  pixels.show();
  delay(DELAYVAL); // Pause before next pass through loop
}
