#include "HX711.h"
#include <LiquidCrystal.h>
#include <stdio.h>
#define DOUT  2       //pin 3 Arduino i wyjście DAT czujnika
#define CLK  7          //pin 2 Arduino i wyjście CLK czujnika

#define zawor A4
#define btn_start 4
#define btn_stop 3
#define led_zawor 6

LiquidCrystal lcd(8,9,10,11,12,13);
HX711 scale(DOUT, CLK);
 
float calibration_factor = 416600;     //współczynnik kalibracji

int machine_state_controller = 0;
int machine_state_nalewanie_enum = 0;
enum machine_state_controller_enum {
  hold = 0,
  go = 1
};
enum machine_state_nalewanie_enum {
  start = 0,
  go_nalewanie = 1,
  stop_ = 2
};



// ## btn start
int btn_start_odczyt;
int btn_start_prev=LOW;
bool btn_start_changed=false;
long time_start = 0;
// ## btn stop
int btn_stop_odczyt;
int btn_stop_prev=LOW;
bool btn_stop_changed=false;
long time_stop = 0;

long debounce = 200;

float waga_odczyt = 0;
float y_ref = 500;
float waga_opoznienia = 30;
char waga_string[5];
long moment_pisania = 0;
bool change_y_ref_opoznienie = false;

class Communication {
public:
  int aktualne_polecenie=0;// 0 nie lej, 1 lej, 2 tare
  String bufor = "";
  bool order_came=false;
  char temp[50];
  String temp2;
  void interpret() {
    
    if(bufor == "") {
      return;
    }

    Serial.println(bufor);
    bufor.toCharArray(temp,50);
    if( bufor[0] == 'u') {
      // ustawianie y_ref
      sscanf(temp,"%c%d;",&temp2,&y_ref);
      Serial.println("Ustawiam y_ref na" + String(y_ref));
      change_y_ref_opoznienie = true;
    }
    else if (bufor[0] == 'o') {
      //ustawiane opóźnień
      sscanf(temp,"%c%d;",&temp2,&waga_opoznienia);
      Serial.println("Opoznienie: " + String(waga_opoznienia));
      change_y_ref_opoznienie = true;
    }
    else {
      // start lub stop
      if(bufor == "start") {
        aktualne_polecenie = 1;
        order_came=true;
      }
      if(bufor == "stop") {
        aktualne_polecenie = 0;
        order_came=true;
      }
      if(bufor == "tare") {
        aktualne_polecenie = 2;
        order_came=true;
      }
    }
    return;
    
  }  
};
Communication Communication_ob;

void setup() {
  Serial.begin(38400);
  scale.set_scale(calibration_factor);
  scale.tare();         //Resetuje skalę na 0
  
  long zero_factor = scale.read_average();     //Odczyt podstawy
  
  digitalWrite(zawor,HIGH);
  pinMode(zawor,OUTPUT);
  pinMode(btn_start,INPUT_PULLUP);
  pinMode(btn_stop,INPUT_PULLUP);
  pinMode(led_zawor,OUTPUT);

  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Auto nalewanie");
  lcd.setCursor(0,1);
  lcd.print("Daniel Rozycki");
  delay(3000);
  lcd.clear();
  Serial.println("Regulacja trójpołożeniowa - start");
}

void loop() {
  //btn
  btn_start_odczyt = digitalRead(btn_start);
  btn_stop_odczyt = digitalRead(btn_stop);

  // led
  waga_odczyt = (int) (scale.get_units()*1000);
  if(millis() - moment_pisania > 500) {
    lcd.setCursor(0,1);
    lcd.print("Odczyt:         ");
    lcd.setCursor(0,1);
    lcd.print("Odczyt:"+String(waga_odczyt)+"g");
  
    moment_pisania = millis();
  }

  if(change_y_ref_opoznienie) {
    change_y_ref_opoznienie = false;
    lcd.setCursor(0,0);
    lcd.print("Yr:      O:     ");
    lcd.setCursor(0,0);
    lcd.print("Yr:"+String(y_ref)+"O:"+String(waga_opoznienia));
  }

  Communication_ob.bufor = Serial.readStringUntil(';');
  Communication_ob.interpret();
  
  if(Communication_ob.order_came && Communication_ob.aktualne_polecenie==2) {
    Communication_ob.order_came = false;
    scale.tare();
    Serial.println("Taruje wage");
  }
  
  switch(machine_state_controller) {
    case hold:
    // #### TRYB ###

      // ### START ###
      if (btn_start_odczyt == LOW && btn_start_prev == HIGH && millis() - time_start > debounce) {
        machine_state_controller = go;
        time_start = millis();    
      }
      break;
    case go:
      switch(machine_state_nalewanie_enum) {
        case start:
          digitalWrite(led_zawor,HIGH);
          digitalWrite(zawor,LOW);

          scale.tare();
          
          machine_state_nalewanie_enum = go_nalewanie;
          break;
        case go_nalewanie:
          if (btn_stop_odczyt == LOW && btn_stop_prev == HIGH && millis() - time_stop > debounce) {
            machine_state_nalewanie_enum = stop_;
            time_stop = millis();    
          }
          if ( waga_odczyt > (y_ref - waga_opoznienia) ) {
            machine_state_nalewanie_enum = stop_;
          }
          break;
        case stop_:
          digitalWrite(led_zawor,LOW);
          digitalWrite(zawor,HIGH);
          machine_state_nalewanie_enum = start;
          machine_state_controller = hold;
          break;
      }
      break;


  }

  // btn
  btn_start_prev = btn_start_odczyt;
  btn_stop_prev = btn_stop_odczyt;
}
