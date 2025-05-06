
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

// Button
const int BUTTON = D4;
bool bflag = false;

// Digit values
int Sig;
int d1, d2;
int speeds[] = {22, 23, 24, 25, 26, 27, 25, 24, 22};
int speeds_len = 9;
int huffa = 0;
const int SPEED_LIMIT = 25;
int TIME_SINCE_VAL = 0;
int TIME_HOLD = 1;

// Values for keeping track of flashing the digits
unsigned long ltime, millidif;
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

  pinMode(BUTTON, INPUT);

  // Set sev segs to blank by default
  digitalWrite(DECODER0, dec_bin[10][0]);
  digitalWrite(DECODER1, dec_bin[10][1]);
  digitalWrite(DECODER2, dec_bin[10][2]);
  digitalWrite(DECODER3, dec_bin[10][3]);
  digitalWrite(DECODER4, dec_bin[10][0]);
  digitalWrite(DECODER5, dec_bin[10][1]);
  digitalWrite(DECODER6, dec_bin[10][2]);
  digitalWrite(DECODER7, dec_bin[10][3]);

  Serial.begin(115200);
}

void loop() {

  check_button();
  
  if (bflag){

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
  
  
    if (TIME_SINCE_VAL > TIME_HOLD){
      huffa += 1;
      if (huffa > speeds_len-1){
        Serial.println("here");
        huffa = 0;
        bflag = false;
      }
      TIME_SINCE_VAL = 0;
      TIME_HOLD = int(random(1,4));
    }
    
    d2 = int(speeds[huffa] / 10);
    d1 = speeds[huffa] % 10;

    if ((speeds[huffa] > SPEED_LIMIT) && (flash_flag == 1)){
      digitalWrite(DECODER0, dec_bin[10][0]);
      digitalWrite(DECODER1, dec_bin[10][1]);
      digitalWrite(DECODER2, dec_bin[10][2]);
      digitalWrite(DECODER3, dec_bin[10][3]);
      
      digitalWrite(DECODER4, dec_bin[10][0]);
      digitalWrite(DECODER5, dec_bin[10][1]);
      digitalWrite(DECODER6, dec_bin[10][2]);
      digitalWrite(DECODER7, dec_bin[10][3]);
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
    if(bflag == false){
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
}

void check_button() {
  int read = digitalRead(BUTTON);

  if (read == HIGH) {
    bflag = true;
  }
}

void change_display(int d1, int d2){
  digitalWrite(DECODER0, dec_bin[d2][0]);
  digitalWrite(DECODER1, dec_bin[d2][1]);
  digitalWrite(DECODER2, dec_bin[d2][2]);
  digitalWrite(DECODER3, dec_bin[d2][3]);
  
  digitalWrite(DECODER4, dec_bin[d1][0]);
  digitalWrite(DECODER5, dec_bin[d1][1]);
  digitalWrite(DECODER6, dec_bin[d1][2]);
  digitalWrite(DECODER7, dec_bin[d1][3]);
}