#define t1 D8
#define t2 D7

void setup() {
  // put your setup code here, to run once:
  pinMode(t1,OUTPUT);
  pinMode(t2,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(t1,LOW);
  digitalWrite(t2,HIGH);
  delay(5000);
  digitalWrite(t2,LOW);
  digitalWrite(t1,HIGH);
  delay(5000);
}
