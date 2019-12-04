int period = 5000;
unsigned long time_now = 0;
boolean isActive = false;
 
void setup() {
    Serial.begin(115200);
}
 
void loop() {
    if(millis() > time_now + period && !isActive){
        time_now = millis();
        Serial.println("active");
        isActive = true;
    } else if (millis() > time_now + period && isActive){
        time_now = millis();
        Serial.println("inactive");
        isActive = false;
    }
   
    //Run other code
}
