/******************************************************************************
Piezo_Vibration_Sensor.ino
Example sketch for SparkFun's Piezo Vibration Sensor
  (https://www.sparkfun.com/products/9197)
Jim Lindblom @ SparkFun Electronics
April 29, 2016

- Connect a 1Mohm resistor across the Piezo sensor's pins.
- Connect one leg of the Piezo to GND
- Connect the other leg of the piezo to A0

Vibrations on the Piezo sensor create voltags, which are sensed by the Arduino's
A0 pin. Check the serial monitor to view the voltage generated.

Development environment specifics:
Arduino 1.6.7
******************************************************************************/
const int PIEZO_PIN = A0; // Piezo output
const int PIEZO_PIN2 = A1; // Piezo output
const int PIEZO_PIN3 = A3; // Piezo output
const int PIEZO_PIN4 = A5; // Piezo output
float THRESHOLD = 3.5;

void setup() 
{
  Serial.begin(9600);
  
}

void loop() 
{
  // Read Piezo ADC value in, and convert it to a voltage
  int piezoADC = analogRead(PIEZO_PIN);
  float piezoV = piezoADC / 1023.0 * 5.0;

  int piezoADC2 = analogRead(PIEZO_PIN2);
  float piezoV2 = piezoADC2 / 1023.0 * 5.0;

  int piezoADC3 = analogRead(PIEZO_PIN3);
  float piezoV3 = piezoADC3 / 1023.0 * 5.0;

  int piezoADC4 = analogRead(PIEZO_PIN4);
  float piezoV4 = piezoADC4 / 1023.0 * 5.0;
  if (piezoV > THRESHOLD){
    Serial.print("1: ");
    Serial.println(piezoV); // Print the voltage.
    
  }


  if (piezoV2 > THRESHOLD){
    Serial.print("2: ");
    Serial.println(piezoV2); // Print the voltage.
    
  }

  if (piezoV3 > THRESHOLD){
    Serial.print("3: ");
    Serial.println(piezoV3); // Print the voltage.
  }

  if (piezoV4 > THRESHOLD){
    Serial.print("4: ");
    Serial.println(piezoV4); // Print the voltage.
  }
  
}
