#include "WiFi.h"
#include "time.h"

const int DECODER0 = D0;
const int DECODER1 = D1;
const int DECODER2 = D2;
const int DECODER3 = D3;

char values[1000];
 
int speed, d1, d2;
unsigned long ltime, millidif;

struct tm timeinfo;
size_t maxsize = 20;

const char *ssid = "Verizon-SM-N970U-4a7d";
const char *password = "uqfw684*";

int dec_bin[][4] = {{0,0,0,0},
                    {0,0,0,1},
                    {0,0,1,0},
                    {0,0,1,1},
                    {0,1,0,0},
                    {0,1,0,1},
                    {0,1,1,0},
                    {0,1,1,1},
                    {1,0,0,0},
                    {1,0,0,1}};
 
void setup() {
  Serial.begin(19200);
  while (!Serial);
  delay(5);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");

  configTime(0, 0, "pool.ntp.org");
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);

}

void loop() {
  TimeHandle(setsevseg());
}

void TimeHandle(int speed) {
  ltime = (unsigned long)(millis() - millidif);
  if (ltime >= 1000){
    millidif = millidif + ltime;
    ltime = 0;
    if (59 < ++timeinfo.tm_sec){
      timeinfo.tm_sec = 0;
      if (59 < ++timeinfo.tm_min){
        timeinfo.tm_min = 0;
        timeinfo.tm_hour++;
      }
    }
    char timestamp[20];
    strftime(timestamp, maxsize, "%D %T", &timeinfo);
    Serial.print(timestamp);
    if (speed < 10){
      Serial.print(" ");
      Serial.println(speed);
    }
  }
}

int setsevseg() {
  
  if (Serial.available()){
    speed = Serial.readString().toInt();
  }
  d2 = speed % 10;
  if (speed > 9){
    d1 = speed / 10;
  }
  // set outputs to decoder (only 2nd digit)
  digitalWrite(DECODER0, dec_bin[d2][0]);
  digitalWrite(DECODER1, dec_bin[d2][1]);
  digitalWrite(DECODER2, dec_bin[d2][2]);
  digitalWrite(DECODER3, dec_bin[d2][3]);

  return speed;
}