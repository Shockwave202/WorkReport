
#include <Arduino.h>//Include common macros definitions,standard methods used 
                    //in arduino.

#include"Motor.h"//Macros definition,internel user-defined datatypes definition,
                 //class declaration.


Motor::Motor(){}

void Motor::attach(int pin1,int pin2)
{
//L9110S is used as actuator,thus,2 signal pin 
//is required.  
pinMode(pin1, OUTPUT);
pinMode(pin2, OUTPUT);
digitalWrite(pin1,LOW); 
digitalWrite(pin2,LOW); 
// set servo pin to output
this->pinNbr1=pin1;
this->pinNbr2=pin2;
}


void Motor::rotateClkwise()
{
digitalWrite(this->pinNbr1,HIGH); 
digitalWrite(this->pinNbr2,LOW); 
}

void Motor::rotateCounterClkwise()
{
digitalWrite(this->pinNbr1,LOW); 
digitalWrite(this->pinNbr2,HIGH); 
}

void Motor::stop()
{
digitalWrite(this->pinNbr1,LOW); 
digitalWrite(this->pinNbr2,LOW);    
}






