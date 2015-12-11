#include "MKMath.h" //include the declaration for this class
MKMath::MKMath()  //construcor
{
	// INIT
	this->verbose = false;		
	this->clear();
}

MKMath::~MKMath(){} //destructor

void MKMath::debug(HardwareSerial &Serial)
{	
	this->debugSerial   = &Serial;
	if (this->verbose)
	{
		this->verbose=false;
		this->debugSerial->println(F("Math: Verbosity off"));
		return;
	}
	else
	{
		this->verbose=true;
		this->debugSerial->println(F("Math: Verbosity on"));
	}	
}

void MKMath::clear()
{
	if (this->verbose){ this->debugSerial->println(F("clearing results.")); }	
	this->calculated = false;	
	this->mid        = 0.0;
	this->deviation  = 0.0;
}

void MKMath::clearBuffer()
{
/*	this->usedBuffer = 0;	
	for (byte i=0;i<MAX_MATH_BUFFER;i++)
		{ mathBuffer[i]=0; }*/
}
boolean MKMath::writeBuffer(float *buffer, byte bufferLength) // used float for compatibility but! can cause Memory Issues.
{
	byte i=0;
	this->mathBuffer = buffer; //copy the pointer only
	this->usedBuffer = bufferLength;
	/* Using Pointers here is much more memory efficient! 
	 * 2MByte with float array is quickly filled.
	 * 
	if (this->verbose)
	{ 
		this->debugSerial->print("Writing Buffer with length="); 
		this->debugSerial->println(bufferLength); 
	}
	//bufferLength should be shorter than MAX_MATH_BUFFER
	if (bufferLength>MAX_MATH_BUFFER) { return false; }
	for (i=0; i<bufferLength; i++)
		{ this->mathBuffer[i] = buffer[i]; }
	//Clear Buffer
	for (i=bufferLength; i<MAX_MATH_BUFFER; i++)
		{ this->mathBuffer[i] = 0.0; }
	if (this->verbose)
	{ 
		for (i=0; i<MAX_MATH_BUFFER; i++)
		{
			this->debugSerial->print(this->mathBuffer[i]); 
			this->debugSerial->print("\t"); 
		}
		this->debugSerial->println(); 
	}	
	this->usedBuffer = bufferLength;
	*/
	this->calculated = false;
	return true;
}

void MKMath::calc(boolean calcDeviation)
{
	float variant = 0.0;
	float sum     = 0.0;
	if (this->calculated = true) { this->clear(); }	
	this->calculated = false;
	if (this->verbose){ this->debugSerial->println(F("Calculation started.")); }
	for (int thisReading = 0; thisReading < this->usedBuffer; thisReading++) 
	{
		sum = sum + mathBuffer[thisReading];
	}
	this->mid = sum/this->usedBuffer;
	if (this->verbose)
	{ 
		this->debugSerial->print(F("Mid: \t")); 
		this->debugSerial->println(this->mid); 
	}
	if (calcDeviation)
	{
		for (int thisReading = 0; thisReading < this->usedBuffer; thisReading++) 
		{
			// Statistical Variant
			variant = variant + (this->mid-this->mathBuffer[thisReading]) * (this->mid-this->mathBuffer[thisReading]);
		}
		// Standard Deviation from previously calculated variant
		this->deviation = sqrt( variant / this->usedBuffer );
		if (this->verbose)
		{ 
			this->debugSerial->print(F("Deviation: \t")); 
			this->debugSerial->println(this->deviation); 
		}		
	}
	this->calculated = true;
	if (this->verbose){ this->debugSerial->println(F("Calculation ended.")); }	
}

float MKMath::average()
{
	if (!this->calculated)
	{
		this->calc();
	}
	
	return this->mid;
}

float MKMath::standardDeviation()  //Do not call average() before this without calling calc(true) before average!
{
	if (!this->calculated)
	{
		this->calc(true);
	}

	return this->deviation;
}
