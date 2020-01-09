
const int PIEZO_PIN = A0; // Piezo output
const int BOUNCE_PIN = 8; //Sound pin
const float VIBRATION_THRESHOLD = 4.0;
bool isTriggered = false;

void setup() 
{
  Serial.begin(9600);
  pinMode(BOUNCE_PIN, OUTPUT);
  digitalWrite(BOUNCE_PIN, HIGH);
}

void loop() 
{
  // Read Piezo ADC value in, and convert it to a voltage
  int piezoADC = analogRead(PIEZO_PIN);
  float piezoV = piezoADC / 1023.0 * 5.0;
  if (piezoV > VIBRATION_THRESHOLD){
    if (!isTriggered){
      Serial.println("trigger");
      isTriggered = true;
      digitalWrite(BOUNCE_PIN, LOW);
      delay(500);
      digitalWrite(BOUNCE_PIN, HIGH);
      isTriggered = false;
      delay(1000);
    }
    
  }
}
