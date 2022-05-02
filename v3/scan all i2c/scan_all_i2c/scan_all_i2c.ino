#include <Wire.h>

uint8_t i2cPins[][2] = {
  {D2, D1}, // Standardowe piny I2C
  {D5, D6},
  {D1, D0}
};

void setup()
{
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}

void loop()
{
  scanAll();
  delay(1000);
}

void scanAll()
{
  for (int x = 0; x < sizeof(i2cPins) / (sizeof(uint8_t) * 2); x++) {
    Serial.print("I2C na pinach SDA: ");
    Serial.print(i2cPins[x][0]);
    Serial.print(",SCL: ");
    Serial.println(i2cPins[x][1]);
    Wire.begin(i2cPins[x][0], i2cPins[x][1]);
    scan();
  }
}

void scan()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
