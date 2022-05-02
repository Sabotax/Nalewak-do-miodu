#include "HX711.h"
 
#define DOUT  2         //pin 3 Arduino i wyjście DAT czujnika
#define CLK  3           //pin 2 Arduino i wyjście CLK czujnika
 
HX711 scale(DOUT, CLK);
 
float calibration_factor = 416600;     //współczynnik kalibracji
//416600
//409910
 
void setup() {
  Serial.begin(9600);
  Serial.println("HX711 - program kalibracyjny");
  Serial.println("Usun obciazenie z belki");
  Serial.println("Po rozpoczeciu odczytu, umiesc znana mase na belce");
  Serial.println("Wcisnij + lub a by zwiekszyc wspolczynnik kalibracji");
  Serial.println("Wcisnij - lub z by zwiekszyc wspolczynnik kalibracji");
 
  scale.set_scale();
  scale.tare();         //Resetuje skalę na 0
 
  long zero_factor = scale.read_average();     //Odczyt podstawy
  Serial.print("Zero factor: ");               //Może być wykorzystane aby usunąć potrzebę tarowania skali. Użyteczne w projektach o stałej skalli
  Serial.println(zero_factor);

  pinMode(LED_BUILTIN, OUTPUT);
}
 
void loop() {
 
  scale.set_scale(calibration_factor);       //Wyrównanie według współczynnika kalibracji
 
  Serial.print("Odczyt: ");
  Serial.print(scale.get_units(), 3);
  Serial.print(" kg");          //Dowolnie możemy wybrać kilogramy lub funty, ponieważ między nimi jest liniowa zależność
  Serial.print("       Wspolczynnik_kalibracji: ");
  Serial.print(calibration_factor);
  Serial.println();
 
  if(Serial.available())
  {
    char temp = Serial.read();                        //pobranie wpisanej wartości i zmiana współczynnika kalibracji
    if(temp == '+')
      calibration_factor += 10;
    else if (temp == 'a') {
      calibration_factor += 500;
    }
    else if (temp == 'z') {
      calibration_factor -= 500;
    }
    else if(temp == '-')
      calibration_factor -= 10;
  }

}
