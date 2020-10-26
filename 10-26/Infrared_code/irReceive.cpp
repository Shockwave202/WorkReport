#include "IRremote.h"


//+=============================================================================
IRrecv::IRrecv() {//Constructor
pinMode(10,INPUT);
}
//--------------------------------
// initialization
//
#ifdef USE_DEFAULT_ENABLE_IR_IN
void IRrecv::enableIRIn() {

pinMode(2, OUTPUT);
digitalWrite(2, HIGH);   


// the interrupt Service Routine fires every 50 uS
    noInterrupts();
    // Setup pulse clock timer interrupt
    // Prescale /8 (16M/8 = 0.5 microseconds per tick)
    // Therefore, the timer interval can range from 0.5 to 128 microseconds
    // Depending on the reset value (255 to 0)
    timerConfigForReceive();

    // Timer2 Overflow Interrupt Enable
    TIMER_ENABLE_RECEIVE_INTR;

    TIMER_RESET_INTR_PENDING;

    interrupts();

	 irparams.ir_state=CAPATURE;
	 irparams.ir_high_level_counter=0;
	 irparams.ir_low_level_counter=0;
	 irparams.ir_start_flag=0;
	 irparams.ir_end_flag=0;
	 irparams.ir_code=0;
     irparams.ir_data_code=0;
     irparams.ir_custom_code=0;
     irparams.ir_data_selflock=0; 
	 irparams.ir_bit_count=0;
         pinMode(10, INPUT);
         pinMode(5,OUTPUT);
         pinMode(3,OUTPUT);
         pinMode(4,OUTPUT);
         digitalWrite(5,LOW);
         digitalWrite(3,LOW);
         digitalWrite(4,LOW);

}

void IRrecv::disableIRIn() {
    TIMER_DISABLE_RECEIVE_INTR;
}

#endif // USE_DEFAULT_ENABLE_IR_IN

//+=============================================================================
// Restart the ISR state machine
//
void IRrecv::resume() {

    if(irparams.new_arrival_flag)
        {irparams.new_arrival_flag=0;
        irparams.ir_data_code= irparams.ir_code & 0x1f;//Clear the highest 3 bits of the receiving data
        irparams.ir_custom_code= irparams.ir_code >> 5;//Right shift 5 bits.
        //Serial.print("DATA CODE:");
        //Serial.println(irparams.ir_data_code,HEX);
        //Serial.print("CUSTOM CODE:");
        //Serial.println(irparams.ir_custom_code,HEX);
        Serial.println(irparams.ir_data_code,HEX);
        if(irparams.ir_custom_code==0)
        {
        if(irparams.ir_data_code==0x15)//END code,unlock the state,
                                       //prepare for the next execution.
           irparams.ir_data_selflock=0; 
        else                            
        {
           if(irparams.ir_data_selflock==0)//Execute for once,then lock the entrance,
                                  //prevent from retriggering.
             {irparams.ir_data_selflock=1;
              switch (irparams.ir_data_code)
                {case 0x02:     
                          digitalWrite(5,LOW);
                          digitalWrite(3,LOW);
                          digitalWrite(4,LOW);
                 break;
                 case 0x03:
                 digitalWrite(5,HIGH);
                          digitalWrite(3,LOW);
                          digitalWrite(4,LOW);
                 break;
                case 0x04:
                digitalWrite(5,LOW);
                digitalWrite(3,HIGH);
                digitalWrite(4,LOW);
                 break;
                 case 0x05:
                 digitalWrite(5,HIGH);
                digitalWrite(3,HIGH);
                digitalWrite(4,LOW);                
                 break;
                 case 0x06:
                   digitalWrite(5,LOW);
                digitalWrite(3,LOW);
                digitalWrite(4,HIGH);      
                 break;
                 case 0x07:
                   digitalWrite(5,HIGH);
                digitalWrite(3,LOW);
                digitalWrite(4,HIGH);      
                 break;
                 case 0x0c:
                   digitalWrite(5,LOW);
                digitalWrite(3,HIGH);
                digitalWrite(4,HIGH);      
                 break;
                case 0x0e:
                 case 0x0d:
                 case 0x0f:
                   digitalWrite(5,HIGH);
                digitalWrite(3,HIGH);
                digitalWrite(4,HIGH);    
                 break;               
                }
             }
        }
        }
        }
  
}
//=================================================================
// Decodes the received IR message
// Returns 0 if no data ready, 1 if data ready.
// Results of decoding are stored in results
//
bool IRrecv::decode()
             {if(irparams.ir_state!=CAPATURE)
                return false;
else {results.ir_data_code = irparams.ir_data_code;
      return 1;
      }
}

    















