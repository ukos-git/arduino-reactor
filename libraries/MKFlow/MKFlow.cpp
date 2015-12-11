#include "MKFlow.h" //include the declaration for this class
MKFlow::MKFlow()  //construcor
{
	// INIT
	this->verbose = false;
	this->clear();
	// Statistic Smoothing
	this->timer = 1.0;          /* time delay after ReadOut in ms (minimum R/O 0.1ms)	
                                 * wait for the analog-to-digital converter to settle after last reading	*/		
}

MKFlow::~MKFlow(){} //destructor

void MKFlow::debug(HardwareSerial &Serial)
{	
	this->debugSerial   = &Serial;
	if (this->verbose)
	{
		this->verbose=false;
		this->debugSerial->println(F("Flow: Verbosity off"));
		return;
	}
	else
	{
		this->verbose=true;
		this->debugSerial->println(F("Flow: Verbosity on"));
	}	
}

void MKFlow::init(byte in1, byte in2, byte out, word maxFlow, float div1, float div2, float lin1, float lin0, float lin2)
{
	float conversion = 0.0;
	
	// static	
	this->analogOutPin = out;
	this->analogInPin1 = in1;
	this->analogInPin2 = in2;		
	
	// Voltage-Reference for the digitalisation of analog input voltages
	::analogReference(INTERNAL);    		// Sets internal Reference for analog input from 5V to 1.1V				
	::pinMode(this->analogOutPin, OUTPUT); 	// Sets Pin for digital Output. PWM-Signal flattened with capacitor.	
	
	//calculate conversion factor once.	
	conversion = (1.1 / 1023.0) / 10.00 * (float) maxFlow;
	this->conversionIn1 = conversion * div1;
	this->conversionIn2 = conversion * div2;	
	
	conversion =  1.0 / (float) maxFlow * 10.0 / 5.0 * 255.0;
	this->conversionOut0 = conversion * lin0;	
	this->conversionOut1 = conversion * lin1;	
	this->conversionOut2 = conversion * lin2;
}

void MKFlow::clear()
{
	if (this->verbose){ this->debugSerial->println(F("clear")); }	
	this->value    = 0.0; // flow in sccm or mg min^-1
	this->error    = 0.0;	
	this->setPoint = 0;
}

float MKFlow::read()
{
	word sensor   = 0;
	float conversion = 0.0;
	
	sensor  = ::analogRead(this->analogInPin1);
	conversion = this->conversionIn1;
/*
	if (this->verbose)
	{ 
		this->debugSerial->print(F("Sensor1 = \t")); 
		this->debugSerial->print(sensor); 	
		this->debugSerial->print(F("\t"));
	}
		*/
	if (sensor<90)
    {
		//measurement of Pin2 only when below 1.1V  		
		sensor 		= ::analogRead(this->analogInPin2);    
		conversion 	= this->conversionIn2;
		/* if (this->verbose)
		{		
			this->debugSerial->print(F("Sensor2 = \t"));
			this->debugSerial->print(sensor);		
			this->debugSerial->print(F("\t"));			
		}	*/	
    }    
    ::delay(this->timer);	
    
    /* convert Flow to sccm or mg/min
     * voltage on Sensor converted from internal reference and the used divider.  */

    this->value = (float)sensor * conversion;
    this->error =       1.0     * conversion;  // 1 DIGIT abs. Error from analog->digital conversion
    
	return this->value;
}

void MKFlow::set(word myValue)
{
	byte setValue;
	word temp;
	if 
		(myValue == 0) { temp = 0; }	//Dont Calculate low PWM Values.
	else
		{ temp = (word) ((float)this->conversionOut0 + (float)myValue*(float)this->conversionOut1 + (float)myValue * (float)myValue * (float)this->conversionOut2); }
	if (temp>255) { temp=0; myValue=0;}  //avoid rollover, max at 255.
	setValue = (byte) temp;
	
	this->setPoint = myValue;
	if (this->verbose)
	{ 
		this->debugSerial->println();
		this->debugSerial->print(F("Flow: PWM Value = ")); 
		this->debugSerial->println(setValue);
		this->debugSerial->print(F("Flow: word setValue = ")); 
		this->debugSerial->println(myValue); 			
	}
	::analogWrite(this->analogOutPin, setValue);
}

void MKFlow::batch(MKMath &Math, byte bufferSize)
{
	// Clear buffer on exit to avoid memory issues.
	// Store only calculated values
	float buffer1[bufferSize];
	float buffer2[bufferSize];
		
	for (int i=0; i < bufferSize; i++)
	{
		this->read();
		buffer1[i]=this->getValue();
		buffer2[i]=this->getError();
	}
	Math.writeBuffer(buffer1,bufferSize);  
	this->deviation  = Math.standardDeviation();
	this->value = Math.average();
	Math.writeBuffer(buffer2,bufferSize);  
	this->error       = Math.average();
}

float 	MKFlow::getValue() 		{ return this->value; 		}
float   MKFlow::getDeviation() 	{ return this->deviation; 	}
float	MKFlow::getError() 		{ return this->error; 		}
word 	MKFlow::getSetPoint() 	{ return this->setPoint; 	}
