#include "MKPressure.h" //include the declaration for this class
MKPressure::MKPressure()  //construcor
{
	// INIT
	this->verbose = false;	
	// Statistic Smoothing
	this->timer = 0.5;          /* time delay after ReadOut in ms (minimum R/O 0.1ms)	
                                 * wait for the analog-to-digital converter to settle after last reading	*/	
	this->clear();
}

MKPressure::~MKPressure(){} //destructor

void MKPressure::debug(HardwareSerial &Serial)
{	
	this->debugSerial   = &Serial;
	if (this->verbose)
	{
		this->verbose=false;
		this->debugSerial->println(F("Pressure: Verbosity off"));
		return;
	}
	else
	{
		this->verbose=true;
		this->debugSerial->println(F("Pressure: Verbosity on"));
	}	
}

void MKPressure::clear()
{
	if (this->verbose){ this->debugSerial->println(F("clear")); }
	this->transistorOn = false;
	this->value    = 0.0; //pressure in mbar
	this->error    = 0.0;
	this->setPoint = 0;	
}

void MKPressure::init(byte in1, byte in2, byte out, float max, float div1, float div2)
{
	float conversion = 0.0;
	
	this->transistorPin= out;   		// On/Off Pin for 30V stearing valver
	this->analogInPin1 = in1;  			// Analog input für Spannungsteiler 1:10
	this->analogInPin2 = in2;  			// Analog input für Spannung vor Diode	
	
	// Voltage-Reference for the digitalisation of analog input voltages
	// ::analogReference(DEFAULT);  // Sets internal Reference for analog input to 5V
	::analogReference(INTERNAL);    // Sets internal Reference for analog input from 5V to 1.1V	
	::pinMode(this->transistorPin, OUTPUT);
	
	//calculate conversion factor once.	
	conversion = (1.1 / 1023.0)  / 10.00 * max;
	
	this->conversionIn1 = conversion * div1;
	this->conversionIn2 = conversion * div2;
	this->maxPressure   = max;
	// set Transistor state off
	this->setPoint		= max;
	this->transistorOn  = false;
}	

float MKPressure::read()
{
	word sensor   = 0;
	float conversion = 0.0;
	
	sensor  = ::analogRead(this->analogInPin1);
	conversion = this->conversionIn1;
/*
	if (this->verbose)
	{ 
		this->debugSerial->print(F("Sensor1 = ")); 
		this->debugSerial->println(sensor); 	
	}
*/		
	if (sensor<90)
    {
		//measurement of Pin2 only when below 1.1V  		
		sensor 		= ::analogRead(this->analogInPin2);    
		conversion 	= this->conversionIn2;
/*		if (this->verbose)
		{		
			this->debugSerial->print(F("Sensor2 = "));
			this->debugSerial->println(sensor);		
		}		
*/
    }    
    ::delay(this->timer);	
    
    /* convert Voltage to mbar    
     * voltage on Sensor converted from internal reference and the used divider.
     * conversion factor of 100.00 for Balzers Manometer. */

    this->value = (float)sensor * conversion;
    this->error =       1.0     * conversion;  // 1 DIGIT abs. Error from analog->digital conversion
    
	return this->value;
}

void MKPressure::set(word myValue)
{
	this->setPoint = myValue;
	this->check();
}

void MKPressure::check()
{
	this->read(); //read out current pressure
	
	/*if (this->verbose)
	{ 
		this->debugSerial->print(F("current setPoint = \t")); 
		this->debugSerial->println(this->setPoint);
		this->debugSerial->print(F("current pressure = \t")); 
		this->debugSerial->println(this->value); 		
	}*/
	if ((this->value > this->setPoint) && (!this->transistorOn))
	{
		::digitalWrite(this->transistorPin, HIGH);
		this->transistorOn = true;
		if (this->verbose){ this->debugSerial->println(F("Valve State: \tHIGH")); }
	}
	else if ((!(this->value > this->setPoint)) && (this->transistorOn))
	{ 
		::digitalWrite(this->transistorPin, LOW);
		this->transistorOn = false;
		if (this->verbose){ this->debugSerial->println(F("Valve State: \tLOW")); }
	}
}

void MKPressure::batch(MKMath &Math, byte bufferSize)
{
	// Clear buffer on exit to avoid memory issues.
	// Store only calculated values
	float buffer1[bufferSize];
	float buffer2[bufferSize];
		
	for (int i=0; i<bufferSize;i++)
	{

		this->read();
		buffer1[i]=this->getValue();
		buffer2[i]=this->getError();
	}
	Math.writeBuffer(buffer1,bufferSize);  
	this->deviation = Math.standardDeviation();
	this->value 	= Math.average();
	Math.writeBuffer(buffer2,bufferSize);  
	this->error		= Math.average();
}

float 	MKPressure::getValue() 		{ return this->value; 		}
float   MKPressure::getDeviation() 	{ return this->deviation; 	}
float	MKPressure::getError() 		{ return this->error; 		}
word 	MKPressure::getSetPoint() 	{ return this->setPoint; 	}
