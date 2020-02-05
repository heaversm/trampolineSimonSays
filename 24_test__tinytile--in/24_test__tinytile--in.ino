
//LED PINS
const int LEDPIN = 10; 

boolean doSerial = true;

void setup() {
  if (doSerial){
    Serial.begin(9600);
    while (!Serial);    // wait for the serial port to open
    Serial.println("initialized");
  }
  
  pinMode(LEDPIN, INPUT);

}

void loop() {
  int ledVal = digitalRead(LEDPIN);
  if (ledVal == HIGH){
    if (doSerial){
      Serial.println("bounce");
    }
  }
}
