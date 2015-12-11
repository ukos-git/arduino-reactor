#include "MKUserInput.h" //include the declaration for this class
MKUserInput::MKUserInput()  //construcor
{
	// INIT
	this->verbose = false;
	this->keyByte1 = 0;
	this->keyByte2 = 0;		
	this->clear();
	//this->MAX_INPUT_BUFFER=32 already set in header file
	
	
	// Wait n milliseconds for next readout. 
	// if socat is used, set this to 3ms. Otherwise 0ms would be sufficient.
	this->latencyDelay = 3;
}

MKUserInput::~MKUserInput(){} //destructor

void MKUserInput::setInput(HardwareSerial &Serial)
{
	this->receiveSerial = &Serial;
}

void MKUserInput::debug(HardwareSerial &Serial)
{	
	this->debugSerial   = &Serial;
	if (this->verbose)
	{
		this->verbose=false;
		this->debugSerial->println(F("Input: Verbosity off"));
		return;
	}
	else
	{
		this->verbose=true;
		this->debugSerial->println(F("Input: Verbosity on"));
	}	
}

void MKUserInput::clear()
{
	this->numberStarts = 0;
	this->usedBuffer   = 0;
	this->bufferFilled = false;
}

void MKUserInput::waitfor(byte character1, byte character2)
{	
	this->keyByte1 = (byte)character1;
	if (character2==0)
	{
		// Convert UpperCase to LowerCase...
		this->keyByte2 = this->keyByte1+32;
	}
	else
	{
		this->keyByte2 = (byte)character2;
	}
	
	if (this->verbose)
	{
		this->debugSerial->println();
		this->debugSerial->print(F("KeyByte1 at "));
		this->debugSerial->println(this->keyByte1);
		this->debugSerial->print(F("KeyByte2 at "));
		this->debugSerial->println(this->keyByte2);			
	}	
}

boolean MKUserInput::ready()
{	
	if (this->bufferFilled)
	{
		if (this->verbose) { this->debugSerial->println(F("Buffer filled.")); }
		return this->checkKeyByte();
	}
	else if (this->receiveSerial->available())
	{
		if (this->verbose) { this->debugSerial->println(F("Serial Port available")); }
		this->readBuffer();
		return this->checkKeyByte();
	}
	return false;
}

void MKUserInput::readBuffer()
{
	byte i = 0;

	if (this->verbose)
		{ this->debugSerial->println(F("reading Buffer")); }
	while ((this->receiveSerial->available()) && (i<MAX_INPUT_BUFFER))
	{
		this->incomingBytes[i]=this->receiveSerial->read();
		delay(this->latencyDelay); //Give the buffer some time. socat has high latency.
		i++;
	}
	this->usedBuffer = i;
	this->bufferFilled = true;
}

boolean MKUserInput::checkKeyByte()
{	
	for (byte i=0;i<this->usedBuffer;i++)
	{
		if (this->verbose)
			{
				this->debugSerial->print(F("Comparing Byte "));
				this->debugSerial->print(this->incomingBytes[i]);
				this->debugSerial->println(F(" with keyByte"));
			}		
		// Check each incoming Byte for a Match
		if ((this->incomingBytes[i]==this->keyByte1) || (incomingBytes[i]==this->keyByte2))
			{	
				if (this->verbose)
				{
					this->debugSerial->print(F("Incoming KeyByte at Position "));
					this->debugSerial->println(i);
				}				
				this->numberStarts=i+1;			
				return true; 
			}
	}
	return false;
}

word MKUserInput::receive()
{
	byte i = 0;
	byte numbers[3] = {0,0,0};
	word number = 0;

	// Save the next 3 ASCII Bytes. Numbers are from ASCII 48 to ASCII 58
	for (i=0; i<+3; i++)
		{ numbers[i] = incomingBytes[this->numberStarts+i]-48; } // rollover
	
	// Convert those ASCII Numbers to real numbers.
	if (this->verbose)
	{
		this->debugSerial->println();
		for (i=0; i<+3; i++)
		{ 
			this->debugSerial->print(numbers[i]); 
			this->debugSerial->print(F("\t")); 
		}
		this->debugSerial->println(); 
	}
	while (numbers[2]>9)
	{	
		//Shift while NAN
		numbers[2]=numbers[1];
		numbers[1]=numbers[0];
		numbers[0]=0;              
	}
	if (this->verbose)
	{
		this->debugSerial->println();
		for (i=0; i<+3; i++)
		{ 
			this->debugSerial->print(numbers[i]); 
			this->debugSerial->print(F("\t")); 
		}
		this->debugSerial->println(); 
	}
	
	this->clear();
	if (!((numbers[2]>9) || (numbers[1]>9)))
		{ number = (word)(numbers[2]+numbers[1]*10+numbers[0]*100); }
	else
		{ number = 0; }
	if (this->verbose)
	{ 
		this->debugSerial->print(F("Converted ASCII Number to Binary: "));
		this->debugSerial->println(number);
	}
	return number;
}
