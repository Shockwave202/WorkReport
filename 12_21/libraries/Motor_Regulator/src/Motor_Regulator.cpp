
#include <Arduino.h> //Include common macros definitions,standard methods used
                     //in arduino.

#include "Motor_Regulator.h" //Macros definition,internel user-defined datatypes definition,
                      //class declaration.

void Motor_Regulator::attach(int pin1, int pin2)
{
    //L9110S is used as actuator,2 signal pin is required.
    this->motor_ctrl.ina = pin1;//Speed regulation
    this->motor_ctrl.inb = pin2;//Direction ctrl
    this->motor_ctrl.current_direction = CLKWISE;
    this->motor_ctrl.expected_direction = CLKWISE;
    this->motor_ctrl.current_speed = 0;
    this->motor_ctrl.expected_speed = 0;
    this->motor_ctrl.complementary_speed = 0;

    pinMode(this->motor_ctrl.ina, OUTPUT);
    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);

    pinMode(this->motor_ctrl.inb, OUTPUT);
    digitalWrite(this->motor_ctrl.inb, LOW);

    this->motor_ctrl.speedRegulationRate = millis(); //Reset the timer.
}

void Motor_Regulator::set(int dir,int speed)
{this->motor_ctrl.expected_direction = dir;
 this->motor_ctrl.expected_speed = speed;
}

bool Motor_Regulator::regulation()
{
    /*------------------------------------------*/
    bool isRegulated = 0; //Report the status.

    if (millis() - this->motor_ctrl.speedRegulationRate > 0) //Count for 1ms
    {
        this->motor_ctrl.speedRegulationRate = millis(); //Reset the timer
        /*----Toggle D4_BOARD-----
        if (digitalRead(13))
            digitalWrite(13, LOW);
        else
            digitalWrite(13, HIGH);
        /*--------------------*/

        /*--------------------MOTOR--------------------*/
        if (this->motor_ctrl.current_direction == CLKWISE)
        {
            //Direction changing
            if (this->motor_ctrl.expected_direction == COUNTER_CLKWISE)
            { //Slow down
                if (this->motor_ctrl.current_speed > 0)
                {
                    this->motor_ctrl.current_speed -= 5;
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);
                }
                else
                {
                    this->motor_ctrl.current_direction = COUNTER_CLKWISE;
                    this->motor_ctrl.current_speed = 255;
                    this->motor_ctrl.complementary_speed = 255 - this->motor_ctrl.expected_speed;
                    digitalWrite(this->motor_ctrl.inb, HIGH);
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.complementary_speed);
                }
                isRegulated = false;
            }
            else
            {
                //Speed regulation
                if (this->motor_ctrl.current_speed < this->motor_ctrl.expected_speed)
                {
                    this->motor_ctrl.current_speed += 5;
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);
                    isRegulated = false;
                }
                else if (this->motor_ctrl.current_speed > this->motor_ctrl.expected_speed)
                {
                    this->motor_ctrl.current_speed -= 5;
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);
                    isRegulated = false;
                }
                else
                    isRegulated = true;
            }
        }
        else //current_direction=COUNTER_CLKWISE
        {
            //Direction changing
            if (this->motor_ctrl.expected_direction == CLKWISE)
            { //Slow down
                if (this->motor_ctrl.current_speed < 255)
                {
                    this->motor_ctrl.current_speed += 5;
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);
                }
                else
                {
                    this->motor_ctrl.current_direction = CLKWISE;
                    this->motor_ctrl.current_speed = 0;
                    digitalWrite(this->motor_ctrl.inb, LOW);
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);
                }
                isRegulated = false;
            }
            else
            {
                //Speed regulation
                this->motor_ctrl.complementary_speed = 255 - this->motor_ctrl.expected_speed;
                if (this->motor_ctrl.current_speed < this->motor_ctrl.complementary_speed)
                {
                    this->motor_ctrl.current_speed += 5;
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);
                    isRegulated = false;
                }
                else if (this->motor_ctrl.current_speed > this->motor_ctrl.complementary_speed)
                {
                    this->motor_ctrl.current_speed -= 5;
                    analogWrite(this->motor_ctrl.ina, this->motor_ctrl.current_speed);
                    isRegulated = false;
                }
                else
                    isRegulated = true;
            }
        }
    }
    return isRegulated;
}
