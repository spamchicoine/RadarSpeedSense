#include <HardwareSerial.h>

//Define two Serial devices mapped to the two internal UARTs
HardwareSerial Serial0(0);

void get_data(){
  if (Serial.available()){
    Serial.println(Serial.readString());
  }
  else{
    Serial.println("No serial data found");
  }
}

void setup(){
  // For the USB, just use Serial as normal:
  Serial.begin(19200);
  delay(100);

  // Configure MySerial0 on pins TX=D6 and RX=D7 (-1, -1 means use the default)
  Serial0.begin(19200, SERIAL_8N1, -1, -1);
  delay(100);

  while (!Serial);
  attachInterrupt(D7, get_data, CHANGE);
  delay(5);
}

void loop() {
  delay(100);
}
