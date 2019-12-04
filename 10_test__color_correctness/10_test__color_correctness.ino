//LEDS
#include <Adafruit_NeoPixel.h>

#define PIN 7 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 9
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int ledBrightness = 30; //0 - 255
float vibrationThreshold = 4; //above this value, trigger a bounce

/*
int colorRed[3] = { 255, 0, 0 };
int colorGreen[3] = { 0, 150, 0 };
int colorBlue[3] = { 0, 255, 255 };
int colorYellow[3] = { 255, 255, 0 };

int* colorArray[4] = {colorRed,colorGreen,colorBlue,colorYellow};
*/

uint32_t colorRed = pixels.Color(255,0,0);
uint32_t colorGreen = pixels.Color(0,150,0);
uint32_t colorBlue = pixels.Color(0,255,255);
uint32_t colorYellow = pixels.Color(255,255,0);

uint32_t colorArray[4] = {colorRed,colorGreen,colorBlue,colorYellow};

int curColorIndex = 0;

//PATTERN

bool isPatternMode = true; //when true, display patterns, when false, user repeats pattern
int patternColorDisplayTime = 3000; //how long to display each color
int curStage = 0; //level of the game (array index of patternArray colors to iterate through)
uint32_t patternArray[4] = {colorRed,colorGreen,colorBlue,colorYellow};


//PIEZO
const int PIEZO_PIN = A0; // Piezo output

//BOUNCE MODE
int bounceTime = 1000; //how much time pass during vibration to constitute a bounce / cycle through colors
int stopBounceTime = 5000; //how much time must pass after bounce to consider it a "submission" of the current color
unsigned long time_now = 0;
boolean isBounced = false;
boolean hasBegunBouncing = false;
boolean isPatternMatch = false;

void setup() {
  Serial.begin(115200);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(ledBrightness);
  pixels.show(); // Set all pixel colors to 'off'
}

void loop() {

  if (isPatternMode){
    handlePatternMode();
  } else {
    handleBounceMode();
  }
  

}

void handlePatternMode(){
  for (int i = 0; i <= curStage; i++) {
    uint32_t curPatternColor = patternArray[i];
    pixels.setPixelColor(i, curPatternColor);
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(patternColorDisplayTime);
  }

  pixels.clear();
  pixels.show();
  delay(1000);
  time_now = millis();
  isPatternMode = false;
}

void handleBounceMode(){
  //PIEZO
  int piezoADC = analogRead(PIEZO_PIN);
  float piezoV = piezoADC / 1023.0 * 5.0;
  
  /*
  if (piezoV > vibrationThreshold){
    Serial.println(piezoV); // Print the voltage.
  }
  */

  if (piezoV > vibrationThreshold && !isBounced) {
    isBounced = true; //register a bounce
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      uint32_t curPixelColor = colorArray[curColorIndex];
      pixels.setPixelColor(i, curPixelColor);
      pixels.show();   // Send the updated pixel colors to the hardware.
    }
  }

  if (millis() > time_now + bounceTime && isBounced && millis() < time_now + stopBounceTime) {
    time_now = millis();
    pixels.clear();
    pixels.show();
    isBounced = false;
    hasBegunBouncing = true;
    if (curColorIndex < 3){
      curColorIndex++;
    } else {
      curColorIndex = 0;
    }
  } else if (millis() > time_now + stopBounceTime && !isBounced && hasBegunBouncing) {
    Serial.println("stop");
    hasBegunBouncing = false;
    
    //get current color
    bool isColorMatch = false;

    int lastColorIndex = curColorIndex - 1;
    if (curColorIndex == -1){
      lastColorIndex = 3;
    }

    
    if (lastColorIndex == curStage){ //MH - this logic will have to change - currently matches only the latest color, doesn't check if the colors before it were a match
      Serial.println("correct");
      isPatternMatch = true;
    }
    
    if (isPatternMatch){ //if the whole pattern is correct:
      //curStage++; //MH - temp
    } else { //else restart the pattern:
      //MH - not sure theres anything special we need to do here
    }


    //regardless of right or wrong, display the pattern again:
    isPatternMode = true;
  }
}
