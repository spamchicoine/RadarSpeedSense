#include <Arduino.h>
#include <driver/uart.h>

// Digit 1
const int DECODER0 = D1;
const int DECODER1 = D2;
const int DECODER2 = D3;
const int DECODER3 = D0;

// Digit 2
const int DECODER4 = D10;
const int DECODER5 = D9;
const int DECODER6 = D8;
const int DECODER7 = D5;

// Values to send to client
String values = String("");

// Digit values
int speed, d1, d2;
const int SPEED_LIMIT = 20;

unsigned long ltime, millidif;
int TIME_SINCE_VAL = 0;

// Values for keeping track of flashing the digits
unsigned long flash_time, flash_millidif;
int flash_flag = 0;

// Binary values for the BCD control pins, index corrosponds to digit
int dec_bin[][4] = {{0,0,0,0},
                    {0,0,0,1},
                    {0,0,1,0},
                    {0,0,1,1},
                    {0,1,0,0},
                    {0,1,0,1},
                    {0,1,1,0},
                    {0,1,1,1},
                    {1,0,0,0},
                    {1,0,0,1},
                    {1,0,1,0}}; // 10 for blank

void setup() {
  
  // Set BCD control pins to outputs
  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);
  pinMode(DECODER4, OUTPUT);
  pinMode(DECODER5, OUTPUT);
  pinMode(DECODER6, OUTPUT);
  pinMode(DECODER7, OUTPUT);

  // Set sev segs to blank by default
  digitalWrite(DECODER0, dec_bin[10][0]);
  digitalWrite(DECODER1, dec_bin[10][1]);
  digitalWrite(DECODER2, dec_bin[10][2]);
  digitalWrite(DECODER3, dec_bin[10][3]);
  digitalWrite(DECODER4, dec_bin[10][0]);
  digitalWrite(DECODER5, dec_bin[10][1]);
  digitalWrite(DECODER6, dec_bin[10][2]);
  digitalWrite(DECODER7, dec_bin[10][3]);

  // Begin Serial connection with Radar
  // If USB CDC on boot is disabled Serial defaults to RX/TX UART
  Serial.begin(115200);
  while (!Serial);
  delay(1000);
}

void loop() {
  ltime = (unsigned long)(millis() - millidif);

  if (ltime >= 1000){
    millidif = millidif + ltime;
    ltime = 0;
    TIME_SINCE_VAL+=1;
  }
  
  flash_time = (unsigned long)(millis() - flash_millidif);

  if (flash_time >= 200){
        flash_millidif = flash_millidif + flash_time;
        flash_time = 0;
        flash_flag = (flash_flag + 1) % 2;
  }
  
  // Handle data from radar
  if (Serial.available()){
    TIME_SINCE_VAL = 0;

    // returns 0 if invalid, radar will have minimum threshold above this anyways
    speed = Serial.readString().toInt();
    
    // assign digit values
    if (0 == speed){
      d1 = 10;
      d2 = 10;
    }
    else{
      d1 = speed % 10;
      d2 = speed / 10;
    }
  }

  if (TIME_SINCE_VAL < 3){
    if (speed > SPEED_LIMIT){
      if (flash_flag == 1){
        digitalWrite(DECODER0, dec_bin[d2][0]);
        digitalWrite(DECODER1, dec_bin[d2][1]);
        digitalWrite(DECODER2, dec_bin[d2][2]);
        digitalWrite(DECODER3, dec_bin[d2][3]);
        
        digitalWrite(DECODER4, dec_bin[d1][0]);
        digitalWrite(DECODER5, dec_bin[d1][1]);
        digitalWrite(DECODER6, dec_bin[d1][2]);
        digitalWrite(DECODER7, dec_bin[d1][3]);
      }
      else{
        digitalWrite(DECODER0, dec_bin[10][0]);
        digitalWrite(DECODER1, dec_bin[10][1]);
        digitalWrite(DECODER2, dec_bin[10][2]);
        digitalWrite(DECODER3, dec_bin[10][3]);
        digitalWrite(DECODER4, dec_bin[10][0]);
        digitalWrite(DECODER5, dec_bin[10][1]);
        digitalWrite(DECODER6, dec_bin[10][2]);
        digitalWrite(DECODER7, dec_bin[10][3]);
      }
    }
    else{
      digitalWrite(DECODER0, dec_bin[d2][0]);
      digitalWrite(DECODER1, dec_bin[d2][1]);
      digitalWrite(DECODER2, dec_bin[d2][2]);
      digitalWrite(DECODER3, dec_bin[d2][3]);
      
      digitalWrite(DECODER4, dec_bin[d1][0]);
      digitalWrite(DECODER5, dec_bin[d1][1]);
      digitalWrite(DECODER6, dec_bin[d1][2]);
      digitalWrite(DECODER7, dec_bin[d1][3]);
    }
  }
  else{
    digitalWrite(DECODER0, dec_bin[10][0]);
    digitalWrite(DECODER1, dec_bin[10][1]);
    digitalWrite(DECODER2, dec_bin[10][2]);
    digitalWrite(DECODER3, dec_bin[10][3]);
    digitalWrite(DECODER4, dec_bin[10][0]);
    digitalWrite(DECODER5, dec_bin[10][1]);
    digitalWrite(DECODER6, dec_bin[10][2]);
    digitalWrite(DECODER7, dec_bin[10][3]);
  }
}
void config() {
  //TO DO
}