#include "HX711.h"
#include <stdio.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define DOUT  2      //pin 3 Arduino i wyjście DAT czujnika
#define CLK  3          //pin 2 Arduino i wyjście CLK czujnika

#define zawor 4
#define btn_start 11
#define btn_stop 12

LiquidCrystal_I2C lcd(0x27, 16, 2);

HX711 scale(DOUT, CLK);

SoftwareSerial Serial_ESP(A1,A0); // A1 RX | A0 TX
 
float calibration_factor = 409910;     //współczynnik kalibracji

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
int btn_start_prev=HIGH;
bool btn_start_changed=false;
long time_start = 0;
// ## btn stop
int btn_stop_odczyt;
int btn_stop_prev=HIGH;
bool btn_stop_changed=false;
long time_stop = 0;

long debounce = 200;
boolean DEBUG = true;
float waga_odczyt = 0;
int y_ref = 1000;
int waga_opoznienia = 30;
char waga_string[5];
long moment_pisania = 0;
bool change_y_ref_opoznienie = true;
bool show_calib_factor = false;
long show_calib_factor_time = 0;
int temp_factor;

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

    //Serial.println(bufor);
    bufor.toCharArray(temp,50);
    if( bufor[0] == 'y') {
      // ustawianie y_ref
      sscanf(temp,"%c%d;",&temp2,&y_ref);
      Serial.println("Ustawiam y_ref na " + String(y_ref));
      change_y_ref_opoznienie = true;
    }
    else if (bufor[0] == 'o') {
      //ustawiane opóźnień
      sscanf(temp,"%c%d;",&temp2,&waga_opoznienia);
      Serial.println("Opoznienie: " + String(waga_opoznienia));
      change_y_ref_opoznienie = true;
    }
    else if (bufor[0] == 's') {
      //set calibration factor
      sscanf(temp,"%c%d;",&temp2,(long) &calibration_factor);
      Serial.println("Calib_factor: " + String(calibration_factor));
      show_calib_factor_time = millis();
      show_calib_factor = true;
    }
    else if (bufor[0] == 'c') {
      //show calibration factor
      show_calib_factor_time = millis();
      show_calib_factor = true;
    }
    else {
      // start lub stop
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
  Wire.begin();
  Serial.begin(38400);
  Serial_ESP.begin(38400);
  scale.set_scale(calibration_factor);
  scale.tare();         //Resetuje skalę na 0
  
  long zero_factor = scale.read_average();     //Odczyt podstawy
  
  digitalWrite(zawor,HIGH);
  pinMode(zawor,OUTPUT);
  pinMode(btn_start,INPUT_PULLUP);
  pinMode(btn_stop,INPUT_PULLUP);
  pinMode(LED_BUILTIN,OUTPUT);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Nalewak v3");
  lcd.setCursor(0,1);
  lcd.print("Daniel Rozycki");
  delay(3000);
  lcd.clear();
  Serial.println("Nalewak - start");
}

void loop() {
  // led
  waga_odczyt = (int) (scale.get_units()*1000);


// debug
  if( digitalRead(btn_start) == LOW || digitalRead(btn_stop) == LOW) {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN,LOW);
  }

  if(change_y_ref_opoznienie) {
    change_y_ref_opoznienie = false;
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("Yr:"+String(y_ref)+" |O:"+String(waga_opoznienia));
  }

  Communication_ob.bufor = Serial_ESP.readStringUntil(';');
  Communication_ob.interpret();
  
//  if(Communication_ob.order_came && Communication_ob.aktualne_polecenie==2) {
//    Communication_ob.order_came = false;
//    scale.tare();
//    if (DEBUG) Serial.println("Taruje wage");
//  }
  
  switch(machine_state_controller) {
    case hold:
      // ### START ###
      if (digitalRead(btn_start) == LOW ) {
        machine_state_controller = go;   
      }
      break;
    case go:
      switch(machine_state_nalewanie_enum) {
        case start:
          digitalWrite(zawor,LOW);
          scale.tare();
          if (DEBUG) Serial.println("Rozpoczynam nalewanie");
          machine_state_nalewanie_enum = go_nalewanie;
          break;
        case go_nalewanie:
          if (digitalRead(btn_stop) == LOW) {
            machine_state_nalewanie_enum = stop_;   
          }
          if ( waga_odczyt > (y_ref - waga_opoznienia) ) {
            machine_state_nalewanie_enum = stop_;
          }
          break;
        case stop_:
          digitalWrite(zawor,HIGH);
          machine_state_nalewanie_enum = start;
          machine_state_controller = hold;
          if (DEBUG) Serial.println("Kończę nalewanie");
          break;
      }
      break;
  }

  if(millis() - moment_pisania > 300) {
    waga_odczyt = (int) (scale.get_units()*1000);
    lcd.setCursor(0,1);
    lcd.print("Odczyt:         ");
    lcd.setCursor(0,1);
    lcd.print("Odczyt:"+String(waga_odczyt)+"g");
  
    moment_pisania = millis();
  }
}
