/*
 * timer.c
 *
 * Created: 2020/11/5 18:31:05
 *  Author: Lenovo
 */
#include <compiler.h>
#include "timer.h"

void enableIRIn(void)
{
    // the interrupt Service Routine fires every 50 uS

    //noInterrupts();
    cli();

    // Setup pulse clock timer interrupt
    // Prescale /8 (16M/8 = 0.5 microseconds per tick)
    // Therefore, the timer interval can range from 0.5 to 128 microseconds
    // Depending on the reset value (255 to 0)

    //timerConfigForReceive();
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS21);
    //OCR2A = TIMER_COUNT_TOP / 8;
    OCR2A = 50;
    TCNT2 = 0;

    // Timer2 Overflow Interrupt Enable
    TIMER_ENABLE_RECEIVE_INTR;

    //Clear interrupt flag
    //TIMER_RESET_INTR_PENDING;

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

    //interrupts();
    sei();
}
