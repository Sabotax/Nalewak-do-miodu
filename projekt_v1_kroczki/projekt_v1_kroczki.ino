#include "HX711.h"
#include <LiquidCrystal.h>
#include <stdio.h>
#define DOUT  2       //pin 3 Arduino i wyjście DAT czujnika
#define CLK  7          //pin 2 Arduino i wyjście CLK czujnika

#define zawor A4
//#define btn_tare -1
#define btn_tryb 5
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
// ### btn tryb
int btn_tryb_odczyt;
int btn_tryb_stan=0;
int btn_tryb_prev=LOW;
bool btn_tryb_changed=true;
long time_tryb = 0;
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
float tryb_ile_nalac = 0;
float waga_opoznienia = 50;
char waga_string[5];
long moment_pisania = 0;
char msg[32];
void setup() {
  scale.set_scale(calibration_factor);
  scale.tare();         //Resetuje skalę na 0
  
  long zero_factor = scale.read_average();     //Odczyt podstawy

  Serial.begin(9600);
  pinMode(zawor,OUTPUT);
//  pinMode(btn_tare,INPUT_PULLUP);
  pinMode(btn_tryb, INPUT_PULLUP);
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
}

void loop() {
  //btn
  btn_tryb_odczyt = digitalRead(btn_tryb);
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
  
  switch(machine_state_controller) {
    case hold:
    // #### TRYB ###
      if (btn_tryb_odczyt == LOW && btn_tryb_prev == HIGH && millis() - time_tryb > debounce) {
        btn_tryb_stan++;
        if(btn_tryb_stan == 3) btn_tryb_stan = 0;
        btn_tryb_changed = true;
        time_tryb = millis();    
      }
      if( btn_tryb_changed) {
        btn_tryb_changed = false;
        tryb_ile_nalac += 100;
        if(tryb_ile_nalac >= 1600) tryb_ile_nalac = 100;
        lcd.setCursor(0,0);
        lcd.print("Parametr:       ");
        lcd.setCursor(0,0);
        //sscanf(msg,"Parametr:"+%3f "g",(tryb_ile_nalac/1000));
        lcd.print("Parametr:"+ String(tryb_ile_nalac)+"g");
        lcd.print(msg);
      }

      // ### START ###
      if (btn_start_odczyt == LOW && btn_start_prev == HIGH && millis() - time_start > debounce) {
        machine_state_controller = go;
        Serial.print("Start");
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
            Serial.print("Stop_btn");
          }
          if ( waga_odczyt > (tryb_ile_nalac - waga_opoznienia) ) {
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
  btn_tryb_prev = btn_tryb_odczyt;
  btn_start_prev = btn_start_odczyt;
  btn_stop_prev = btn_stop_odczyt;
}
