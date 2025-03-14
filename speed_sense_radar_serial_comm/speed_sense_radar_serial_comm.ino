#include <Arduino.h>
#include <driver/uart.h>

#include "time.h"
#include "wifi_setup.h"

extern HTTPClient client;
extern WebServer server;

bool isConnectedToWiFi = false;
bool debug = true;
String test_data = "03/12/25-00:47:33 122 11\n";

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
const int SPEED_LIMIT = 25;

int STAMP_FLAG = 0;
int TIME_SINCE_VAL;

// Values for keeping track of time
struct tm timeinfo;
size_t maxsize = 20;
unsigned long ltime, millidif;

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
  
  
  WiFi.mode(WIFI_OFF);
  if(debug) {
    isConnectedToWiFi = connectToWiFi("MS7549", "t88-24O6");
  } else {
    setup_mode();
    while(!isConnectedToWiFi) {
      delay(500);
      server.handleClient();
    }
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

  // Begin Serial connection with Radar
  // If USB CDC on boot is disabled Serial defaults to RX/TX UART
  Serial.begin(115200);
  while (!Serial);
  delay(1000);

  // send_SpeedData(test_data);
  // Sync time with ntp server
  configTime(0, 0, "pool.ntp.org");
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char timestamp[20];
  strftime(timestamp, 20, "%D-%T", &timeinfo);
  values.concat(String(timestamp));

  // GPIO 20 is the default UART_0 RX pin
  gpio_sleep_set_direction(GPIO_NUM_20, GPIO_MODE_INPUT);
  gpio_sleep_set_pull_mode(GPIO_NUM_20, GPIO_PULLUP_ONLY);

  uart_set_wakeup_threshold(UART_NUM_0, 3);   // 3 edges on U0RXD to wakeup
  esp_sleep_enable_uart_wakeup(UART_NUM_0);   // Enable UART 0 as wakeup source

  gpio_sleep_set_pull_mode(GPIO_NUM_3, GPIO_PULLUP_ONLY);
  gpio_sleep_set_pull_mode(GPIO_NUM_4, GPIO_PULLDOWN_ONLY);
  gpio_sleep_set_pull_mode(GPIO_NUM_5, GPIO_PULLUP_ONLY);
  gpio_sleep_set_pull_mode(GPIO_NUM_2, GPIO_PULLDOWN_ONLY);

  gpio_sleep_set_pull_mode(GPIO_NUM_10, GPIO_PULLUP_ONLY);
  gpio_sleep_set_pull_mode(GPIO_NUM_9, GPIO_PULLDOWN_ONLY);
  gpio_sleep_set_pull_mode(GPIO_NUM_8, GPIO_PULLUP_ONLY);
  gpio_sleep_set_pull_mode(GPIO_NUM_7, GPIO_PULLDOWN_ONLY);

}

void loop() {
  
  ltime = (unsigned long)(millis() - millidif);

  if (ltime >= 1000){
    millidif = millidif + ltime;
    ltime = 0;
    TIME_SINCE_VAL+=1;

    if (59 < ++timeinfo.tm_sec){
      timeinfo.tm_sec = 0;

      TIME_SINCE_VAL = 0;

      if (values.length() > 0){
        values.concat("\n");
        send_SpeedData(values);
        values = String("");
      }
      STAMP_FLAG = 1;

      if (59 < ++timeinfo.tm_min){

        timeinfo.tm_min = 0;
        configTime(0, 0, "pool.ntp.org");

        if(!getLocalTime(&timeinfo)){
          Serial.println("Failed to obtain time");
          return;
        }

        TIME_SINCE_VAL = 0;
        if (values.length() > 0){
          values.concat("\n");
          send_SpeedData(values);
          values = String("");
        }
        STAMP_FLAG = 1;
      }
    }
  }
  
  flash_time = (unsigned long)(millis() - flash_millidif);

  if (flash_time >= 200){
        flash_millidif = flash_millidif + flash_time;
        flash_time = 0;
        flash_flag = (flash_flag + 1) % 2;
  }
  
  // Handle data from radar
  if (Serial.available()){

    // returns 0 if invalid, radar will have minimum threshold above this anyways
    speed = Serial.readString().toInt();

    if (STAMP_FLAG == 1){
      char timestamp[20];
      strftime(timestamp, 20, "%D-%T", &timeinfo);
      values.concat(String(timestamp));
      STAMP_FLAG = 0;
    }
    
    // assign digit values
    if (0 == speed){
      d1 = 10;
      d2 = 10;
    }
    else{
      d1 = speed % 10;
      d2 = speed / 10;
    }

    // Add reading to values
    values.concat(" "+String(speed));
    TIME_SINCE_VAL = 0;
  } 

  if (TIME_SINCE_VAL <= 3){
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

    if (values.length() > 0){
      values.concat("\n");
      send_SpeedData(values);
      values = String("");
    }
    
    STAMP_FLAG = 1;

    //esp_light_sleep_start();
  }
}