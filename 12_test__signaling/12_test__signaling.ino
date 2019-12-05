//LEDS
#include <Adafruit_NeoPixel.h>

#define PIN 7 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 9 //how many lights are on our LED strip?
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //initialize neopixels

int ledBrightness = 30; //0 - 255 - set the overall brightness of our strip
float vibrationThreshold = 4; //above this value, trigger a bounce

//define the colors we want to cycle through
uint32_t colorRed = pixels.Color(255, 0, 0);
uint32_t colorGreen = pixels.Color(0, 150, 0);
uint32_t colorBlue = pixels.Color(0, 255, 255);
uint32_t colorYellow = pixels.Color(255, 255, 0);

//add them to an array in the order we want them to display
uint32_t colorArray[4] = {colorRed, colorGreen, colorBlue, colorYellow};

int curColorIndex = 0; //keeps track of the current color to display

//PATTERN

bool isPatternMode = true; //when true, display patterns, when false, user repeats pattern
int patternColorDisplayTime = 3000; //how long to display each color in pattern mode
int patternCompleteReadyTime = 1000; //how long to give the user after the pattern has been displayed before they can start jumping
int curStage = 0; //level of the game (array index of patternArray colors to iterate through)
uint32_t patternArray[4] = {colorRed, colorGreen, colorBlue, colorYellow}; //TODO: randomize this pattern! //array specifying pattern which user will have to replicate. After reaching last color, they win the game


//PIEZO
const int PIEZO_PIN = A0; // pin on which we read vibration / piezo output

//BOUNCE MODE
int bounceTime = 1000; //how much time must during vibration to constitute a bounce / cycle through colors. Essentially this would be long enough to allow the trampoline to vibrate during a jump, but short enough that it's ready to detect again before the user lands, i.e. "airborne time"
int stopBounceTime = 5000; //how much time must pass after bounce to consider it a "submission" of the current color - i.e. how long the user must wait off of the trampoline
unsigned long time_now = 0; //keeps track of the passing of time to constitute a bounce and stop bounce
boolean isBounced = false; //the user has started a bounce (should occur while each jump is occuring)
boolean hasBegunBouncing = false; //true when user has triggered vibration for the first time for each color guess

int correctCount = 0; //keeps track of how many colors have been guessed correctly so far for the current stage. Set back to zero each stage / level

void setup() {
  Serial.begin(115200);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(ledBrightness);
  pixels.show(); // Set all pixel colors to 'off'
}

void loop() {

  if (isPatternMode) {
    handlePatternMode();
  } else {
    handleBounceMode();
  }


}

void handlePatternMode() { //display the pattern the user must replicate
  for (int i = 0; i <= curStage; i++) {
    uint32_t curPatternColor = patternArray[i];
    for (int j = 0; j < NUMPIXELS; j++) { // For each pixel...

      pixels.setPixelColor(j, curPatternColor);
      pixels.show(); //Send the updated pixel colors to the hardware.
    }
    delay(patternColorDisplayTime);
  }

  pixels.clear();
  pixels.show();
  delay(patternCompleteReadyTime);
  time_now = millis();
  isPatternMode = false;
}

void handleBounceMode() {
  //PIEZO
  int piezoADC = analogRead(PIEZO_PIN);
  float piezoV = piezoADC / 1023.0 * 5.0; //scale the vibration to a more user friendly range

  /*
    if (piezoV > vibrationThreshold){
    Serial.println(piezoV); // Print the voltage.
    }
  */

  if (piezoV > vibrationThreshold && !isBounced) { //if the vibration is significant enough to constitute a bounce, and we haven't already triggered a bounce...
    Serial.println("bounce");

    time_now = millis();

    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel on the strip
      uint32_t curPixelColor = colorArray[curColorIndex];
      pixels.setPixelColor(i, curPixelColor);
      pixels.show();   // Send the updated pixel colors to the hardware.
    }
    isBounced = true; //register a bounce - keeps this block of code (pixel showing) from happening repeatedly
  }

  if ((millis() > time_now + bounceTime) && isBounced && (millis() < time_now + stopBounceTime)) { //we have a completed bounce, but not a stop bouncing, i.e. user is airborne
    isBounced = false; //keeps this block from repeating
    if (!hasBegunBouncing) {
      Serial.println("has begun bouncing");
      hasBegunBouncing = true; //stays true while user is cycling through colors but has not yet submitted a guess
    }
    Serial.println("bounce");
    time_now = millis();
    pixels.clear();
    pixels.show();
    if (curColorIndex < 3) { //prepare the next color in the sequence to be displayed
      curColorIndex++;
    } else {
      curColorIndex = 0;
    }

  } else if ((millis() >= time_now + stopBounceTime) && !isBounced && hasBegunBouncing) { //we've stopped jumping and are waiting to see if our submission is correct
    Serial.println("submit color guess");
    hasBegunBouncing = false; //keeps this block of code from running repeatedly

    int lastColorIndex = curColorIndex - 1;
    if (curColorIndex == -1) {
      lastColorIndex = 3;
    }


    if (colorArray[lastColorIndex] == patternArray[correctCount]) { //if the color we stopped bouncing at matches the stage in the pattern so far

      if (correctCount == curStage) { //we have gotten all the colors in this stage correct
        if (curStage < 3) {
          showStageComplete();
        } else {
          Serial.println("you win!!! Start over");
          curStage = 0;
        }

        curColorIndex = 0;
        correctCount = 0;
        isPatternMode = true; //show the next pattern sequence
      } else { //if there are more colors, we need to keep bouncing to get the next color in the sequence
        correctCount++;
        curColorIndex = 0;
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

void showStageComplete() {
  Serial.println("correct pattern! Next stage");
  //signal a correct pattern...
  blinkLights(colorGreen, 3); //color, numBlinks
  curStage++; //then advance to the next stage
}

int blinkOnTime = 500;
int blinkOffTime = 500;
int finishBlinkPauseTime = 1000; //how long to leave the lights off after blinking before advancing to next order of operations

void blinkLights(uint32_t blinkColor, int numBlinks) {
  for (int j = 0; j < numBlinks; j++) {
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel on the strip
      pixels.setPixelColor(i, blinkColor);
      pixels.show(); // Send the updated pixel colors to the hardware.
    }
    delay(blinkOnTime);
    pixels.clear();
    pixels.show();
    delay(blinkOffTime);
  }
  delay(finishBlinkPauseTime);
}
