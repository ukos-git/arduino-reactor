#ifndef MKUSERINPUT_H
#define MKUSERINPUT_H
 
#include <Arduino.h>

#define  MAX_INPUT_BUFFER  8

class MKUserInput {
public:
	MKUserInput();  //Constructor
	~MKUserInput(); //Destructor
	void waitfor(byte character1, byte character2=0); // set char1 to upper case Character by default: character1='T'
	word receive();
	void setInput(HardwareSerial &Serial=::Serial);
	void debug(HardwareSerial &Serial=::Serial);
	void clear();	
	boolean ready();
private:
	void readBuffer();
	boolean checkKeyByte();
	HardwareSerial* debugSerial;
	HardwareSerial* receiveSerial;
	boolean verbose;
	
	// The Keywords to search in Buffer
	byte keyByte1;
	byte keyByte2;
	
	byte incomingBytes[MAX_INPUT_BUFFER];
	byte usedBuffer;
	boolean bufferFilled;	
	byte numberStarts;
	byte latencyDelay;
};
 
#endif
