#include <Arduino.h>
#include <driver/uart.h>

#include "WiFi.h"
#include "time.h"
#include "wifi_setup.h"

// Digit 1
const int DECODER0 = D0;
const int DECODER1 = D3;
const int DECODER2 = D2;
const int DECODER3 = D1;

// Digit 2
const int DECODER4 = D5;
const int DECODER5 = D8;
const int DECODER6 = D9;
const int DECODER7 = D10;

// Values to send to client
String values = String("");

// Digit values
int speed, d1, d2;
const int SPEED_LIMIT = 25;

// Values for keeping track of time
struct tm timeinfo;
size_t maxsize = 20;
unsigned long ltime, millidif;

// Values for keeping track of sleeping
int TIME_SINCE_VAL = 0;
int uMS_S_FACTOR = 1000000;

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
  // Begin Serial connection with Radar
  // If USB CDC on boot is disabled Serial defaults to RX/TX UART
  Serial.begin(115200);
  while (!Serial);
  delay(2000);
  
  // Begin WiFi connection with client
  setup_wifi();

  // Sync time with ntp server
  configTime(0, 0, "pool.ntp.org");
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

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

  // Sleep setup
  gpio_sleep_set_direction(GPIO_NUM_20, GPIO_MODE_INPUT);
  gpio_sleep_set_pull_mode(GPIO_NUM_20, GPIO_PULLUP_ONLY);

  uart_set_wakeup_threshold(UART_NUM_0, 3);   // 3 edges on U0RXD to wakeup
  esp_sleep_enable_uart_wakeup(UART_NUM_0);   // Enable UART 0 as wakeup source

  Serial.flush();
}

void loop() {
  
  ltime = (unsigned long)(millis() - millidif);

  // Update time
  if (ltime >= 1000){
    millidif = millidif + ltime;
    ltime = 0;
    TIME_SINCE_VAL+=1;

    if (59 < ++timeinfo.tm_sec){
      timeinfo.tm_sec = 0;

      if (59 < ++timeinfo.tm_min){
        timeinfo.tm_min = 0;
        configTime(0, 0, "pool.ntp.org");
        if(!getLocalTime(&timeinfo)){
          Serial.println("Failed to obtain time");
          return;
        }
        send_SpeedData(values);
        values = String("");
        char timestamp[maxsize];
        strftime(timestamp, maxsize, "%D-%T", &timeinfo);
        values.concat(String(timestamp));
      }
    }
  }
  
  // Handle data from radar
  if (Serial.available()){
    TIME_SINCE_VAL = 0;
    speed = Serial.readString().toInt();
    values.concat(" "+String(speed));
    
    d1 = speed % 10;
    d2 = speed / 10;

    // Handle flashing of digits
    flash_time = (unsigned long)(millis() - flash_millidif);

    if (flash_time >= 50){
        flash_millidif = flash_millidif + flash_time;
        flash_time = 0;
        flash_flag = (flash_flag + 1) % 2;
    }
    // Set digits
    if (speed > SPEED_LIMIT){
      if (flash_flag == 1) {
        
        digitalWrite(DECODER0, dec_bin[10][0]);
        digitalWrite(DECODER1, dec_bin[10][1]);
        digitalWrite(DECODER2, dec_bin[10][2]);
        digitalWrite(DECODER3, dec_bin[10][3]);
        
        digitalWrite(DECODER4, dec_bin[d2][0]);
        digitalWrite(DECODER5, dec_bin[d2][1]);
        digitalWrite(DECODER6, dec_bin[d2][2]);
        digitalWrite(DECODER7, dec_bin[d2][3]);
      }
      else {
        digitalWrite(DECODER0, dec_bin[d1][0]);
        digitalWrite(DECODER1, dec_bin[d1][1]);
        digitalWrite(DECODER2, dec_bin[d1][2]);
        digitalWrite(DECODER3, dec_bin[d1][3]);

        digitalWrite(DECODER4, dec_bin[10][0]);
        digitalWrite(DECODER5, dec_bin[10][1]);
        digitalWrite(DECODER6, dec_bin[10][2]);
        digitalWrite(DECODER7, dec_bin[10][3]);
      }
    }
    else{
      digitalWrite(DECODER0, dec_bin[d1][0]);
      digitalWrite(DECODER1, dec_bin[d1][1]);
      digitalWrite(DECODER2, dec_bin[d1][2]);
      digitalWrite(DECODER3, dec_bin[d1][3]);

      digitalWrite(DECODER4, dec_bin[d2][0]);
      digitalWrite(DECODER5, dec_bin[d2][1]);
      digitalWrite(DECODER6, dec_bin[d2][2]);
      digitalWrite(DECODER7, dec_bin[d2][3]);
    }
  // No data -> new line and timestamp
  else{
    
    char timestamp[maxsize];
    strftime(timestamp, maxsize, "%D-%T", &timeinfo);
    values.concat("\n"+String(timestamp));
    
    digitalWrite(DECODER0, dec_bin[10][0]);
    digitalWrite(DECODER1, dec_bin[10][1]);
    digitalWrite(DECODER2, dec_bin[10][2]);
    digitalWrite(DECODER3, dec_bin[10][3]);
    digitalWrite(DECODER4, dec_bin[10][0]);
    digitalWrite(DECODER5, dec_bin[10][1]);
    digitalWrite(DECODER6, dec_bin[10][2]);
    digitalWrite(DECODER7, dec_bin[10][3]);
  }
  // Handle sleep
  if (5 <= TIME_SINCE_VAL){
    TIME_SINCE_VAL = 0;
    esp_light_sleep_start();
    Serial.flush();
    // Light sleep will resume here, wifi is turned off.
    setup_wifi();
  }
}

void config() {
  //TO DO
}