#ifndef MKMATH_H
#define MKMATH_H
 
#include <Arduino.h>

#define  MAX_MATH_BUFFER  64

class MKMath {
public:
	MKMath();  //Constructor
	~MKMath(); //Destructor
	void debug(HardwareSerial &Serial);
	boolean writeBuffer(float *buffer, byte bufferLength);
	void calc(boolean calcDeviation=false);
	float average();
	float standardDeviation();
private:
	void clear(); 		// clear results
	void clearBuffer();
	HardwareSerial* debugSerial;
	boolean verbose;
	boolean calculated;
	
	float mathBuffer[MAX_MATH_BUFFER];
	byte usedBuffer;
	
	float mid;
	float deviation;
};
 
#endif
