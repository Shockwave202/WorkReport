 
#include "IRremote.h"

struct irparams_struct irparams; // the irparams instance

//+=============================================================================
// Interrupt Service Routine - Fires every 50uS
// TIMER2 interrupt code to collect raw data.

ISR (TIMER_INTR_NAME) {
    TIMER_RESET_INTR_PENDING; // reset timer interrupt flag if required (currently only for Teensy and ATmega4809)

if(irparams.ir_state==CAPATURE)//Detecting the header of the frame.
/*------------------------
Capature this:
				   _______________
				  ' |
				   |
				  /|\
				   |
___________________|
|<----650US------->|<----650US------>|
----------------------------*/
	  {	if(!digitalRead(10))
	  	   {irparams.ir_high_level_counter=0;//Debounce
		    irparams.ir_low_level_counter++;
		    if(irparams.ir_low_level_counter==11)//50US*12=600US
			   irparams.ir_start_flag=1;//Produce
		    else if(irparams.ir_low_level_counter==14)//50US*14=700US
				   {//Low level exceeds 700US,abort and restart.
				   	irparams.ir_start_flag=0;//Abort
				   	irparams.ir_low_level_counter=0;
				   }
		   }
       else{ irparams.ir_low_level_counter=0;//Debounce
				   irparams.ir_high_level_counter++;
				   if(irparams.ir_high_level_counter==11)//50US*13=650US
					 {
					  if(irparams.ir_start_flag)
				   	    {irparams.ir_start_flag=0;//Consume
					   //volt_low 650US + voltage_high 650US has been capatured,
				       //switch to the next stage for receiving 8-bit data.
				        irparams.ir_state=RECEIVING_DATA;
						irparams.ir_high_level_counter=0;
						    //Clear the counter,prepare for the next stage.
							//Initial variables
							irparams.ir_bit_count=0;	
							irparams.ir_code=0;//receiving buffer.
                             irparams.offset=0;
  
digitalWrite(2, LOW); 

				        }
					  }
				  else if(irparams.ir_high_level_counter==14)
				          irparams.ir_high_level_counter=0;
	       }
	  }

		 

			else if(irparams.ir_state==RECEIVING_DATA)
					{
					 if(digitalRead(10))//The pin is at high level 
					    {irparams.ir_low_level_counter=0;
						 irparams.ir_high_level_counter++;						 
						 if(irparams.ir_high_level_counter==7)//7*50US=350US
						 //High level asserts for 0.4MS represents 0
							{irparams.ir_high_level_counter=0;
							 irparams.ir_code&=~0x80;//Record the sampling result, 
							                   //Clear the MSB.
   digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
   digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW

							 irparams.ir_bit_count++;
							 if(irparams.ir_bit_count==8)					         
							   { irparams.ir_state=CONFIRMATION;
                                 irparams.ir_end_flag=0;
                                 irparams.new_arrival_flag=0;
                               }						
						   else irparams.ir_code>>=1;//Right shift one bit,prepare
						                         //for the arrival of the next bit
							}
						}
					 else {irparams.ir_high_level_counter=0;
					       irparams.ir_low_level_counter++;						   
					 	   if(irparams.ir_low_level_counter==7)//7*50US=350US
						 //Low level asserts for 0.4MS represents 1
							{irparams.ir_low_level_counter=0;
							 irparams.ir_code|=0x80;//Record the sampling result,
							                  //Set the MSB

digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
   digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
							    irparams.ir_bit_count++;
							 if(irparams.ir_bit_count==8)
							   { irparams.ir_state=CONFIRMATION;
                                 irparams.ir_end_flag=0;
                                 irparams.new_arrival_flag=0;
//If the last bit of the receiving data is low,some modification
//is required.
                                irparams.offset=1;
                                }

						   else irparams.ir_code>>=1;//Right shift one bit,prepare
						                         //for the arrival of the next bit
							}
					      }
				}

	
	else if	(irparams.ir_state==CONFIRMATION)
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
			      
			if(digitalRead(10))
			   { 
		       if(irparams.ir_low_level_counter>1&&irparams.ir_low_level_counter<11)
			      //Volt_low is too short,confirmation failure,
			      //abort and restart the mission.
			    { if(irparams.offset==1)
                            {irparams.offset=0;
                             irparams.ir_low_level_counter=0;
                              //No need to switch state
                             }
                          else
                           {
              irparams.ir_state=CAPATURE;
			  irparams.ir_low_level_counter=0;
                            }
			}

			//Check if the volt_high has asserted for approximately 650US
			  //if the voltage level becomes low during this period,
			  //or exceeds the regular time,abort and restart the mission
			  irparams.ir_high_level_counter++;
			  if(irparams.ir_high_level_counter==11)//50US*13=650US
				 irparams.ir_end_flag=1;//Produce
			 else if(irparams.ir_high_level_counter==20)
			     {irparams.ir_end_flag=0;
				  //Time exceed,abort and restart the mission
				  irparams.ir_state=CAPATURE;
				 }	
			   }
			else 
			   {  
	if(irparams.ir_high_level_counter>1&&irparams.ir_high_level_counter<11)
	         //Volt_high is too short,abort.
			 {irparams.ir_state=CAPATURE;
			 irparams.ir_high_level_counter=0;
			 }
					irparams.ir_low_level_counter++;
				 if(irparams.ir_low_level_counter==11)//The receiving pin has being pulled down
					//for at least 0.6MS
					{ if(irparams.ir_end_flag)//Consume
						{ irparams.ir_end_flag=0;
						  irparams.new_arrival_flag=1;
						 //Format confirmed,the data is reliable.
						 irparams.ir_state=CAPATURE;
						 //Switch to the capature stata,start a new cycle.
irparams.ir_low_level_counter=0;
irparams.ir_high_level_counter=0;
//Probe    
 digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
   //digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
//while(1);


						}
					}
			    else if(irparams.ir_low_level_counter==14)
			     {irparams.ir_low_level_counter=0;
				  //Time exceed,abort and restart the mission
				  irparams.ir_state=CAPATURE;
				 }	
			   }	  
		}	


		
}
