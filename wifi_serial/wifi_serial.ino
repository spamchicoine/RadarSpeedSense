// Need this for the lower level access to set them up.
#include "WiFi.h"
#include "wifi_setup.h"
#include "string.h"

int speed =0;
int d1 = 0;
int d2 = 0;

// Digit 1 -- LEFT
const int DECODER0 = D0;
const int DECODER1 = D3;
const int DECODER2 = D2;
const int DECODER3 = D1;

// Digit 2 -- RIGHT
const int DECODER4 = D5;
const int DECODER5 = D8;
const int DECODER6 = D9;
const int DECODER7 = D10;

// Setup mode button pin (temp use for sending sample data)
const int BUTTON0 = D4;
bool bflag = false;

// Values for keeping track of time
struct tm timeinfo;
size_t maxsize = 20;
unsigned long ltime, millidif;

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

void setup(){
    // For the USB, just use Serial as normal:
  setup_wifi();
  Serial.begin(115200);

  // Sync time with ntp server
  configTime(0, 0, "pool.ntp.org");
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);
  pinMode(DECODER4, OUTPUT);
  pinMode(DECODER5, OUTPUT);
  pinMode(DECODER6, OUTPUT);
  pinMode(DECODER7, OUTPUT);

  // Set sev segs to blank by default
  digitalWrite(DECODER0, dec_bin[10][3]);
  digitalWrite(DECODER1, dec_bin[10][2]);
  digitalWrite(DECODER2, dec_bin[10][1]);
  digitalWrite(DECODER3, dec_bin[10][0]);
  
  digitalWrite(DECODER4, dec_bin[10][3]);
  digitalWrite(DECODER5, dec_bin[10][2]);
  digitalWrite(DECODER6, dec_bin[10][1]);
  digitalWrite(DECODER7, dec_bin[10][0]);

  pinMode(BUTTON0, INPUT);

}

void loop(){
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
      }
    }
  }

  speed = 0;

  if (Serial.available()){
    String tmp = Serial.readString();
    char x[tmp.length()+1];
    tmp.toCharArray(x, sizeof(x));
    char * pch = strtok(x, ",");
    pch = strtok(NULL,",");
    send_debug(String(pch));
    speed = String(pch).toInt();
    if (speed < 0){
      speed *= -1;
    }
  }
  d2 = 0;
  if (speed > 9){
    d2 = speed/10;
  }
  d1 = speed%10;

  if (speed){
    digitalWrite(DECODER0, dec_bin[d2][3]);
    digitalWrite(DECODER1, dec_bin[d2][2]);
    digitalWrite(DECODER2, dec_bin[d2][1]);
    digitalWrite(DECODER3, dec_bin[d2][0]);
    
    digitalWrite(DECODER4, dec_bin[d1][3]);
    digitalWrite(DECODER5, dec_bin[d1][2]);
    digitalWrite(DECODER6, dec_bin[d1][1]);
    digitalWrite(DECODER7, dec_bin[d1][0]);
    delay(750);
  }
  else{
    digitalWrite(DECODER0, dec_bin[10][3]);
    digitalWrite(DECODER1, dec_bin[10][2]);
    digitalWrite(DECODER2, dec_bin[10][1]);
    digitalWrite(DECODER3, dec_bin[10][0]);
    
    digitalWrite(DECODER4, dec_bin[10][3]);
    digitalWrite(DECODER5, dec_bin[10][2]);
    digitalWrite(DECODER6, dec_bin[10][1]);
    digitalWrite(DECODER7, dec_bin[10][0]);
    delay(50);
  }
}

void checkbutton() {
  int read = digitalRead(BUTTON0);
  if (read == LOW) {
    bflag = true;
  }
  else if (bflag == true && read == HIGH){
    // send sample data
    char timestamp[maxsize];
    strftime(timestamp, maxsize, "%D-%T", &timeinfo);
    send_TestData(String(timestamp)+String(" 99"));

    delay(50);
    bflag = false;
  }
}