#ifndef MKMODBUS_H
#define MKMODBUS_H
 
#include <Arduino.h>
#include <SoftwareSerial.h>

#define rxPinOven 8
#define txPinOven 9

#define  MAX_BUFFER  64


class MKModbus {
public:
	MKModbus(SoftwareSerial &SoftwareRS232) ;  //Constructor
	~MKModbus(); //Destructor
	void deliver(byte *message, byte sizeofMessage);
	void debug(HardwareSerial &Serial);
	void writeBuffer(byte *message, byte messageLength);
	void sendBuffer();
	boolean wait(int timeout=1000);
	boolean sendMessage(int retryTimes=5);
	word readRegister(word input);
	boolean writeRegister(word address, word value);
	boolean test();
	boolean loopback(byte address=1);
private:
	void receiveBuffer();
	boolean checkMessage();
	word calcCRC(byte *message, byte messageLength);
	HardwareSerial* debugSerial;
	SoftwareSerial* mySerial;
	boolean verbose;
	long  lastrun;		//milliseconds on last run.
	byte  outBuffer[MAX_BUFFER]; //This Array will store the message from master
	byte  outBufferSize;         //The current Size of the Filled buffer.
	byte  inBuffer[MAX_BUFFER]; //This Array will receive the message from slave
	byte  inBufferSize;         //The current Size of the Filled buffer.
};
 
#endif
