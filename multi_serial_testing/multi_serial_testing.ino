
int c=0;

const int DECODER0 = D0;
const int DECODER1 = D1;
const int DECODER2 = D2;
const int DECODER3 = D3;

const int DECODER4 = D5;
const int DECODER5 = D8;
const int DECODER6 = D9;
const int DECODER7 = D10;

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
                    {1,0,1,0}};

void setup(){
  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);
  pinMode(DECODER4, OUTPUT);
  pinMode(DECODER5, OUTPUT);
  pinMode(DECODER6, OUTPUT);
  pinMode(DECODER7, OUTPUT);
}

void loop() {
  digitalWrite(DECODER0, dec_bin[10][0]);
  digitalWrite(DECODER1, dec_bin[10][1]);
  digitalWrite(DECODER2, dec_bin[10][2]);
  digitalWrite(DECODER3, dec_bin[10][3]);
  digitalWrite(DECODER4, dec_bin[10][0]);
  digitalWrite(DECODER5, dec_bin[10][1]);
  digitalWrite(DECODER6, dec_bin[10][2]);
  digitalWrite(DECODER7, dec_bin[10][3]);
  delay(1000);
  digitalWrite(DECODER0, dec_bin[8][0]);
  digitalWrite(DECODER1, dec_bin[8][1]);
  digitalWrite(DECODER2, dec_bin[8][2]);
  digitalWrite(DECODER3, dec_bin[8][3]);
  digitalWrite(DECODER4, dec_bin[8][0]);
  digitalWrite(DECODER5, dec_bin[8][1]);
  digitalWrite(DECODER6, dec_bin[8][2]);
  digitalWrite(DECODER7, dec_bin[8][3]);
  c++;
  c = c % 10;
  delay(1000);
}
