#include "WiFi.h"
#include "time.h"
#include "wifi_setup.h"

// Digit 1
const int DECODER0 = D0;
const int DECODER1 = D1;
const int DECODER2 = D2;
const int DECODER3 = D3;

// Digit 2
const int DECODER4 = D5;
const int DECODER5 = D8;
const int DECODER6 = D9;
const int DECODER7 = D10;

// Setup mode button pin (temp use for sending sample data)
const int BUTTON0 = D4;
bool bflag = false;

// Values to send to client
String values = String("");

// Sample values to demo webapp graphin
String sample_data = String(""); // TO DO

// Digit values
int speed, d1, d2;
const int SPEED_LIMIT = 25;

// Values for keeping track of time
struct tm timeinfo;
size_t maxsize = 20;
unsigned long ltime, millidif;

// Values for keeping track of flashing the digits
unsigned long flash_time, flash_millidif
int flash_flag = 0;

// Hardcoded values for WiFi setup
const char *ssid = "Verizon-SM-N970U-4a7d";
const char *password = "uqfw684*";

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
  Serial.begin(19200);
  while (!Serial);
  delay(5);

  // Begin WiFi connection with client
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");

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

  // Set button
  pinMode(BUTTON0, INPUT);

  // Start radar hibernate mode
  //Serial.print("Z+")
}

void loop() {
  checkbutton();

  ltime = (unsigned long)(millis() - millidif);

  // Update time
  if (ltime >= 1000){
    millidif = millidif + ltime;
    ltime = 0;

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
        values = String("")
        char timestamp[maxsize];
        strftime(timestamp, maxsize, "%D %T", &timeinfo);
        values.concat(String(timestamp))
      }
    }
  }
  // Handle data from radar
  if (Serial.available()){
    speed = Serial.readString().toInt();
    values.concat(String(" "+String(speed)))

    // Set digits
    flash_time = (unsigned long)(millis() - flash_millidif);

    if (flash_time >= 50){
        flash_millidif = flash_millidif + flash_time;
        flash_time = 0;
        flash_flag = (flash_flag + 1) % 2;
    }
    if (flash_flag == 1) {
      d1 = speed % 10;
      digitalWrite(DECODER0, dec_bin[10][0]);
      digitalWrite(DECODER1, dec_bin[10][1]);
      digitalWrite(DECODER2, dec_bin[10][2]);
      digitalWrite(DECODER3, dec_bin[10][3]);
      if (speed > 9){
        d2 = speed / 10;
        digitalWrite(DECODER4, dec_bin[d2][0]);
        digitalWrite(DECODER5, dec_bin[d2][1]);
        digitalWrite(DECODER6, dec_bin[d2][2]);
        digitalWrite(DECODER7, dec_bin[d2][3]);
      }
    }
    else {
      if (speed > 9){
        d2 = speed / 10;
        digitalWrite(DECODER4, dec_bin[10][0]);
        digitalWrite(DECODER5, dec_bin[10][1]);
        digitalWrite(DECODER6, dec_bin[10][2]);
        digitalWrite(DECODER7, dec_bin[10][3]);
      }
      digitalWrite(DECODER4, dec_bin[d1][0]);
      digitalWrite(DECODER5, dec_bin[d1][1]);
      digitalWrite(DECODER6, dec_bin[d1][2]);
      digitalWrite(DECODER7, dec_bin[d1][3]);
    }
  }
  // No data -> new line and timestamp
  else{
    char timestamp[maxsize];
    strftime(timestamp, maxsize, "%D %T", &timeinfo);
    values.concat(String("\n"+timestamp))

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

void checkbutton() {
  int read = digitalREAD(BUTTON0);
  if (read == LOW) {
    bflag = true;
  }
  else if (blfag == true && read == HIGH){
    // send sample data
    send_SpeedData(sample_data);
    bflag = false;
  }
  
}
