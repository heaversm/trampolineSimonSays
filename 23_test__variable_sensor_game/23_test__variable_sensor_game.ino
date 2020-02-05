//LEDS
#include <Adafruit_NeoPixel.h>

//#define PIN 7 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 172 //how many lights are on our LED strip?
//#define NUMPIXELS 9


uint32_t* colorRed = new uint32_t;
uint32_t* colorGreen = new uint32_t;
uint32_t* colorBlue = new uint32_t;
uint32_t* colorYellow = new uint32_t;


int ledBrightness = 30; //0 - 255 - set the overall brightness of our strip

//add them to an array in the order we want them to display
int totalNumColors = 4; //number of colors user can cycle through
uint32_t* totalColorArray = new uint32_t;
uint32_t* colorArray = new uint32_t[totalNumColors];

int curColorIndex = 0; //keeps track of the current color to display

int blinkOnTime = 500; //how long to keep lights lit during blinking
int blinkOffTime = 500; //how long to keep lights off during blinking
int finishBlinkPauseTime = 500; //how long to leave the lights off after all blinks before advancing to next order of operations
int winOffDelay(3000); //how long to wait after win with the lights off before starting over


//PATTERN
int totalPatternLength = 4; //sets how long we want the game to be in terms of number of colors the user must match
bool isAssignPatternMode = true; //when true, make a random pattern at startup for the user to duplicate. Then turn this false so it doesn't run again
bool isPatternMode = true; //when true, display patterns, when false, user repeats pattern
int patternColorDisplayTime = 2000; //how long to display each color in pattern mode
int patternColorOffTime = 500; //how long between each color to turn off the lights (helps with distinguishing repeat colors in the pattern the user must reproduce)
int patternCompleteReadyTime = 500; //how long to give the user after the pattern has been displayed before they can start jumping
int curStage = 0; //level of the game (array index of patternArray colors to iterate through)
uint32_t* patternArray = new uint32_t[totalPatternLength];


//PIEZO
const int PIEZO_PIN0 = 9; // pin on which we read vibration / piezo output
const int PIEZO_PIN1 = 10; // pin on which we read vibration / piezo output
const int PIEZO_PIN2 = 11; // pin on which we read vibration / piezo output
const int PIEZO_PIN3 = 12; // pin on which we read vibration / piezo output
uint32_t totalPiezoArray[4] = {PIEZO_PIN0, PIEZO_PIN1, PIEZO_PIN2, PIEZO_PIN3};
uint32_t* piezos = new uint32_t[totalNumColors];

Adafruit_NeoPixel pixels0(NUMPIXELS, PIEZO_PIN0, NEO_GRB + NEO_KHZ800); //initialize neopixels
Adafruit_NeoPixel pixels1(NUMPIXELS, PIEZO_PIN1, NEO_GRB + NEO_KHZ800); //initialize neopixels
Adafruit_NeoPixel pixels2(NUMPIXELS, PIEZO_PIN2, NEO_GRB + NEO_KHZ800); //initialize neopixels
Adafruit_NeoPixel pixels3(NUMPIXELS, PIEZO_PIN3, NEO_GRB + NEO_KHZ800); //initialize neopixels


//SOUNDS
const int BOUNCE_SOUND_PIN = 8;
const int WRONG_SOUND_PIN = 4;
const int COLOR_CORRECT_SOUND_PIN = 5;
const int PATTERN_CORRECT_SOUND_PIN = 2;
const int WIN_SOUND_PIN = 3;
int soundDelay = 1000;

//BOUNCE MODE
float vibrationThreshold = 4.5; //above this value, trigger a bounces
int bounceTime = 500; //how much time must during vibration to constitute a bounce / cycle through colors. Essentially this would be long enough to allow the trampoline to vibrate during a jump, but short enough that it's ready to detect again before the user lands, i.e. "airborne time"
int stopBounceTime = 3000; //how much time must pass after bounce to consider it a "submission" of the current color - i.e. how long the user must wait off of the trampoline
unsigned long time_now = 0; //keeps track of the passing of time to constitute a bounce and stop bounce
boolean isBounced = false; //the user has started a bounce (should occur while each jump is occuring)
boolean hasBegunBouncing = false; //true when user has triggered vibration for the first time for each color guess

int correctCount = 0; //keeps track of how many colors have been guessed correctly so far for the current stage. Set back to zero each stage / level

