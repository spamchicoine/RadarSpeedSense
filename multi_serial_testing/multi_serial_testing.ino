
// Setup mode button pin (temp use for sending sample data)
const int BUTTON0 = D4;
bool bflag = false;

void setup(){
  
  Serial.begin(115200);
  pinMode(BUTTON0, INPUT);

}

void loop(){
  int read = digitalRead(BUTTON0);
  if (read == LOW) {
    Serial.println("Goodbye");
    bflag = true;
  }
  else if (bflag == true && read == HIGH){
    // send sample data
    Serial.println("Hello");
    bflag = false;
  }
  delay(50);
}
