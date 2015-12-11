#include <MKPressure.h>

MKPressure Pressure;

void setup()
{
  Serial.begin(9600);
  Pressure.debug(Serial);
  Pressure.set(30);
}

void loop()
{
  Serial.println(Pressure.read());
  Pressure.check();
  delay(500);
}
