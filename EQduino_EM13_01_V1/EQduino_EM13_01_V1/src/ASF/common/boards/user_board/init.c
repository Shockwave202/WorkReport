/*
 * init.c
 *
 * Created: 2020/11/5 12:52:22
 *  Author: Administrator
 */

#include <board.h>
#include <compiler.h>
#include <conf_board.h>
#include "ioport.h"
#include "motor.h"
#include "timer.h"

void board_init(void)
{
    /* On board LED initialization */
    //ioport_configure_pin(LED0, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);

    /* On board Switch initialization */
    //ioport_configure_pin(GPIO_PUSH_BUTTON_0,
    //IOPORT_DIR_INPUT | IOPORT_PULL_UP);

    ioport_configure_pin(MOTOR1_A, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(MOTOR1_B, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(MOTOR2_A, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(MOTOR2_B, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);

    ioport_configure_pin(IR_REMOTE,
                         IOPORT_DIR_INPUT | IOPORT_PULL_UP);

    ioport_configure_pin(TEST, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);

    ioport_configure_pin(MODE1, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
    ioport_configure_pin(MODE2, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
    ioport_configure_pin(MODE3, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
    ioport_configure_pin(REMEOTE_ID1, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
    ioport_configure_pin(REMEOTE_ID2, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
    ioport_configure_pin(REMEOTE_ID3, IOPORT_DIR_INPUT | IOPORT_PULL_UP);

    //Theoretically,this operation will turn-off IR

    //ioport_configure_pin(IR_SWITCH, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);

    //Hopefully,this operation will turn-on IR

    ioport_configure_pin(IR_SWITCH, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
}
