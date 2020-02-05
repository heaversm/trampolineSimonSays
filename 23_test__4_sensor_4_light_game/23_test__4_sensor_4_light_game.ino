//LEDS
#include <Adafruit_NeoPixel.h>

//PINS:

//LED PINS
const int LEDPIN0 = 10; // Which pin on the Arduino is connected to the NeoPixels Red?
const int LEDPIN1 = 11; //Which pin on the Arduino is connected to the NeoPixels Green?
const int LEDPIN2 = 12; //Which pin on the Arduino is connected to the NeoPixels Blue?
const int LEDPIN3 = 13; //Which pin on the Arduino is connected to the NeoPixels Yellow?

//PIEZO PINS
const int PIEZO_PIN0 = 6; // pin on which we read vibration / piezo output
const int PIEZO_PIN1 = 7; // pin on which we read vibration / piezo output
const int PIEZO_PIN2 = 8; // pin on which we read vibration / piezo output
const int PIEZO_PIN3 = 9; // pin on which we read vibration / piezo output
const int piezos[4] = { PIEZO_PIN0, PIEZO_PIN1, PIEZO_PIN2, PIEZO_PIN3 };


//SOUND PINS
const int BOUNCE_SOUND_PIN = 1; //old: 8
const int WRONG_SOUND_PIN = 2; //old: 4
const int COLOR_CORRECT_SOUND_PIN = 3; //old: 5
const int PATTERN_CORRECT_SOUND_PIN = 4; //old: 2
const int WIN_SOUND_PIN = 1; //old: 3

const int NUMPIXELS = 172; //how many lights are on our LED strip?

 //initialize neopixels
