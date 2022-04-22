#define t1_pin 13
#define t2_pin 15

void setup() {
  // put your setup code here, to run once:
  pinMode(t1_pin, OUTPUT);
  pinMode(t2_pin, OUTPUT);

  Serial.begin(9600);
  Serial.println("----");
  Serial.println("start");
}
String bufor = "";
boolean t1 = false;
long t1_time;
boolean t2 = false;
long t2_time;

int oczekiwanie =10000;
void loop() {
  // put your main code here, to run repeatedly:
  bufor = Serial.readStringUntil('\r\n');

  if( bufor == "open" ) {
    Serial.println("otwieram");
    t2 = false;
    t1 = true;
    t1_time = millis();
    digitalWrite(t2_pin,LOW);
    digitalWrite(t1_pin,HIGH);
  } 
  else if (bufor == "close") {
    Serial.println("zamykam");
    t1 = false;
    t2 = true;
    t2_time = millis();
    digitalWrite(t1_pin,LOW);
    digitalWrite(t2_pin,HIGH);
  }
  else if (bufor != "") {
    Serial.println("Nie czaje");
    Serial.println(bufor);
  }

  if( t1 && millis() + oczekiwanie > t1_time) {
    t1 = false;
    digitalWrite(t1_pin,LOW);
  }
  if( t2 && millis() + oczekiwanie > t2_time) {
    t2 = false;
    digitalWrite(t2_pin,LOW);
  }

  delay(50);
}
