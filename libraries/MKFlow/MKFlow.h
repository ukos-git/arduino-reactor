#ifndef MKFLOW_H
#define MKFLOW_H
 
#include <Arduino.h>
#include <MKMath.h>

class MKFlow {
public:
	MKFlow();  // Constructor
	~MKFlow(); // Destructor
	
	void init(byte in1=A1, byte in2=A2, byte out=11, word maxFlow=6500, float div1 = 10.88, float div2 = 1.0, float lin1=1.0, float lin0=0.0, float lin2=0.0);	
	void debug(HardwareSerial &Serial=::Serial);	
	
	float read();	
	void batch(MKMath &Math, byte bufferSize=64);
	void set(word myValue);

	float getValue();
	float getDeviation();
	float getError();	
	word  getSetPoint();
private:
	void clear(); 		// clear results	
	HardwareSerial* debugSerial;
	boolean verbose;
	float timer;	
	
	byte analogOutPin;
	byte analogInPin1;
	byte analogInPin2;

	float conversionIn1;
	float conversionIn2;
	float conversionOut0;
	float conversionOut1;	
	float conversionOut2;
		
	float value;
	float deviation;
	float error;
	word setPoint;
};
 
#endif
