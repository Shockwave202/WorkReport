#ifndef IRremote_h
#define IRremote_h

//------------------------------------------------------------------------------
#include "private/IRremoteInt.h"

/****************************************************
 *                     RECEIVING
 ****************************************************/

/**
 * Main class for receiving IR
 */
class IRremote
{
public:
    // Instantiate the IRremote class.
    IRremote();
    void attach(int pin);
    void enableIRIn();
    bool isReady();
    uint8_t getResult();
};

#endif // IRremote_h
