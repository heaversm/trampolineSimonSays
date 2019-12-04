//LEDS
#include <Adafruit_NeoPixel.h>

#define PIN 7 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 9
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int ledBrightness = 30; //0 - 255
float vibrationThreshold = 4; //above this value, trigger a bounce

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

int correctCount = 0; //keeps track of how many colors have been guessed correctly so far for the current stage

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
    for (int j = 0; j < NUMPIXELS; j++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(j, curPatternColor);
      pixels.show();   // Send the updated pixel colors to the hardware.
    }
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
    Serial.println("bounce");
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      uint32_t curPixelColor = colorArray[curColorIndex];
      pixels.setPixelColor(i, curPixelColor);
      pixels.show();   // Send the updated pixel colors to the hardware.
    }
    isBounced = true; //register a bounce
  }

  if ((millis() > time_now + bounceTime) && isBounced && (millis() < time_now + stopBounceTime)) { //we have a completed bounce, but not a stop bouncing
    if (!hasBegunBouncing){
      Serial.println("has begun bouncing");
      hasBegunBouncing = true;
    }
    Serial.println("bounce");
    time_now = millis();
    pixels.clear();
    pixels.show();
    if (curColorIndex < 3){
      curColorIndex++;
    } else {
      curColorIndex = 0;
    }
    isBounced = false;
  } else if ((millis() >= time_now + stopBounceTime) && isBounced && hasBegunBouncing) { //we're pausing to see if our submission is correct
    Serial.println("submit color guess");
    hasBegunBouncing = false;

    int lastColorIndex = curColorIndex - 1;
    if (curColorIndex == -1){
      lastColorIndex = 3;
    }

    
    if (colorArray[lastColorIndex] == patternArray[correctCount]){ //if the color we stopped bouncing at matches the stage in the pattern so far

      if (correctCount == curStage){ //we have gotten all the colors in this stage correct
        if (curStage < 3){
          Serial.println("correct pattern! Next stage");
          curStage++; //advance the stage
        } else {
          Serial.println("you win!!! Start over");
          curStage = 0;
        }
        
        curColorIndex = 0;
        correctCount = 0;
        isPatternMode = true; //show the next pattern sequence
      } else { //if there are more colors, we need to keep bouncing to get the next color in the sequence
        correctCount++;
        Serial.print("correct so far: ");
        Serial.println(correctCount);
      }
      
    } else { //wrong color guess, start over:
      Serial.println("incorrect");
      curColorIndex = 0;
      correctCount = 0;
      isPatternMode = true;
    }

    hasBegunBouncing = false;
    time_now = millis();
    isBounced = false;
    
  }
}
