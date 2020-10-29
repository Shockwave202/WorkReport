#ifndef Motor_h
#define Motor_h

Class Motor
{
public:
Motor();
uint8_t attach(int pin);  
void rotateClkwise();
void rotateCounterClkwise();
void stop();
private:
uint8_t pinNbr1,pinNbr2;
}

#endif












