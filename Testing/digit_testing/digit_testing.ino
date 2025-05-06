
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
                  

int c = 0;
int v = 1;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  //while (!Serial);
  delay(1000);
  
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
}

void loop() {
  
  digitalWrite(DECODER0, dec_bin[c][0]);
  digitalWrite(DECODER1, dec_bin[c][1]);
  digitalWrite(DECODER2, dec_bin[c][2]);
  digitalWrite(DECODER3, dec_bin[c][3]);
  
  digitalWrite(DECODER4, dec_bin[v][0]);
  digitalWrite(DECODER5, dec_bin[v][1]);
  digitalWrite(DECODER6, dec_bin[v][2]);
  digitalWrite(DECODER7, dec_bin[v][3]);
  
  c++;
  v++;
  if (c>9){
    c = 0;
  }
  if (v>9){
    v = 0;
  }
  delay(2000);
  // put your main code here, to run repeatedly:

}
