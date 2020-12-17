//Generate a square wave of 1KHZ using timer2.
//---------------------------------------------------------
// Timer2 (8 bits)
#include "Board.h"
#include "PushButton_Simple.h"
#include "PushButton.h"
#include "METALROBO_Module.h" //For the buzzer.
#include "Motor_Regulator.h"

//Create objs.
Board LABBOT_Board;
PushButton_Simple startButton;
PushButton pushButton1;
buzzer ontone;
Motor_Regulator motor1, motor2;

//Flags and variables associate with LABBOT_frog.
int frog_mode;
bool frog_is_jumping;
bool motor1_isRegulated, motor2_isRegulated;

void setup()
{ //The startup time is approximate to 1.6S.
  // put your setup code here, to run once:
  LABBOT_Board.initial_all_ioports();
  Serial.begin(9600);

  //Input:
  startButton.create(START);
  pushButton1.create(D3_BOARD);

  //Output:
  //LED
  pinMode(D1_BOARD, OUTPUT); //Attach 2 LEDs.
  digitalWrite(D1_BOARD, LOW);
  pinMode(D2_BOARD, OUTPUT);
  digitalWrite(D2_BOARD, LOW);
  //BEEP
  ontone.port(A1);
  //Motor
  motor1.attach(MOTOR1_INA, MOTOR1_INB); //Attach INA to the PWM output port
  motor2.attach(MOTOR2_INA, MOTOR2_INB); //Attach INB to the general ioport for direction ctrl
  motor1.set(CLKWISE, 0);
  motor2.set(CLKWISE, 0);

  frog_mode = 0;
  frog_is_jumping = 0;
}

void loop()
{
  // put your main code here, to run repeatedly

  //Mode selection.
  /*------------------------------------*/
  if (pushButton1.isSingleClick()) //Single click
  {
    frog_mode = 1;
  }
  if (pushButton1.isDoubleClick()) //Effective time interval:600MS
  {
    frog_mode = 2;
  }
  /*-------START the program-------*/
  startButton.detection(); //START button is detected in the main loop.
  if (startButton.isPress())
  {
    if (frog_mode) //If mode has been selected,start the program.
      frog_is_jumping = 1;

  } //Play the song for once.

  if (frog_is_jumping)
  {
    digitalWrite(D1_BOARD, HIGH); //Enlight LEDs
    digitalWrite(D2_BOARD, HIGH);

    //Activate the motors
    motor1.set(CLKWISE, 255);
    motor2.set(CLKWISE, 255);

    do
    {
      motor1_isRegulated = motor1.regulation();
      motor2_isRegulated = motor2.regulation();
    }
    //Wait until the speed finishes regulating.
    while (!motor1_isRegulated && !motor2_isRegulated);
    //Play the song.
    do
    {
      ontone.star();
      if (frog_mode == 2) //Play again
        ontone.star();
    } while (pushButton1.isPress());
    //If the button has been pressed during singing,play again.
    //Deactivate the motor
    motor1.set(CLKWISE, 0);
    motor2.set(CLKWISE, 0);
    do
    {
      motor1_isRegulated = motor1.regulation();
      motor2_isRegulated = motor2.regulation();
    }
    //Wait until the speed finishes regulating.
    while (!motor1_isRegulated && !motor2_isRegulated);
    digitalWrite(D1_BOARD, LOW);
    digitalWrite(D2_BOARD, LOW);
    //frog_mode = 0; 
    frog_is_jumping = 0;
  }
  /*------------------------------------*/
}
