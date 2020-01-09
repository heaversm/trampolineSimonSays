
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        7 

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 120 // Popular NeoPixel ring size

int PIXELSPACING = 9; //how many pixels to skip between each lit pixel
int ledBrightness = 30; //0 - 255 - set the overall brightness of our strip

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.




//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(ledBrightness);
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    if (i % PIXELSPACING == 0){
       // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
    }
    

    delay(DELAYVAL); // Pause before next pass through loop
  }
}
