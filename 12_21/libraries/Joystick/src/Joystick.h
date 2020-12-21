#ifndef Joystick_h
#define Joystick_h

#include <Arduino.h> //Include common macros definitions,standard methods used
                     //in arduino.

//When the joystick is at different position,the voltage
//level on axis_x and axis_y will be changed accordingly.
#define LOW_LEVEL 0  //reading<350,assert for 50ms
#define MID_LEVEL 1  //otherwise
#define HIGH_LEVEL 2 //reading>700,assert for 50ms

//Direction
#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4
#define NORTH_EAST 5
#define NORTH_WEST 6
#define SOUTH_EAST 7
#define SOUTH_WEST 8
#define MIDDLE 0

const int numReadings = 16;

//Declaraction of the structure.
struct axis_t
{
    uint8_t pin;
    int readings[numReadings]; // the readings from the analog input
    int readIndex;             // the index of the current reading
    int total;                 // the running total
    int average;               // the average
    int readings_count;        //In order to perform average calculation,
                               //at least 16 readings are required.
    uint8_t level;
    uint8_t low_level_debounce_counter;
    uint8_t high_level_debounce_counter;
};

class Joystick
{
public:
    unsigned long samplingRate;
    struct axis_t axis_x;
    struct axis_t axis_y;
    uint8_t direction;
    Joystick(){}
    void attach(int pin_x, int pin_y);
    uint8_t getResult(); //Return the direction of the joystick.
};

#endif
