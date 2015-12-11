#include "MKModbus.h" //include the declaration for this class
//construcor
MKModbus::MKModbus(SoftwareSerial &SoftwareRS232)  
{
  // Open software serial communications at PIN rxPinOven txPinOven and wait for port to open
  this->mySerial = &SoftwareRS232;
  this->lastrun  = ::millis();
  // Init debug Level
  verbose=false;
  
  // Search for device address, perform tests
  
}

//destructor
MKModbus::~MKModbus(){}

void MKModbus::debug(HardwareSerial &Serial)
{
	this->debugSerial   = &Serial;
	if (this->verbose)
	{
		this->verbose=false;
		this->debugSerial->println(F("ModBus: Verbosity off"));
		return;
	}
	else
	{
		this->verbose=true;
		this->debugSerial->println(F("Modbus: Verbosity on"));
	}	
}

boolean MKModbus::test()
{
	word registers[]    = {199, 122, 121, 65528, 18, 199};
	word staticAnswer[] = {0,  9312, 500,  4626,  5,   0};
	word answer[]    = {0, 0, 0, 0, 0, 0};
	boolean compare = true;
	for (byte i=0; i<6; i++)
	{
	  answer[i]=readRegister(registers[i]);
	  if (verbose)
	  {
		  debugSerial->println(F("------------"));
		  debugSerial->print(F("register \t"));
		  debugSerial->println(registers[i]);
		  debugSerial->print(F("value    \t"));
		  debugSerial->println(answer[i]);
		  debugSerial->println(F("------------"));		  
	  }
	  if (answer[i]==staticAnswer[i])
		{compare=false;}
	}
	return compare;
}

/* 
 * Source: Manual Eurotherm HA026230_3_2000
	* Message format:
	* Device address | Function code | Address of first word input | Number of words to read
 * Modbus Device address is set directly  on Eurotherm 2416 (config menu) to be 1	  
 * 
 * It is recommended that function code 3 is used for reads and function code 16 
 * is used for writes. This includes Boolean data. 
 * 
 * The Address word input is split up in to two bytes
 * 
 * The maximum number of words that may be read is 125 
 * for 2400 Series instruments. Here we set it static to 0,1
*/

word MKModbus::readRegister(word input)
{
	byte message[6] = {1, 3, highByte(input), lowByte(input), 0, 1};  
	deliver(message, sizeof(message));
    /*
     * Reply
		* Message format
		* Device address | Function code | Number of bytes in Message | Address of first word input
     * only one slave so we dont check if the message is for us. 
     * We also know that we sent 1 word so we probably receive one word.
     * 
     */
    byte highAnswer = inBuffer[3];
    byte lowAnswer  = inBuffer[4];    
    word answer = word(highAnswer,lowAnswer);
    return answer;
}

boolean MKModbus::writeRegister(word address, word value)
{
	/* 
	 * at device 1 write(16) to the (word) address (word) one (0,1)=1 register long. 
	 * The message, which consists of 2 bytes has the (word) value
	 */
	 
	byte message[9] = {1, 16, highByte(address), lowByte(address), 0, 1, 2, highByte(value), lowByte(value)};  
	deliver(message, sizeof(message));
	
	// check the answer
	word returnedAddress = word(inBuffer[2],inBuffer[3]);
	word countWords      = word(inBuffer[4],inBuffer[5]);
	if ((returnedAddress == address) && (countWords==1))
		{ return true; }
	return false;
	
}

//Checks for active device on given address
boolean MKModbus::loopback(byte address) //address defaults 1
{
	byte message[6] = {address, 8, 0, 0, 12, 34};  
	deliver(message, sizeof(message));
    
    for (int i=0;i<6;i++)
    {
		if (inBuffer[i] != message[i])
			{ return false; }
	}
    return true;
}

//Write Message to Buffer
void MKModbus::writeBuffer(byte *message, byte messageLength)
{
  int i=0;
  for (i=0; i<messageLength; i++)
    { outBuffer[i] = message[i]; }
  //Clear Buffer
  for (i=messageLength; i<MAX_BUFFER; i++)
    { outBuffer[i] = 0; }
  //Calculate CheckSum and Add it to the end of the Message
  word crc = calcCRC(outBuffer, messageLength);
  outBufferSize = messageLength+2;
}

