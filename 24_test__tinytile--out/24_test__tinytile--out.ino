/*
   This sketch example demonstrates how the BMI160 on the
   Intel(R) Curie(TM) module can be used to read accelerometer data
*/

#include "CurieIMU.h"

float lastZ = 0;
float curZ = 0;

boolean isBounced = false;
int bounceCount = 0;

unsigned long time_now = 0;
int bounceTime = 500;

const int LEDPIN = 10;

boolean doSerial = false;


void setup() {
  if (doSerial) {
    Serial.begin(9600); // initialize Serial communication
    while (!Serial);    // wait for the serial port to open

    //initialize device
    Serial.println("Initializing IMU device...");
  }

  CurieIMU.begin();

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);

  pinMode(LEDPIN, OUTPUT);


}

void loop() {
  float ax, ay, az;   //scaled accelerometer values

  // read accelerometer measurements from device, scaled to the configured range
  CurieIMU.readAccelerometerScaled(ax, ay, az);

  // display tab-separated accelerometer x/y/z values
  lastZ = curZ;
  curZ = az;

  if (lastZ < 0 && curZ > 0 && !isBounced) {
    bounceCount++;
    time_now = millis();
    isBounced = true;
    //if (bounceCount % 2 == 0){
    if (doSerial) {
      Serial.println(az);
    }
    digitalWrite(LEDPIN, HIGH);
    //}

  } else {
    digitalWrite(LEDPIN, LOW);
  }

  if ((millis() > time_now + bounceTime) && isBounced) {
    isBounced = false;
    time_now = millis();

  }


}
