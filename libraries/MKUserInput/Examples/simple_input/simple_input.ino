#include <MKUserInput.h>

MKUserInput Input;

void setup()
{
  Serial.begin(9600);  
  Input.setInput(Serial);
  //Input.debug(Serial);  
  Input.waitfor('T');
}

void loop()
{ 
  if (Input.ready())
    { Serial.println(Input.receive()); }
}
