#ifndef IRremote_h
#define IRremote_h

//------------------------------------------------------------------------------
#include "private/IRremoteInt.h"

/****************************************************
 *                     RECEIVING
 ****************************************************/
/**
 * Results returned from the decoder
 */
struct decode_results {
 
    uint8_t ir_data_code;          
 
};

/****************************************************
 *                     RECEIVING
 ****************************************************/

/**
 * Main class for receiving IR
 */
class IRrecv {
public:
    /**
     * Instantiate the IRrecv class. Multiple instantiation is not supported.
     * @param recvpin Arduino pin to use. No sanity check is made.
     */
    IRrecv();

    /**
     * Enable IR reception.
     */
    void enableIRIn();

    /**
     * Disable IR reception.
     */
    void disableIRIn();

	  /**
     * Called to re-enable IR reception.
     */
    void resume();

     bool decode();	 
	 decode_results results; // the instance for decoding
};

#endif // IRremote_h