void assignColorArray(){
  for (int i=0;i<totalNumColors;i++){
    colorArray[i] = totalColorArray[i];
    piezos[i] = totalPiezoArray[i];
  }  
};

void setup() {
  assignColorArray();
  pinMode(BOUNCE_SOUND_PIN, OUTPUT);
  pinMode(WRONG_SOUND_PIN, OUTPUT);
  pinMode(COLOR_CORRECT_SOUND_PIN, OUTPUT);
  pinMode(PATTERN_CORRECT_SOUND_PIN, OUTPUT);
  pinMode(WIN_SOUND_PIN, OUTPUT);

  digitalWrite(BOUNCE_SOUND_PIN, HIGH);
  digitalWrite(WRONG_SOUND_PIN, HIGH);
  digitalWrite(COLOR_CORRECT_SOUND_PIN, HIGH);
  digitalWrite(PATTERN_CORRECT_SOUND_PIN, HIGH);
  digitalWrite(WIN_SOUND_PIN, HIGH);

  Serial.begin(115200);
  randomSeed(analogRead(1)); //allows for truly random values in the pattern

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(ledBrightness);
  pixels.show(); // Set all pixel colors to 'off'


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
    pixels.fill(curPatternColor, 0, NUMPIXELS);
    pixels.show();
    delay(patternColorDisplayTime);
    pixels.clear();
    pixels.show();
    delay(patternColorOffTime);
  }

  pixels.clear();
  pixels.show();
  delay(patternCompleteReadyTime);
  time_now = millis();
  isPatternMode = false;
  Serial.println("pattern done");
}

void handleBounceMode() {
  //PIEZO
  for (int i = 0; i < 4; i++) {
    int piezoADC = analogRead(piezos[i]);
    float piezoV = piezoADC / 1023.0 * 5.0; //scale the vibration to a more user friendly range
    Serial.println(piezoV);
    if (piezoV > vibrationThreshold && !isBounced) { //if the vibration is significant enough to constitute a bounce, and we haven't already triggered a bounce...
      time_now = millis();
      isBounced = true; //register a bounce - keeps this block of code (pixel showing) from happening repeatedly
      curColorIndex = i;
      uint32_t curPixelColor = colorArray[curColorIndex];
      Serial.println(piezoV);
      pixels.fill(curPixelColor, 0, NUMPIXELS);
      pixels.show();
      digitalWrite(BOUNCE_SOUND_PIN, LOW);
    }

    if ((millis() > time_now + bounceTime) && isBounced /*&& (millis() < time_now + stopBounceTime)*/) { //we have a completed bounce, but not a stop bouncing, i.e. user is airborne
      isBounced = false; //keeps this block from repeating
      digitalWrite(BOUNCE_SOUND_PIN, HIGH);

      /*
      if (!hasBegunBouncing) {
        Serial.println("has begun bouncing");
        hasBegunBouncing = true; //stays true while user is cycling through colors but has not yet submitted a guess
      }*/
      //Serial.println("bounce");
      time_now = millis();
      pixels.clear();
      pixels.show();

      hasBegunBouncing = false; //keeps this block of code from running repeatedly


      if (colorArray[curColorIndex] == patternArray[correctCount]) { //if the color we stopped bouncing at matches the stage in the pattern so far

        if (correctCount == curStage) { //we have gotten all the colors in this stage correct
          if (curStage < totalPatternLength - 1) {
            showStageComplete();
          } else {
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

void showCorrectSoFar() {
  Serial.print("correct so far: ");
  Serial.println(correctCount + 1);
  //digitalWrite(COLOR_CORRECT_SOUND_PIN, LOW);
  //delay(soundDelay);
  //digitalWrite(COLOR_CORRECT_SOUND_PIN, HIGH);
  //blinkLights(colorYellow, 3);
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
  //curStage = 0;
  isPatternMode = true;
}

void turnOffLights() {
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel on the strip
    pixels.clear();
    pixels.show();
  }
  //  pixels.clear();
  //  pixels.show();
}

void blinkLights(uint32_t blinkColor, int numBlinks) {
  for (int j = 0; j < numBlinks; j++) {
    pixels.fill(blinkColor, 0, NUMPIXELS);
    pixels.show();
    delay(blinkOnTime);
    pixels.clear();
    pixels.show();
    delay(blinkOffTime);
  }
  delay(finishBlinkPauseTime);
}

void rainbow(int wait) {
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / NUMPIXELS);
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
