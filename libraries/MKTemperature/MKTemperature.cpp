#include "MKTemperature.h" //include the declaration for this class
//construcor
MKTemperature::MKTemperature(MKModbus &Eurotherm2416)  
{
	// Open software serial communications at PIN rxPinOven txPinOven and wait for port to open
	this->modbus = &Eurotherm2416;

	// Init debug Level
	this->verbose=false;

	// INIT
	this->Temperature 	= 0.0;
	this->TDeviation  	= 0.0;
	this->Power			= 0.0;
	this->PDeviation  	= 0.0;
	this->setPoint		= 0;	
	this->setPointTime	= 0;

/*	// Read basic values once
	this->commsResolution = (boolean)modbus->readRegister(12550);
	this->userPassword    = modbus->readRegister(514); //nice to know
	this->configPassword  = modbus->readRegister(515); */
}

//destructor
MKTemperature::~MKTemperature(){}

void MKTemperature::debug(HardwareSerial &Serial)
{
	this->debugSerial = &Serial;	
	if (this->verbose)
	{
		this->verbose=false;
		this->debugSerial->println(F("Temperature: Verbosity off"));
		return;
	}
	else
	{
		this->verbose=true;
		this->debugSerial->println(F("Temperature: Verbosity on"));
	}
	// Search for device address, perform tests
	if (this->modbus->loopback(1)==false)
	{
		this->debugSerial->println(F("Loopback Error on Modbus Address 1. Checking other Addresses"));
		// send again, this time with output.
		for (byte address=0;address<256;address++)
		{
			if (this->modbus->loopback(address))
			{
				this->debugSerial->print(F("-!-No Signal on address "));
				this->debugSerial->println(address);
			}
			else
			{
				this->debugSerial->print(F("-!-!- Signal received on address "));
				this->debugSerial->println(address);				
			}
		}
	}
	else
	{
		this->debugSerial->println(F("Verbosity on"));
		this->verbose=true;
	}
}

boolean MKTemperature::set(word newValue) //set setPoint to new Value
{
	word oldValue;
	oldValue = modbus->readRegister(2);

	this->modbus->writeRegister(273, 0); // program state: auto:0 manual:1
	this->modbus->writeRegister(23, 1);  // program state: off:1 run:2 hold:4
	this->modbus->writeRegister(140, 1); // reset time

	if (!this->modbus->writeRegister(2, newValue))
	{
		this->modbus->writeRegister(2, oldValue);
		return false;
	}  
}

void MKTemperature::autoTune(word tuneSetPoint)
{
	this->set(tuneSetPoint);
	this->modbus->writeRegister(270, 1); // tuneE: off:0 on:1
}

void MKTemperature::manualTune(word tuneSetPoint)
{
	this->modbus->writeRegister(8, 0); // Integraltime (ti) off
	this->modbus->writeRegister(9, 0); // Differentialtme (di) off
	this->modbus->writeRegister(18, 0); // High CutBack (Hcb) Auto
	this->modbus->writeRegister(17, 0); // Low CutBack (Lcb) Auto
	
	this->set(tuneSetPoint);	
}

void MKTemperature::manualTuneInit()
{
	this->modbus->writeRegister(6, 1); // init proportional-band
}

word MKTemperature::manualTuneSetPb(boolean decrease) //set setPoint to new Value
{
	word oldValue = 0;
	word newValue = 0;
	
	word ModBusAddress = 6; // set Address for proportional band (pb).
	
	oldValue = this->modbus->readRegister(ModBusAddress);
	if (decrease)
	{
		newValue = oldValue-1;
	}
	else
	{
		newValue = oldValue+1;
	}
	this->modbus->writeRegister(ModBusAddress, newValue); // tuneE: off:0 on:1
	
	return this->modbus->readRegister(ModBusAddress);
}

void MKTemperature::readAll()
{
	this->Temperature  	= this->modbus->readRegister(1);
	this->TDeviation	= 0.0;
	this->Power 		= this->modbus->readRegister(4);
	this->PDeviation	= 0.0;
	this->setPoint     	= this->modbus->readRegister(2);
	this->setPointTime 	= this->modbus->readRegister(139);
}
word MKTemperature::read()
{
	return this->readTemperature();
}

word MKTemperature::readTemperature()
{
	this->Temperature  	= this->modbus->readRegister(1);
	return this->Temperature;
}

word MKTemperature::readPower()
{
	this->Power 		= this->modbus->readRegister(4);
	return this->Power;
}


void MKTemperature::printAll(HardwareSerial &Serial)
{
	this->outputSerial = &Serial;
	this->readAll();
	this->outputSerial->print(this->Temperature);
	this->outputSerial->print("\t");
	this->outputSerial->print(this->Power);
	this->outputSerial->print("\t");	
	this->outputSerial->print(this->setPoint);
	this->outputSerial->print("\t");
	this->outputSerial->print(this->setPointTime);  
}

void MKTemperature::printTune(HardwareSerial &Serial)
{
	this->outputSerial = &Serial;
	this->outputSerial->print(this->modbus->readRegister(6)); //proportional
	this->outputSerial->print("\t");
	this->outputSerial->print(this->modbus->readRegister(8)); //integral
	this->outputSerial->print("\t");	
	this->outputSerial->print(this->modbus->readRegister(9)); //differential
	this->outputSerial->print("\t");
	this->outputSerial->print(this->modbus->readRegister(17)); //lcb
	this->outputSerial->print("\t");
	this->outputSerial->print(this->modbus->readRegister(18)); //hcb
}

void MKTemperature::batch(MKMath &Math, byte bufferSize)
{
	// Clear buffer on exit to avoid memory issues.
	// Store only calculated values
	float buffer1[bufferSize];
	float buffer2[bufferSize];
		
	for (int i=0; i<bufferSize;i++)
	{
		this->readTemperature();
		buffer1[i]=this->getTemperature();
		this->readPower();
		buffer2[i]=this->getPower();
	}
	Math.writeBuffer(buffer1,bufferSize);  
	this->TDeviation  = Math.standardDeviation();
	this->Temperature = Math.average();
	Math.writeBuffer(buffer2,bufferSize);  
	this->PDeviation  = Math.standardDeviation();	
	this->Power       = Math.average();
}

float   MKTemperature::getTemperature()	{ return this->Temperature;	}
float   MKTemperature::getTDeviation()	{ return this->TDeviation;	}
float   MKTemperature::getPower()		{ return this->Power;		}
float   MKTemperature::getPDeviation()	{ return this->PDeviation; }
byte 	MKTemperature::getError() 		{ return 1; } //Currently not possible to readout float values though it should be feasable.
boolean MKTemperature::available() 		{ return this->modbus->loopback(1); }
word MKTemperature::getSetPointPower() 	  		{ return this->modbus->readRegister(3); }
word 	MKTemperature::getSetPoint() 	{ return this->modbus->readRegister(2); }
