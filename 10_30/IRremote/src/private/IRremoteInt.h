#ifndef IRremoteint_h
#define IRremoteint_h

//------------------------------------------------------------------------------
// Include the Arduino header
//
#include <Arduino.h>

// All board specific stuff have been moved to its own file, included here.
#include "IRremoteBoardDefs.h"

// ISR Receiver States
#define CAPATURE 0
#define RECEIVING_DATA 1
#define CONFIRMATION 2

/**
 * This struct is used for the ISR (interrupt service routine)
 * and is copied once only in state STATE_STOP, so only rcvstate needs to be volatile.
 */
struct irparams_struct
{
	uint8_t pin;
	uint8_t state;
	uint8_t highLevelCounter;
	uint8_t lowLevelCounter;
	uint8_t startFlag;
	uint8_t endFlag;
	uint8_t code;
	uint8_t bitCount;
	uint8_t receivedFlag;
	uint8_t offset;
};

extern struct irparams_struct irparams;

#endif
