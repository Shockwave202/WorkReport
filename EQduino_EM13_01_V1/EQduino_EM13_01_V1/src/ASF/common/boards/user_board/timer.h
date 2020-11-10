/*
 * timer.h
 *
 * Created: 2020/11/5 18:30:51
 *  Author: Lenovo
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "ioport.h"

//------------------------------------------------------------------------------
// CPU Frequency
//
//External oscillator frequency
#define BOARD_XOSC_HZ 8000000
#define SYSCLOCK BOARD_XOSC_HZ // main clock

//---------------------------------------------------------
// Timer2 (8 bits)
//
#define TIMER_ENABLE_RECEIVE_INTR (TIMSK2 = _BV(OCIE2A))
// Output Compare Match A Interrupt Enable

//------------------------------------------------------------------------------
// microseconds per clock interrupt tick
#if !defined(MICROS_PER_TICK)
#define MICROS_PER_TICK 50
#endif

#define TIMER_COUNT_TOP (SYSCLOCK * MICROS_PER_TICK / 1000000)

/*static void timerConfigForReceive() {
	#  if (TIMER_COUNT_TOP < 256)
	TCCR2A = _BV(WGM21);
	TCCR2B = _BV(CS20);
	OCR2A  = TIMER_COUNT_TOP;
	TCNT2  = 0;
	#  else
	TCCR2A = _BV(WGM21);
	TCCR2B = _BV(CS21);
	OCR2A = TIMER_COUNT_TOP / 8;
	TCNT2 = 0;
	#  endif
}*/

#define TEST IOPORT_CREATE_PIN(PORTB, 3)

#define IR_REMOTE IOPORT_CREATE_PIN(PORTD, 2)

#define MODE1 IOPORT_CREATE_PIN(PORTC, 2)
#define MODE2 IOPORT_CREATE_PIN(PORTB, 1)
#define MODE3 IOPORT_CREATE_PIN(PORTD, 4)
#define REMEOTE_ID1 IOPORT_CREATE_PIN(PORTC, 1)
#define REMEOTE_ID2 IOPORT_CREATE_PIN(PORTB, 5)
#define REMEOTE_ID3 IOPORT_CREATE_PIN(PORTB, 4)

#define IR_SWITCH IOPORT_CREATE_PIN(PORTC, 3)

//The variable 'irremote' is of structure datatype

// ISR Receiver States
#define CAPATURE 0
#define RECEIVING_DATA 1
#define CONFIRMATION 2

#define MASK_REMOTE_ID1 (1 << 2)
#define MASK_REMOTE_ID2 (1 << 1)
#define MASK_REMOTE_ID3 (1 << 0)

uint8_t remote_id_setting;
uint8_t remote_id_setting_counter;
uint8_t count_1MS;

uint16_t count_1S;

struct irparams_t //Datatype declaration.
{
    uint8_t state;
    uint8_t highLevelCounter;
    uint8_t lowLevelCounter;
    uint8_t startFlag;
    uint8_t endFlag;
    uint8_t code;
    uint8_t bitCount;
    uint8_t receivedFlag;
    uint8_t offset;
} irparams;

void enableIRIn(void);

#endif /* TIMER_H_ */