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

// Digit values
int speed, d1, d2;
const int SPEED_LIMIT = 25;

int STAMP_FLAG = 0;
int TIME_SINCE_VAL;

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

  // Configure UART0
  Serial.begin(115200);
  while (!Serial);
  delay(1000);


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

  // Update time
  ltime = (unsigned long)(millis() - millidif);
  if (ltime >= 1000){
    millidif = millidif + ltime;
    ltime = 0;
    TIME_SINCE_VAL+=1;
  }

  // Handle data from radar
  if (Serial.available()){

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

    TIME_SINCE_VAL = 0;
  }

  if (TIME_SINCE_VAL > 5){
    digitalWrite(DECODER0, dec_bin[10][0]);
    digitalWrite(DECODER1, dec_bin[10][1]);
    digitalWrite(DECODER2, dec_bin[10][2]);
    digitalWrite(DECODER3, dec_bin[10][3]);
    
    digitalWrite(DECODER4, dec_bin[10][0]);
    digitalWrite(DECODER5, dec_bin[10][1]);
    digitalWrite(DECODER6, dec_bin[10][2]);
    digitalWrite(DECODER7, dec_bin[10][3]);

    TIME_SINCE_VAL = 0;

    esp_light_sleep_start();
    esp_sleep_wakeup_cause_t wakeup_cause;
    wakeup_cause = esp_sleep_get_wakeup_cause();

    Serial.readString();

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