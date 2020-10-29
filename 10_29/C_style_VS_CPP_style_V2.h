#ifndef Pushbutton_h
#define Pushbutton_h

//Define the marcos used in the CPP file 
//Cpp file contains the actual implementation of 
//the hardware.
#define DEBOUNCE_TIME 50

//Define a variable of the structure datatype:

//Method 1:
//Make a statement of the structure datatype,then
//define the required variables;

//Method 2:
//Make a statement of the structure datatype and 
//define the required variables simutaneously;
//
//Method 3:
//Define the variable directly without specifying
//the structure name.

//hahahahaha!C++



class Pushbutton
{
  public: 
    Pushbutton(); 
    attach(int pin); 
    bool isFallingEdge();//For loop()
  private:
  uint8_t pinNbr;
  bool selfLock;
  bool fallingEdge;
  uint8_t debounceCounter;
}
























#endif 