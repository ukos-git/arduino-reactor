#include <SoftwareSerial.h>
#include <MKModbus.h>
#include <MKTemperature.h>

//SoftwareSerial Eurotherm2416(9, 8, true);   // Init Eurotherm PID Controller on Ports rx,tx,convert the ttl by software
SoftwareSerial Eurotherm2416(9, 8);           // Init Eurotherm PID Controller on Ports rx,tx,convert the ttl by hardware
MKModbus Modbus(Eurotherm2416);               // Init Modbus Protocol for Eurotherm PID Controller
MKTemperature Temperature(Modbus);            // Init Oven with attached Eurotherm Controller

#include <MKFlow.h>
MKFlow Argon;
MKFlow Ethanol;

#include <MKPressure.h>
MKPressure Pressure;

#include <MKUserInput.h>
MKUserInput Input;

#include <MKMath.h>
MKMath Math;

long runtime=0;

void setup()
{
  //INIT
  Serial.begin(9600);
  Eurotherm2416.begin(9600);
  Input.setInput(Serial);
  Pressure.init (A1, A0, 2,  1000, 11.078, 1.02);       // pin in1, pin in2, pin out, max, div1(in), div2(in), div3(out)
//  Ethanol.init  (A3, A2, 11, 6500, 11.0, 1.017, 5.520529461, 234.8957289, 0);    // Ethanol factory default 285mbar 50°C  
  Ethanol.init  (A3, A2, 11, 6817, 11.0, 1.017, 5.467162915, 250.1738099, 0);    // Ethanol 15mbar 32°C
//  Ethanol.init  (A3, A2, 11, 7272, 11.0, 1.017, 5.502342313, 262.5962413, 0);    // Methanol 15mbar 32°C
//  Ethanol.init  (A3, A2, 11, 6709, 11.0, 1.017, 5.464545339, 248.0806556, 0);    // 1Propanol 15mbar 32°C  
//  Ethanol.init  (A3, A2, 11, 7545, 11.0, 1.017, 5.490258893, 280.8848865,0);     // 1Butanol 15mbar 32°C     
  Argon.init    (A5, A4, 5,  2500, 11.0, 1.00, 1.28581846, 19.90550708, 0);

}

void loop()
{ 
  checkinput();
  //wait(1000);  
  //printTime();
  Serial.print((float)millis()/1000.0);
  Temperature.batch(Math,2); //Temperature change is very low and Modbus protocol is slow
  printTemperature(); 
  Pressure.batch(Math,64);
  printPressure();  
  Argon.batch(Math,64);
  printArgon();  
  Ethanol.batch(Math,64);
  printEthanol();
  Serial.println();
  Pressure.check();
  secure();
}

void secure()
{
  if (Pressure.getValue()>980.0)
  { 
      Argon.set(0);
      Ethanol.set(0);
      //Pressure.set(1000);
  }
    
}
void printTime()
{
  int  minute=0;
  byte second=0;
  float time =0.0;
  time   = (float)millis()/1000.0/60.0;
  minute = (int)(time);
  second = (byte)((time-(float)minute)*60);
  Serial.print(minute);
  Serial.print(":");  
  Serial.print(second);  
}

void wait(long millisecond)
{
  long now     = 0;
  long waittime= 0;
  now = millis();
  waittime = millisecond - (now - runtime);
  if (waittime<0) {waittime=0;}
  delay(waittime);
  runtime = now;
}

void printTemperature()
{
  Serial.print("\tTemp\t");
  Serial.print(Temperature.getTemperature());
  Serial.print("\t");
  Serial.print(Temperature.getTDeviation());
  Serial.print("\t");
  Serial.print(Temperature.getError());  
  Serial.print("\t");
  Serial.print(Temperature.getSetPoint());
  
  Serial.print("\tPower\t");  
  Serial.print(Temperature.getPower());
/*  Serial.print("\t");
  Serial.print(Temperature.getPDeviation());
  Serial.print("\t");
  Serial.print(Temperature.getError());
  Serial.print("\t");
  Serial.print(Temperature.getSetPointPower());  */
}

void printPressure()
{
  Serial.print("\tPress\t");  
  Serial.print(Pressure.getValue());
  Serial.print("\t");  
  Serial.print(Pressure.getDeviation());
  Serial.print("\t");
  Serial.print(Pressure.getError());
  Serial.print("\t");
  Serial.print(Pressure.getSetPoint());
}

void printEthanol()
{
  Serial.print("\tEtOH\t");  
  Serial.print(Ethanol.getValue());
  Serial.print("\t");  
  Serial.print(Ethanol.getDeviation());
  Serial.print("\t");
  Serial.print(Ethanol.getError());
  Serial.print("\t");
  Serial.print(Ethanol.getSetPoint());
}

void printArgon()
{
  Serial.print("\tArgon\t");  
  Serial.print(Argon.getValue());
  Serial.print("\t");  
  Serial.print(Argon.getDeviation());
  Serial.print("\t");
  Serial.print(Argon.getError());
  Serial.print("\t");
  Serial.print(Argon.getSetPoint());
}

void checkinput()
{
  Input.waitfor('t','/');  
    if (Input.ready()) { Temperature.set(Input.receive()); }
  Input.waitfor('p','*');  
    if (Input.ready()) { Pressure.set(Input.receive()); }
  Input.waitfor('a','-');  
    if (Input.ready()) { Argon.set(Input.receive()); }
  Input.waitfor('e','+');  
    if (Input.ready()) { Ethanol.set(Input.receive()); }  
  Input.waitfor('d',',');
    if (Input.ready()) 
    { 
      word value = Input.receive();
      switch (value) {
      case 1:
        Temperature.debug(Serial);
      break;
      case 2:
        Pressure.debug(Serial);
      break;
      case 3:
        Ethanol.debug(Serial);
      break;
      case 4:
        Argon.debug(Serial);
      break;
      case 5:
        Input.debug(Serial);
      break;      
      case 6:
        Math.debug(Serial);
      break;
      case 7:
        Modbus.debug(Serial);
      break;      
      }
    } 
  Input.clear();  
}

