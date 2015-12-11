#ifndef MKPRESSURE_H
#define MKPRESSURE_H
 
#include <Arduino.h>
#include <MKMath.h>

class MKPressure {
public:
	MKPressure();  //Constructor
	~MKPressure(); //Destructor
	
	void init(byte in1=A1, byte=A0, byte out=3, float max=1000.0, float div1 = 11.0, float div2 = 1.0);
	void debug(HardwareSerial &Serial=::Serial);	
	
	float read();
	void batch(MKMath &Math, byte bufferSize=64);
	void set(word myValue);
	void check();
	
	float getValue();//read
	float getDeviation();
	float getError();
	word  getSetPoint();
private:
	void clear(); 		// clear results	
	HardwareSerial* debugSerial;
	boolean verbose;
	float timer;	
	
	byte analogInPin1;
	byte analogInPin2;
	byte transistorPin;
	
	float conversionIn1;
	float conversionIn2;
	float maxPressure;		// Needed for Transistor.
	
	// read constantly
	float value;
	float deviation;
	float error;
	word  setPoint;
	boolean transistorOn;
};
 
#endif
