void setup() {
  Serial.begin(19200);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
}

void loop() {
  if ((digitalRead(10) == 1) || (digitalRead(11) == 1)){
    Serial.println('0');
  }else{
    Serial.println(analogRead(A0));
  }
  delay(16);
}