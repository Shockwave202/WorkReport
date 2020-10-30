#ifndef Motor_h
#define Motor_h

#include <inttypes.h>

class Motor
{
public:
Motor();
void attach(int pin1,int pin2);
void rotateClkwise();
void rotateCounterClkwise();
void stop();
private:
uint8_t pinNbr1,pinNbr2;
};

#endif












