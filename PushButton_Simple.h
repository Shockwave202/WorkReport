#ifndef PushButton_Simple_h
#define PushButton_Simple_h

#include <Arduino.h> //Include common macros definitions,standard methods used
                     //in arduino.

struct pb_t //Declaraction of the structure.
{
uint8_t pinNbr;
int buttonState;           // the current reading from the input pin
int lastButtonState; // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime ; // the last time the output pin was toggled
unsigned long debounceDelay;   // the debounce time; increase if the output flickers
bool fallingEdge;                 //Indicate the START button is pressed.
bool risingEdge; 
bool reset;    
}; 


class PushButton_Simple
{
    //This button is supposed to be active low.
    //Which means the level on this pin remains low when the 
    //button is pressed,and remains high when the button is 
    //released.
public:
    struct pb_t pb; 
    PushButton_Simple(){}
    void create(int pin);
    void detection();
    bool isPress();
    bool isRelease();
};

#endif
