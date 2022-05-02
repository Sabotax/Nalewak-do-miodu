void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  pinMode(4,OUTPUT);
  Serial.println("Start");
  pinMode(11,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  if( digitalRead(11) == LOW ) {
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(4,HIGH);
  } 
  else {
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(4,LOW);
  }
}
