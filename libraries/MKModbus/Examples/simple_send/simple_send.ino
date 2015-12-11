#include <MKModbus.h>
#include <SoftwareSerial.h>

SoftwareSerial Eurotherm2416(8, 9, true); // arguments: (rx,tx,convert the ttl by software)
MKModbus modbus(Eurotherm2416); //initialize communication class

void setup()
{
  Serial.begin(9600);
  // Open Communication with Eurotherm2416 PID Controller for Oven
  Eurotherm2416.begin(9600);
  delay(1000);
}

void loop()
{
  Serial.print(modbus.readRegister(199));
  delay(1000);
}
