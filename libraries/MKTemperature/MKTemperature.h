#ifndef MKTEMPERATURE_H
#define MKTEMPERATURE_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <MKModbus.h>
#include <MKMath.h>

class MKTemperature {
public:
	MKTemperature(MKModbus &Eurotherm2416);  //Constructor
	~MKTemperature(); //Destructor
	void 	debug(HardwareSerial &Serial=::Serial);
	
	void 	readAll();	
	void 	printAll(HardwareSerial &Serial=::Serial);
	void	printTune(HardwareSerial &Serial);
		
	word	read(); // := readTemperature
	word 	readTemperature();
	word	readPower();
	void 	batch(MKMath &math, byte bufferSize=16);	
	
	boolean set(word newValue);
	boolean available();
	void autoTune(word tuneSetPoint=400);
	void manualTune(word tuneSetPoint=400);
	word manualTuneSetPb(boolean decrease);
	void manualTuneInit();
	
	byte  	getError();	
	float   getTemperature();
	float   getTDeviation();
	float   getPower();
	float   getPDeviation();
	word	getSetPointPower();
	word	getSetPoint();
	
private:
	HardwareSerial* debugSerial;
	HardwareSerial* outputSerial;
	MKModbus* modbus;
	boolean verbose;	
	
	word    setPoint;
	word    setPointTime;   // time of temperature not reaching setpoint
	
	float   Temperature;
	float   TDeviation;
	float   Power; 			// Power on heater (0...100)
	float   PDeviation;

/*	boolean commsResolution;  // True=Integer False=Float
	byte    userPassword;  
	byte    configPassword; */
};
 
#endif
