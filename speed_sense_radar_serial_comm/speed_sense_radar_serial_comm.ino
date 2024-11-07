const int DECODER0 = D0;
const int DECODER1 = D1;
const int DECODER2 = D2;
const int DECODER3 = D3;
 
int speed = 0;
int d1 = 0;
int d2 = 0;

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
  Serial.println("SI");

  // put your setup code here, to run once:
  pinMode(DECODER0, OUTPUT);
  pinMode(DECODER1, OUTPUT);
  pinMode(DECODER2, OUTPUT);
  pinMode(DECODER3, OUTPUT);

}

void loop() {

  speed = Serial.readString().toInt();

  if(speed < 0){
    speed *= -1;
  }

  // put your main code here, to run repeatedly:
  d2 = speed % 10;

  if (speed > 9){
    d1 = speed / 10;
  }

  // set outputs to decoder (only 2nd digit)
  digitalWrite(DECODER0, dec_bin[d2][0]);
  digitalWrite(DECODER1, dec_bin[d2][1]);
  digitalWrite(DECODER2, dec_bin[d2][2]);
  digitalWrite(DECODER3, dec_bin[d2][3]);

}