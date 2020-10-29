
uint8_t Motor::attach(int pin1,int pin2)
{
//L9110S is used as actuator,thus,2 signal pin 
//is required.  
pinMode(pin1, OUTPUT);
pinMode(pin2, OUTPUT);
digitalWrite(pin1,LOW); 
digitalWrite(pin2,LOW); 
// set servo pin to output
this->pinNbr1=pin1;
this->pinNbr2=pin1;
}


Motor::rotateClkwise()
{
digitalWrite(this->pinNbr1,HIGH); 
digitalWrite(this->pinNbr2,LOW); 
}

Motor::rotateCounterClkwise()
{
digitalWrite(this->pinNbr1,LOW); 
digitalWrite(this->pinNbr2,HIGH); 
}

Motor::stop()
{
digitalWrite(this->pinNbr1,LOW); 
digitalWrite(this->pinNbr2,LOW);    
}






