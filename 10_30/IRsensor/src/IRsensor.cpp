
#include <Arduino.h> //Include common macros definitions,standard methods used
                     //in arduino.

#include "IRsensor.h" //Macros definition,internel user-defined datatypes definition,
                      //class declaration.

//Constructor
IRsensor::IRsensor() {}

//Methods and fcns

void IRsensor::attach(int pin)
{
    this->pinNbr = pin;
}

uint16_t IRsensor::getResult()
{
    return analogRead(this->pinNbr);
}
