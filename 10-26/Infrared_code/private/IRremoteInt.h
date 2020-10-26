#ifndef IRremoteint_h
#define IRremoteint_h

//------------------------------------------------------------------------------
// Include the Arduino header
//
#include <Arduino.h>

// All board specific stuff have been moved to its own file, included here.
#include "IRremoteBoardDefs.h"

// ISR State-Machine : Receiver States
#define CAPATURE      0
#define RECEIVING_DATA      1
#define CONFIRMATION     2

/**
 * This struct is used for the ISR (interrupt service routine)
 * and is copied once only in state STATE_STOP, so only rcvstate needs to be volatile.
 */
struct irparams_struct {

	uint8_t ir_state;
	uint8_t ir_high_level_counter;
	uint8_t ir_low_level_counter;
	uint8_t ir_start_flag;
	uint8_t ir_end_flag;
	uint8_t ir_code;
	uint8_t ir_data_code;
    uint8_t  ir_custom_code;
	uint8_t  ir_data_selflock;
	uint8_t ir_bit_count;
	uint8_t new_arrival_flag;
	uint8_t offset;
};

extern struct irparams_struct irparams;

#endif
