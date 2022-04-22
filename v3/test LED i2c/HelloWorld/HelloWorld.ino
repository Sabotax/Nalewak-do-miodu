#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
// dostępne adresy wg botland: 0x20, 0x27, 0x38 , 0x3F
void setup()
{
  Wire.begin(D2,D1);// start I2c na D2 - SDA, D1 - SCL
	// initialize the LCD
	lcd.begin();

	// Turn on the blacklight and print a message.
	lcd.backlight();
	lcd.print("Hello, world!");
}

void loop()
{
	// Do nothing here...
}
