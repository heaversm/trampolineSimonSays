
int sensor = 0;           //Variable to store analog value (0-1023)


void setup()
{
  Serial.begin(9600);      //Only for debugging
}

void loop()
{
  sensor = analogRead(A0);
  //While sensor is not moving, analog pin receive 1023~1024 value
  if (sensor<1022){
    Serial.print("Sensor Value: ");
    Serial.println(sensor);
  }
  
  else{ 
    Serial.print("Sensor Value: ");
    Serial.println(sensor);
  }
  
  
  delay(100); //Small delay
}
