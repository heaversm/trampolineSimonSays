//LEDS
#include <Adafruit_NeoPixel.h>

#define PIN 7 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 9
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int ledBrightness = 30; //0 - 255

//PIEZO
const int PIEZO_PIN = A0; // Piezo output

//TIMING
int period = 5000;
unsigned long time_now = 0;
boolean isActive = false;
 
void setup() {
    Serial.begin(115200);
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.setBrightness(ledBrightness);
    pixels.show(); // Set all pixel colors to 'off'
}
 
void loop() {
    if(millis() > time_now + period && !isActive){
        time_now = millis();
        //Serial.println("active");
        for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
          // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
          pixels.setPixelColor(i, pixels.Color(0, 150, 0));
          pixels.show();   // Send the updated pixel colors to the hardware.
        }
        isActive = true;
    } else if (millis() > time_now + period && isActive){
        time_now = millis();
        //Serial.println("inactive");
        pixels.clear();
        pixels.show();
        isActive = false;
    }
   
    //PIEZO
  int piezoADC = analogRead(PIEZO_PIN);
  float piezoV = piezoADC / 1023.0 * 5.0;
  Serial.println(piezoV); // Print the voltage.
  
}
