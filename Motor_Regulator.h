#ifndef Motor_Regulator_h
#define Motor_Regulator_h

#include <inttypes.h>

//The speed and the direction of the motor will change gradually,
//for instance:if the current state is motor1.set(CLKWISE,255);
//and the desired state is motor1.set(COUNTER_CLKWISE,255);
//the speed will gradually decrease to 0,then the direction switches,                   
//and the speed will gradually increase to the max.

#define CLKWISE 1
#define COUNTER_CLKWISE 0

struct motor_ctrl_t{
unsigned long speedRegulationRate;
uint8_t ina,inb;//ina is set as PWM port,inb is general ioport.
bool current_direction;
bool expected_direction;
uint8_t current_speed;
uint8_t expected_speed;
uint8_t complementary_speed;
};


class Motor_Regulator
{
public:
struct motor_ctrl_t motor_ctrl;
Motor_Regulator(){}
void attach(int pin1,int pin2);
void set(int dir,int speed);
bool regulation();
};

#endif












