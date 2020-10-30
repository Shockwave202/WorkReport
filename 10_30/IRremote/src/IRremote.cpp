#include "IRremote.h"

//+=============================================================================
IRremote::IRremote() {} //Constructor

void IRremote::attach(int pin)
{
  irparams.pin = pin;
}
//--------------------------------
// initialization
//
#ifdef USE_DEFAULT_ENABLE_IR_IN
void IRremote::enableIRIn()
{

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

  irparams.state = CAPATURE;
  irparams.highLevelCounter = 0;
  irparams.lowLevelCounter = 0;
  irparams.startFlag = 0;
  //Some of the params are cleared at the beginning of
  // each particular stage,no need to initialize all of them.
  // irparams.endFlag=0;
  // irparams.code=0;
  // irparams.bitCount=0;
  irparams.receivedFlag = 0;
  // irparams.offset=0;

  interrupts();
}

#endif // USE_DEFAULT_ENABLE_IR_IN

// Returns 0 if no data ready, 1 if data ready.
bool IRremote::isReady()
{
  //The cpu will repeatedly polling the arrival flag
  // to see if the receiving is occured.
  if (irparams.receivedFlag)
  {
    irparams.receivedFlag = 0;
    //After querying,this bit should be cleared to
    //prevent from retriggering.
    return true;
  }
  else
    return false;
}

uint8_t IRremote::getResult()
{
  return irparams.code;
}
