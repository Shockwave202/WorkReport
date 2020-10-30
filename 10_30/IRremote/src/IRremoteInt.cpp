#include "IRremote.h"

struct irparams_struct irparams; // the irparams instance

//+=============================================================================
// Interrupt Service Routine - Fires every 50uS
// TIMER2 interrupt code to collect raw data.

ISR(TIMER_INTR_NAME)
{
	TIMER_RESET_INTR_PENDING; // reset timer interrupt flag if required (currently only for Teensy and ATmega4809)

	if (irparams.state == CAPATURE) //Detecting the header of the frame.
									/*------------------------
Capature this:
				   _______________
				   |
				   |
				  /|\
				   |
___________________|
|<----650US------->|<----650US------>|
----------------------------*/
	{
		if (!digitalRead(irparams.pin))
		{
			irparams.highLevelCounter = 0; //Debounce
			irparams.lowLevelCounter++;
			if (irparams.lowLevelCounter == 11)		 //50US*12=600US
				irparams.startFlag = 1;				 //Produce
			else if (irparams.lowLevelCounter == 14) //50US*14=700US
			{										 //Low level exceeds 700US,abort and restart.
				irparams.startFlag = 0;				 //Abort
				irparams.lowLevelCounter = 0;
			}
		}
		else
		{
			irparams.lowLevelCounter = 0; //Debounce
			irparams.highLevelCounter++;
			if (irparams.highLevelCounter == 11) //50US*13=650US
			{
				if (irparams.startFlag)
				{
					irparams.startFlag = 0; //Consume
											//volt_low 650US + voltage_high 650US has been capatured,
											//switch to the next stage for receiving 8-bit data.
					irparams.state = RECEIVING_DATA;
					irparams.highLevelCounter = 0;
					//Clear the counter,prepare for the next stage.
					//Initial variables
					irparams.bitCount = 0;
					irparams.code = 0; //receiving buffer.
					irparams.offset = 0;
				}
			}
			else if (irparams.highLevelCounter == 14)
				irparams.highLevelCounter = 0;
		}
	}

	else if (irparams.state == RECEIVING_DATA)
	{
		if (digitalRead(irparams.pin)) //The pin is at high level
		{
			irparams.lowLevelCounter = 0;
			irparams.highLevelCounter++;
			if (irparams.highLevelCounter == 7) //7*50US=350US
			//High level asserts for 0.4MS represents 0
			{
				irparams.highLevelCounter = 0;
				irparams.code &= ~0x80; //Record the sampling result,
										//Clear the MSB.

				irparams.bitCount++;
				if (irparams.bitCount == 8)
				{
					irparams.state = CONFIRMATION;
					irparams.endFlag = 0;
					irparams.receivedFlag = 0;
				}
				else
					irparams.code >>= 1; //Right shift one bit,prepare
										 //for the reception of the next bit
			}
		}
		else
		{
			irparams.highLevelCounter = 0;
			irparams.lowLevelCounter++;
			if (irparams.lowLevelCounter == 7) //7*50US=350US
			//Low level asserts for 0.4MS represents 1
			{
				irparams.lowLevelCounter = 0;
				irparams.code |= 0x80; //Record the sampling result,
									   //Set the MSB
				irparams.bitCount++;
				if (irparams.bitCount == 8)
				{
					irparams.state = CONFIRMATION;
					irparams.endFlag = 0;
					irparams.receivedFlag = 0;
					//If the last bit of the receiving data is low,some modification
					//is required.
					irparams.offset = 1;
				}

				else
					irparams.code >>= 1; //Right shift one bit,prepare
										 //for the arrival of the next bit
			}
		}
	}

	else if (irparams.state == CONFIRMATION)
	{
		/*----------------
Confirm this:
		_____________
		            |
					|
				   \|/
					|
					|_____________
|<----650US-------->|<----650US------>|
----------------------*/
		//Confirm if END compiles the standard KXM013C format,if the confirmation
		//failed,abort the value and restart from the capature state.

		if (digitalRead(irparams.pin))
		{
			if (irparams.lowLevelCounter > 0 && irparams.lowLevelCounter < 11)
			//Volt_low is too short,confirmation failure,
			//abort and restart the mission.
			{
				if (irparams.offset == 1)
				{
					irparams.offset = 0;
					irparams.lowLevelCounter = 0;
					//No need to switch state
				}
				else
				{
					irparams.state = CAPATURE;
					irparams.lowLevelCounter = 0;
				}
			}

			//Check if the volt_high has asserted for approximately 650US
			//if the voltage level becomes low during this period,
			//or exceeds the regular time,abort and restart the mission
			irparams.highLevelCounter++;
			if (irparams.highLevelCounter == 11) //50US*13=650US
				irparams.endFlag = 1;			 //Produce
			else if (irparams.highLevelCounter == 20)
			{
				irparams.endFlag = 0;
				irparams.highLevelCounter = 0;
				//Time exceed,abort and restart the mission
				irparams.state = CAPATURE;
			}
		}
		else
		{
			if (irparams.highLevelCounter > 0 && irparams.highLevelCounter < 11)
			//Volt_high is too short,abort.
			{
				irparams.state = CAPATURE;
				irparams.highLevelCounter = 0;
			}
			irparams.lowLevelCounter++;
			if (irparams.lowLevelCounter == 11) //The receiving pin has being pulled down
			//for at least 0.6MS
			{
				if (irparams.endFlag) //Consume
				{
					irparams.endFlag = 0;
					irparams.receivedFlag = 1;
					//Format confirmed,the data is reliable.
					irparams.state = CAPATURE;
					//Switch to the capature stata,start a new cycle.
					irparams.lowLevelCounter = 0;
					irparams.highLevelCounter = 0;
					//Probe
					//digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
					//digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
					//while(1);
				}
			}
			else if (irparams.lowLevelCounter == 14)
			{
				irparams.lowLevelCounter = 0;
				//Time exceed,abort and restart the mission
				irparams.state = CAPATURE;
			}
		}
	}
}
