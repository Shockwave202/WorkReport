/**
 * \file
 *
 * \brief MEGA LED Example
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage MEGA LED Example
 * See \ref appdoc_main "here" for project documentation.
 * \copydetails appdoc_preface
 *
 *
 * \page appdoc_preface Overview
 * This application demonstrates a simple example to turn on and off the board LED.
 */

/**
 * \page appdoc_main MEGA LED Example
 *
 * Overview:
 * - \ref appdoc_mega_led_app_intro
 * - \ref appdoc_mega_led_app_usage
 * - \ref appdoc_mega_led_app_compinfo
 * - \ref appdoc_mega_led_app_contactinfo
 *
 * \section appdoc_mega_led_app_intro Introduction
 * This application demonstrates a simple example turn on and off the board LED.
 *
 * This application has been tested on following boards:
 * - ATmega328p Xplained Mini
 * - ATmega328pb Xplained Mini
 * - ATmega168pb Xplained Mini
 * - ATmega324pb Xplained PRO
 *
 * \section appdoc_mega_led_app_usage Usage
 * The application uses button to control the LED, 
 * once the button is pressed, LED0 will turn on
 * once the button is released, LED0 will turn off
 *
 * \section appdoc_mega_led_app_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for MEGA.
 * Other compilers may or may not work.
 *
 * \section appdoc_mega_led_app_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>

uint8_t cmd;
uint8_t data_code;
uint8_t custom_code;

extern struct irparams_t irparams;
extern uint8_t remote_id_setting;
extern uint8_t remote_id_setting_counter;
extern uint8_t count_1MS;

volatile bool recv_flag;
volatile uint8_t code;
volatile uint8_t remote_id;

volatile bool remote_signal_countdown;
volatile bool remote_signal_lost;
extern uint16_t count_1S;

int main(void)
{
    /* set board io port */
    board_init();

    enableIRIn();
    //These variables should be initialized before activate the process.
    remote_id_setting_counter = 0;
    count_1MS = 0;
    while (1)
    {
        /*button_state = ioport_get_pin_level(GPIO_PUSH_BUTTON_0);
		if(button_state){
			LED_Off(LED0);
		}else{
			LED_On(LED0);
		}*/
        //	motor_rotateCounterClkwise(MOTOR1_A,MOTOR1_B);
        //	motor_rotateClkwise(MOTOR2_A,MOTOR2_B);
        if (recv_flag)
        {
            //After querying,this bit should be cleared to
            //prevent from retriggering.
            recv_flag = 0;

            //	ioport_set_pin_low(TEST);
            //	ioport_set_pin_high(TEST);

            //Each time receving the data code(cmd),compare with the previous cmd,
            //if the cmd changes,refresh and execute the cmd,
            //if the receving cmd is the same as the previous one,
            //just ignore the cmd so as to prevent from retriggering.
            //Check if the cmd changes
            //	 if (cmd != code) //The cmd will execute only when it is different from the previous one.
            //	 {                                 //Refresh the cmd
            //		 cmd = code;
            /*------------------------------------------------------*/
            //Effective time mechanism
            //The motor will run only when receiving the continuous remote
            //signal,if the remote signal is lost,
            //that is,the receiver hasn't
            //received anything for a long period since last reception,
            //stop the motor.
            cmd = code;
            //Start the counter to monitor the signal,if the signal
            //lost,stop the motor.
            remote_signal_countdown = 1;
            count_1S = 0;
            remote_signal_lost = 0;
            //Execute the cmd
            data_code = cmd & 0x1f;       //Clear the highest 3 bits of the receiving data
            custom_code = cmd >> 5;       //Right shift 5 bits.
            if (custom_code == remote_id) //Custom code identification
            {

                switch (data_code)
                {
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                    break;
                case 0x0c: //L up

                    motor_rotateClkwise(MOTOR1_A, MOTOR1_B);
                    motor_stop(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x0d: //R up

                    motor_stop(MOTOR1_A, MOTOR1_B);
                    motor_rotateClkwise(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x0e: //L down

                    motor_rotateCounterClkwise(MOTOR1_A, MOTOR1_B);
                    motor_stop(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x0f: //R down

                    motor_stop(MOTOR1_A, MOTOR1_B);
                    motor_rotateCounterClkwise(MOTOR2_A, MOTOR2_B);

                    break;
                //Combination:
                case 0x1c: //L_up +R_up

                    motor_rotateClkwise(MOTOR1_A, MOTOR1_B);
                    motor_rotateClkwise(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x13: //L_down +R_down

                    motor_rotateCounterClkwise(MOTOR1_A, MOTOR1_B);
                    motor_rotateCounterClkwise(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x16: //L_up +R_down

                    motor_rotateClkwise(MOTOR1_A, MOTOR1_B);
                    motor_rotateCounterClkwise(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x19: //L_down +R_up

                    motor_rotateCounterClkwise(MOTOR1_A, MOTOR1_B);
                    motor_rotateClkwise(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x1a: //L_up +L_down
                    //Invalid
                    break;
                case 0x15: //END code

                    motor_stop(MOTOR1_A, MOTOR1_B);
                    motor_stop(MOTOR2_A, MOTOR2_B);

                    break;
                case 0x02: //F1
                    break;
                }
            }
            //	 }
        }

        if (remote_signal_lost)
        {
            remote_signal_lost = 0;
            //Shutdown the motor
            motor_stop(MOTOR1_A, MOTOR1_B);
            motor_stop(MOTOR2_A, MOTOR2_B);
        }
    }
}

ISR(TIMER2_COMPA_vect)
{

    //Clear interrupt flag
    //TIMER_RESET_INTR_PENDING; // reset timer interrupt flag if required (currently only for Teensy and ATmega4809)

    //gpio_toggle_pin(TEST);

    if (irparams.state == CAPATURE) //Detecting the header of the frame.
                                    /*------------------------
Capature this:
				   _______________
				   |
				   |
				  /|\
				   |
___________________|
|<----650US------->|<----650US------>|
----------------------------*/
    {
        if (ioport_pin_is_low(IR_REMOTE))
        {

            irparams.highLevelCounter = 0; //Debounce
            irparams.lowLevelCounter++;
            if (irparams.lowLevelCounter == 11) //50US*12=600US
            {
                irparams.startFlag = 1; //Produce
            }
            else if (irparams.lowLevelCounter == 14) //50US*14=700US
            {                                        //Low level exceeds 700US,abort and restart.
                irparams.startFlag = 0;              //Abort
                irparams.lowLevelCounter = 0;
            }
        }
        else
        {
            irparams.lowLevelCounter = 0; //Debounce
            irparams.highLevelCounter++;
            if (irparams.highLevelCounter == 11) //50US*13=650US
            {

                if (irparams.startFlag)
                {
                    irparams.startFlag = 0; //Consume
                                            //volt_low 650US + voltage_high 650US has been capatured,
                                            //switch to the next stage for receiving 8-bit data.
                    irparams.state = RECEIVING_DATA;
                    irparams.highLevelCounter = 0;
                    //Clear the counter,prepare for the next stage.
                    //Initial variables
                    irparams.bitCount = 0;
                    irparams.code = 0; //receiving buffer.
                    irparams.offset = 0;
                }
            }
            else if (irparams.highLevelCounter == 14)
                irparams.highLevelCounter = 0;
        }
    }

    else if (irparams.state == RECEIVING_DATA)
    {
        if (ioport_pin_is_high(IR_REMOTE)) //The pin is at high level
        {
            irparams.lowLevelCounter = 0;
            irparams.highLevelCounter++;
            if (irparams.highLevelCounter == 7) //7*50US=350US
            //High level asserts for 0.4MS represents 0
            {
                irparams.highLevelCounter = 0;
                irparams.code &= ~0x80; //Record the sampling result,
                                        //Clear the MSB.

                irparams.bitCount++;

                ioport_set_pin_low(TEST);
                ioport_set_pin_high(TEST);

                if (irparams.bitCount == 8)
                {
                    irparams.state = CONFIRMATION;
                    irparams.endFlag = 0;
                    irparams.receivedFlag = 0;
                }
                else
                    irparams.code >>= 1; //Right shift one bit,prepare
                                         //for the reception of the next bit
            }
        }
        else
        {
            irparams.highLevelCounter = 0;
            irparams.lowLevelCounter++;
            if (irparams.lowLevelCounter == 8) //7*50US=350US
            //Low level asserts for 0.4MS represents 1
            {
                irparams.lowLevelCounter = 0;
                irparams.code |= 0x80; //Record the sampling result,
                                       //Set the MSB
                irparams.bitCount++;

                ioport_set_pin_low(TEST);
                ioport_set_pin_high(TEST);

                if (irparams.bitCount == 8)
                {
                    irparams.state = CONFIRMATION;
                    irparams.endFlag = 0;
                    irparams.receivedFlag = 0;
                    //If the last bit of the receiving data is low,some modification
                    //is required.
                    irparams.offset = 1;
                }

                else
                    irparams.code >>= 1; //Right shift one bit,prepare
                                         //for the arrival of the next bit
            }
        }
    }

    else if (irparams.state == CONFIRMATION)
    {
        /*----------------
Confirm this:
		_____________
		            |
					|
				   \|/
					|
					|_____________
|<----650US-------->|<----650US------>|
----------------------*/
        //Confirm if END compiles the standard KXM013C format,if the confirmation
        //failed,abort the value and restart from the capature state.

        if (ioport_pin_is_high(IR_REMOTE))
        {
            if (irparams.lowLevelCounter > 0 && irparams.lowLevelCounter < 11)
            //Volt_low is too short,confirmation failure,
            //abort and restart the mission.
            {
                if (irparams.offset == 1)
                {
                    irparams.offset = 0;
                    irparams.lowLevelCounter = 0;
                    //No need to switch state
                }
                else
                {
                    irparams.state = CAPATURE;
                    irparams.lowLevelCounter = 0;
                }
            }

            //Check if the volt_high has asserted for approximately 650US
            //if the voltage level becomes low during this period,
            //or exceeds the regular time,abort and restart the mission
            irparams.highLevelCounter++;
            if (irparams.highLevelCounter == 11) //50US*13=650US
                irparams.endFlag = 1;            //Produce
            else if (irparams.highLevelCounter == 20)
            {
                irparams.endFlag = 0;
                irparams.highLevelCounter = 0;
                //Time exceed,abort and restart the mission
                irparams.state = CAPATURE;
            }
        }
        else
        {
            if (irparams.highLevelCounter > 0 && irparams.highLevelCounter < 11)
            //Volt_high is too short,abort.
            {
                irparams.state = CAPATURE;
                irparams.highLevelCounter = 0;
            }
            irparams.lowLevelCounter++;
            if (irparams.lowLevelCounter == 11) //The receiving pin has being pulled down
            //for at least 0.6MS
            {
                if (irparams.endFlag) //Consume
                {
                    irparams.endFlag = 0;
                    irparams.receivedFlag = 1;
                    recv_flag = 1;
                    code = irparams.code;
                    //Format confirmed,the data is reliable.
                    irparams.state = CAPATURE;
                    //Switch to the capature stata,start a new cycle.
                    irparams.lowLevelCounter = 0;
                    irparams.highLevelCounter = 0;
                    //Probe
                    //digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
                    //digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
                    //while(1);

                    //	gpio_set_pin_low(TEST);
                    //	gpio_set_pin_high(TEST);
                }
            }
            else if (irparams.lowLevelCounter == 14)
            {
                irparams.lowLevelCounter = 0;
                //Time exceed,abort and restart the mission
                irparams.state = CAPATURE;
            }
        }
    }

    //Task:Refresh the remote_id at remote ctrl state.
    count_1MS++;
    if (count_1MS == 20) //50US*20=1MS
    {
        count_1MS = 0;
        //Generate a clk tick with a time interval of 1MS.
        //The tasks in this loop needs to be debouncing first,
        //so as to get more reliable values.

        //TASK_REMOTE_ID_SETTING
        //Sampling the dial-switch
        remote_id_setting = 0;
        if (!ioport_get_pin_level(REMEOTE_ID1)) //Active low
            remote_id_setting |= MASK_REMOTE_ID1;
        if (!ioport_get_pin_level(REMEOTE_ID2)) //Active low
            remote_id_setting |= MASK_REMOTE_ID2;
        if (!ioport_get_pin_level(REMEOTE_ID3)) //Active low
            remote_id_setting |= MASK_REMOTE_ID3;
        //If the value changed,implementing debouncing process.
        if (remote_id != remote_id_setting)
        {
            remote_id_setting_counter++;
            if (remote_id_setting_counter == 100)
                //If the change lasts for 100MS
                //Confirm the change and refresh the value.
                remote_id = remote_id_setting;
        }
        else
            remote_id_setting_counter = 0;

        //TASK_SIGNAL_MONITOR
        //After receiving the signal,start this counter,if the counter
        //reaches the time line,signal the main ctrller to stop the motor.
        if (remote_signal_countdown)
        {
            count_1S++;
            if (count_1S == 1000)
            {
                remote_signal_countdown = 0;
                remote_signal_lost = 1;
            }
        }
    }
}
