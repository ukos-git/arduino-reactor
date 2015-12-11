#include <MKMath.h>

MKMath Math;

void setup()
{
  Serial.begin(9600);  
  //Math.debug(Serial);  
}

void loop()
{
  float temp[] = {2.0, 1.0, 2.0, 1.0};
  Math.writeBuffer(temp,4);  
  Math.calc(true);
  Serial.println(Math.standardDeviation());
  Serial.println(Math.average());  
  delay(5000);
}