Adafruit_NeoPixel pixels0(NUMPIXELS, LEDPIN0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels1(NUMPIXELS, LEDPIN1, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel pixels2(NUMPIXELS, LEDPIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3(NUMPIXELS, LEDPIN3, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pixelsArray[4] = { pixels0,pixels1,pixels2,pixels3 };



//define the colors we want to cycle through
uint32_t colorRed = pixels0.Color(255, 0, 0);
uint32_t colorGreen = pixels0.Color(0, 150, 0);
uint32_t colorBlue = pixels0.Color(0, 255, 255);
uint32_t colorYellow = pixels0.Color(255, 255, 0);

//add them to an array in the order we want them to display
int totalNumColors = 4; //number of colors user can cycle through
uint32_t colorArray[4] = {colorRed, colorGreen, colorBlue, colorYellow};


//VARIABLES:

//LED VARIABLES
int ledBrightness = 30; //0 - 255 - set the overall brightness of our strip
int blinkOnTime = 500; //how long to keep lights lit during blinking
int blinkOffTime = 500; //how long to keep lights off during blinking
int finishBlinkPauseTime = 500; //how long to leave the lights off after all blinks before advancing to next order of operations
int winOffDelay(3000); //how long to wait after win with the lights off before starting over


//PATTERN VARIABLES
int totalPatternLength = 4; //sets how long we want the game to be in terms of number of colors the user must match
int patternColorDisplayTime = 2000; //how long to display each color in pattern mode
int patternColorOffTime = 500; //how long between each color to turn off the lights (helps with distinguishing repeat colors in the pattern the user must reproduce)
int patternCompleteReadyTime = 500; //how long to give the user after the pattern has been displayed before they can start jumping
uint32_t patternArray[4];

//SOUND VARIABLES
int soundDelay = 1000;

//BOUNCE MODE VARIABLES
float vibrationThreshold = 4.5; //above this value, trigger a bounces
int bounceTime = 500; //how much time must during vibration to constitute a bounce / cycle through colors. Essentially this would be long enough to allow the trampoline to vibrate during a jump, but short enough that it's ready to detect again before the user lands, i.e. "airborne time"
int stopBounceTime = 3000; //how much time must pass after bounce to consider it a "submission" of the current color - i.e. how long the user must wait off of the trampoline

//GAME STATE VARIABLES
bool isPatternMode = true; //when true, display patterns, when false, user repeats pattern
bool isAssignPatternMode = true; //when true, make a random pattern at startup for the user to duplicate. Then turn this false so it doesn't run again
int curStage = 0; //level of the game (array index of patternArray colors to iterate through)
int curColorIndex = 0; //keeps track of the current color to display
int correctCount = 0; //keeps track of how many colors have been guessed correctly so far for the current stage. Set back to zero each stage / level
unsigned long time_now = 0; //keeps track of the passing of time to constitute a bounce and stop bounce
boolean isBounced = false; //the user has started a bounce (should occur while each jump is occuring)
boolean hasBegunBouncing = false; //true when user has triggered vibration for the first time for each color guess


void setup() {
  pinMode(BOUNCE_SOUND_PIN, OUTPUT);
  pinMode(WRONG_SOUND_PIN, OUTPUT);
  pinMode(COLOR_CORRECT_SOUND_PIN, OUTPUT);
  pinMode(PATTERN_CORRECT_SOUND_PIN, OUTPUT);
  pinMode(WIN_SOUND_PIN, OUTPUT);

  for (int i = 0; i < totalNumColors; i++) {
    pinMode(piezos[i], INPUT);
    pixelsArray[i].begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixelsArray[i].setBrightness(ledBrightness);
    pixelsArray[i].show(); // Set all pixel colors to 'off'
  }


  digitalWrite(BOUNCE_SOUND_PIN, HIGH);
  digitalWrite(WRONG_SOUND_PIN, HIGH);
  digitalWrite(COLOR_CORRECT_SOUND_PIN, HIGH);
  digitalWrite(PATTERN_CORRECT_SOUND_PIN, HIGH);
  digitalWrite(WIN_SOUND_PIN, HIGH);

  Serial.begin(115200);
  randomSeed(analogRead(1)); //allows for truly random values in the pattern



}

void loop() {
  if (isAssignPatternMode) { //MH - should run only once
    assignNewPatternColors();
    isAssignPatternMode = false;
  } else {

    if (isPatternMode) {
      handlePatternMode();
    } else {
      handleBounceMode();
    }

  }
}

void assignNewPatternColors() {
  for (int i = -0; i < totalPatternLength; i++) {
    int randIndex = random(totalNumColors);
    patternArray[i] = colorArray[randIndex];
    Serial.print("color ");
    Serial.print(i);
    Serial.print (" = ");
    Serial.print(randIndex);
  }
  Serial.println("begin");
}

void handlePatternMode() { //display the pattern the user must replicate
  for (int i = 0; i <= curStage; i++) {
    uint32_t curPatternColor = patternArray[i];
    pixelsArray[i].fill(curPatternColor, 0, NUMPIXELS);
    pixelsArray[i].show();
    delay(patternColorDisplayTime);
    pixelsArray[i].clear();
    pixelsArray[i].show();
    delay(patternColorOffTime);
  }

  turnOffLights();
  delay(patternCompleteReadyTime);
  time_now = millis();
  isPatternMode = false;
  Serial.println("pattern done");
}

void handleBounceMode() {
  //PIEZO
  for (int i = 0; i < totalNumColors; i++) {
    int piezoV = digitalRead(piezos[i]);
    Serial.println(piezoV);
    if (piezoV == HIGH) { //if the vibration is significant enough to constitute a bounce, and we haven't already triggered a bounce...
      time_now = millis();
      isBounced = true; //register a bounce - keeps this block of code (pixel showing) from happening repeatedly
      curColorIndex = i;
      uint32_t curPixelColor = colorArray[curColorIndex];
      Serial.println(piezoV);
      pixels0.fill(curPixelColor, 0, NUMPIXELS);
      pixels0.show();
      digitalWrite(BOUNCE_SOUND_PIN, LOW);
    }

    if ((millis() > time_now + bounceTime) && isBounced /*&& (millis() < time_now + stopBounceTime)*/) { //we have a completed bounce, but not a stop bouncing, i.e. user is airborne
      isBounced = false; //keeps this block from repeating
      digitalWrite(BOUNCE_SOUND_PIN, HIGH);

      time_now = millis();
      turnOffLights();
      hasBegunBouncing = false; //keeps this block of code from running repeatedly

      if (colorArray[curColorIndex] == patternArray[correctCount]) { //if the color we stopped bouncing at matches the stage in the pattern so far

        if (correctCount == curStage) { //we have gotten all the colors in this stage correct
          if (curStage < totalPatternLength - 1) {
            showStageComplete();
          } else {
            showWinSequence();
          }
          curColorIndex = 0;
          correctCount = 0;
          isPatternMode = true; //show the next pattern sequence
        } else { //if there are more colors, we need to keep bouncing to get the next color in the sequence
          showCorrectSoFar();
        }

      } else { //wrong color guess, start over:
        showIncorrect();
      }

      hasBegunBouncing = false;
      time_now = millis();
      isBounced = false;

    }
  }
}

void showStageComplete() {
  Serial.println("correct pattern! Next stage");
  digitalWrite(PATTERN_CORRECT_SOUND_PIN, LOW);
  delay(soundDelay);
  digitalWrite(PATTERN_CORRECT_SOUND_PIN, HIGH);
  //signal a correct pattern...
  blinkLights(colorGreen, 3); //color, numBlinks
  curStage++; //then advance to the next stage
}

void showWinSequence(){
  Serial.println("you win!!! Start over");
  digitalWrite(WIN_SOUND_PIN, LOW);
  delay(soundDelay);
  digitalWrite(WIN_SOUND_PIN, HIGH);
  rainbow(5);
  turnOffLights();
  delay(winOffDelay);
  assignNewPatternColors();
  curStage = 0;
}

void showCorrectSoFar() {
  Serial.print("correct so far: ");
  Serial.println(correctCount + 1);
  correctCount++;
  curColorIndex = 0;
}

void showIncorrect() {
  Serial.println("incorrect");
  digitalWrite(WRONG_SOUND_PIN, LOW);
  delay(soundDelay);
  digitalWrite(WRONG_SOUND_PIN, HIGH);
  blinkLights(colorRed, 3);
  curColorIndex = 0;
  correctCount = 0;
  isPatternMode = true;
}

void turnOffLights() {
  //TODO: does this need to include all pixels in each strip? i.e. 172 loop
  for (int i = 0; i < totalNumColors; i++) { // For each pixel on the strip
     pixelsArray[i].clear();
     pixelsArray[i].show();
  }

}

void blinkLights(uint32_t blinkColor, int numBlinks) {
  for (int i=0; i<totalNumColors;i++){
    for (int j = 0; j < numBlinks; j++) {
      pixelsArray[i].fill(blinkColor, 0, NUMPIXELS);
      pixelsArray[i].show();
      if (i == totalNumColors - 1){
        delay(blinkOnTime); //TODO: millis
        turnOffLights();
        delay(blinkOffTime); //TODO: millis
      }
    }
  }
  delay(finishBlinkPauseTime);
}

void rainbow(int wait) {
  for (int j=0; j<totalNumColors;j++){
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
      for (int i = 0; i < NUMPIXELS; i++) { // For each pixel in strip...
        int pixelHue = firstPixelHue + (i * 65536L / NUMPIXELS);
        pixelsArray[j].setPixelColor(i, pixelsArray[j].gamma32(pixelsArray[j].ColorHSV(pixelHue)));
      }
      pixelsArray[j].show(); // Update strip with new contents
      if (i == totalNumColors - 1){
        delay(wait);  // Pause for a moment //TODO: millis
      }
    }
  }
}
