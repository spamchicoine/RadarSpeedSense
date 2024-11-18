
String speed = String("");
int c=0;

void setup(){
    // For the USB, just use Serial as normal:
    Serial.begin(19200);
    while(!Serial);
}

void loop() {
  speed.concat(String(" " + String(c)));
  Serial.println(speed);
  c++;
  delay(1000);
}