void MKModbus::sendBuffer()
{
  int i=0;
  
  //EOT delay at least: 3.5ms (Check at beginning of new message)
  long timeDelay = 0;  
  long now = millis();
  timeDelay = now-this->lastrun;
  if (!timeDelay<4) { timeDelay=0; }
  ::delay(timeDelay);  
  this->lastrun  = ::millis();
  
  for (i=0; i < outBufferSize; i++)
  {
    this->mySerial->write(outBuffer[i]);
    ::delay(0.1); //give the processor some time
  }
  
  //Display afterwards to avoid delay
  if (verbose)
  {
    debugSerial->print(F("sent["));
    debugSerial->print(outBufferSize);
    debugSerial->print(F("]\t"));
    for (i=0; i < outBufferSize; i++)
    {    
      debugSerial->print(outBuffer[i]);
      debugSerial->print(F("\t"));
    }    
    debugSerial->println();          
  }  
}

//Wait for Answer from Machine
boolean MKModbus::wait(int timeout) 
{
  int latency=0;
  
  if (verbose)
	{ debugSerial->print(F("waiting")); }
  while ((this->mySerial->available()==0)&&(latency<timeout)) 
  {
    latency++;
    delay(1);
    if (verbose)
      { if (latency%10==0) {debugSerial->print(F(".")); }
    }
  }
  if (verbose)
	{ debugSerial->println(); }  
  if (latency==timeout) 
    {return false;}
  return true;
}

// retry n Times until message was sent correctly and device answers.
boolean MKModbus::sendMessage(int retryTimes)
{
  sendBuffer();
  int i=0;
  //retry after timeout
  while ((wait()==false)&&(i<retryTimes))
  {
    i++;
    sendBuffer();
  }
  if (i==retryTimes) 
    {return false;}  
  return true;
}  

//Read Signal (if any)
void MKModbus::receiveBuffer()
{
  inBufferSize = 0; //set Buffer  
  int i=0;
  if (verbose)
  {    
    debugSerial->println();
    debugSerial->print(F("received \t"));
  }
  while(this->mySerial->available())
  {
    inBuffer[i] = this->mySerial->read();
    i++;
    delay(1); //give the internal buffer some time
  }
  //Clear Buffer
  inBufferSize = i;  
  for (i=inBufferSize; i<MAX_BUFFER; i++)
    { inBuffer[i] = 0; }
    
  if (verbose)
  {     
    for (i=0; i<inBufferSize; i++)
    {    
      debugSerial->print(inBuffer[i]);
      debugSerial->print(F("\t"));    
    }
    debugSerial->println();    
  }
}

boolean MKModbus::checkMessage()
{
  receiveBuffer();
  if (calcCRC(inBuffer,inBufferSize)==0)
    {return false;}
  else
    {return true;}
}

/* CRC-Calculation from Eurotherm Manual HA026230_3_2000 */
/* uint16_t is word uint8_t is byte                      */
/* converted for readability to arduino-friedly language */
word MKModbus::calcCRC(byte *message, byte messageLength)
/* CRC runs cyclic Redundancy Check Algorithm on input z_p */
/* Returns value of 16 bit CRC after completion and */
/* always adds 2 crc bytes to message */
/* returns 0 if incoming message has correct CRC */
{
  word CRC= 0xffff;
  word next, carry;
  byte crch, crcl;  //CRC Low and High bytes
  
  for (int i=0; i < messageLength; i++)
  {
    next = (word)*message;
    CRC ^= next;
    for (int n = 0; n < 8; n++) 
    {
      carry = CRC & 1;
      CRC >>= 1;
      if (carry) {CRC ^= 0xA001;}
    }
    message++;
  }
  crch = CRC / 256;
  crcl = CRC % 256;  
  
  //exceeding messageLength in Buffer
  *message = crcl;
  message++;
  *message = crch;
  return CRC;
}

void MKModbus::deliver(byte *message, byte sizeofMessage)
{
  int retry = 0;
  int maxRetry = 2;
  writeBuffer(message, sizeofMessage);    
  for (retry=0; retry<maxRetry; retry++)
  {
	if (sendMessage()==true)
	{
		receiveBuffer();
		if (calcCRC(inBuffer,inBufferSize)==0)
			{ break; }
		else
			{ if (verbose) { debugSerial->println(F("CRC Error")); } }
	}
	else if (verbose) 
	{ 
		debugSerial->print(F("sent failure. Try "));
		debugSerial->print(retry+1);
		debugSerial->print(F(" of "));
		debugSerial->println(maxRetry);
	}
  }
  //Data received. Do something.
}
