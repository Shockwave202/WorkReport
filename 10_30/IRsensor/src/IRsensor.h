#ifndef IRsensor_h
#define IRsensor_h

#include <inttypes.h>
//Macros

//datatypes

//class
class IRsensor
{
public:
    IRsensor();
    void attach(int pin);
    uint16_t getResult();

private:
    uint8_t pinNbr;
};
#endif
