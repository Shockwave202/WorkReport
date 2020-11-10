/*
 * motor.c
 *
 * Created: 2020/11/5 11:01:29
 *  Author: Lenovo
 */ 

#include <compiler.h>
#include "motor.h"

void motor_rotateClkwise(int pin1,int pin2)
{
	ioport_set_pin_high(pin1);//gpio operation
	ioport_set_pin_low(pin2);
}


void motor_rotateCounterClkwise(int pin1,int pin2)
{
	ioport_set_pin_low(pin1);//gpio operation
	ioport_set_pin_high(pin2);
}

void motor_stop(int pin1,int pin2)
{
	ioport_set_pin_low(pin1);//gpio operation
	ioport_set_pin_low(pin2);
}































