/*
 * motor.h
 *
 * Created: 2020/11/5 11:00:56
 *  Author: Lenovo
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "compiler.h"
#include "ioport.h"

#define MOTOR1_A IOPORT_CREATE_PIN(PORTD, 5)
#define MOTOR1_B IOPORT_CREATE_PIN(PORTD, 6)
#define MOTOR2_A IOPORT_CREATE_PIN(PORTD, 7)
#define MOTOR2_B IOPORT_CREATE_PIN(PORTB, 0)
    void motor_rotateClkwise(int pin1, int pin2);
    void motor_rotateCounterClkwise(int pin1, int pin2);
    void motor_stop(int pin1, int pin2);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H_ */