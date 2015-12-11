#include <MKFlow.h>

MKFlow Flow;

void setup()
{
  Serial.begin(9600);  
  Flow.debug(Serial);
  Flow.init();
}

void loop()
{
//  Flow.set(); 30V
  Serial.println(Flow.read());
  delay(500);
}
