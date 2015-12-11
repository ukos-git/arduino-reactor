#include <SoftwareSerial.h>
#include <MKModbus.h>
#include <MKTemperature.h>

SoftwareSerial Eurotherm2416(8, 9, true);   // Init Eurotherm PID Controller on Ports rx,tx,convert the ttl by software
MKModbus modbus(Eurotherm2416);             // Init Modbus Protocol for Eurotherm PID Controller
MKTemperature oven(modbus);                 // Init Oven with attached Eurotherm Controller

void setup()
{
  //INIT
  Serial.begin(9600);
  Eurotherm2416.begin(9600);
}

void loop()
{
  oven.read();
  oven.print(Serial);   
}
