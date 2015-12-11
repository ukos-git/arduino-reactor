#include <MKModbus.h>
#include <SoftwareSerial.h>

SoftwareSerial Eurotherm2416(8, 9, true); // convert the ttl by software
MKModbus modbus(Eurotherm2416); //initialize communication class

void setup()
{
  // Open serial communications over USB (PIN 1/2) and wait for port to open
  Serial.begin(9600);
  modbus.debug(Serial);

  // Open Communication with Eurotherm2416 PID Controller for Oven
  Eurotherm2416.begin(9600);
  delay(1000);
}

void loop()
{
  modbus.test();
  Serial.print(modbus.readRegister(199));
  delay(10000);
}
