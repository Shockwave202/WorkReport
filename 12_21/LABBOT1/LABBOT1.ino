//Verify each parts,test if everything works fine,
//then assemble the whole stuff.

//Generate a square wave of 1KHZ using timer2.
//---------------------------------------------------------
// Timer2 (8 bits)
#include "Board.h"
#include "PushButton_Simple.h"
#include "PushButton.h"
#include "Joystick.h"
#include "METALROBO_Module.h" //For the buzzer.
#include "Motor_Regulator.h"

//Create objs.
Board LABBOT_Board;
//Input:
PushButton_Simple startButton;
PushButton pushButton1, pushButton2;
Joystick joystick;
//Output:
buzzer ontone;
Motor_Regulator motor1, motor2;

bool program_selection;
int pgm_index;
bool pgm_switching;

bool aircraft;
bool motor1_isRegulated, motor2_isRegulated;

//Flags and variables associate with LABBOT_frog.
int frog_mode;
bool frog_is_jumping;

bool self_tracing_vehicle;

bool joystickCtrlledVehicle;

bool avoidObstacleVehicle;

//IR_Sensor
uint16_t samplingRateForDebouncing;
//IR_Sensor front
bool analogReadSelflock1;
uint8_t analogReadDebouneCount1;
bool avoidObstacleInFront;
bool avoidObstacleInFront_wait350msFlag;
bool avoidObstacleOnTheFloor_turnLeft;
unsigned long avoidObstacleInFront_wait350msTimer;
//IR_Sensor underneath
bool analogReadSelflock2;
uint8_t analogReadDebouneCount2;
bool avoidObstacleOnTheFloor;
bool avoidObstacleOnTheFloor_wait350msFlag;
bool avoidObstacleInFront_turnRight;
unsigned long avoidObstacleOnTheFloor_wait350msTimer;

//Shaking
#define TURN_LEFT 0
#define TURN_RIGHT 1
bool shaking_wait150msFlag;
unsigned long shaking_wait150msTimer;
bool shakingState;

bool interactive_robot;
unsigned long startTime;
//Record the start time of the time interval.
unsigned long timeInterval;
//Within the time intervel,the randomized condition remains;
//and it will change at the end of the time interval.
int randomNbr;
int timeIntervalCount;
//Count for 5 times,then decrease the time interval.

bool motor1_rotation;
bool motor1_move_forward;
unsigned long motor1_runningTime;

bool motor2_rotation;
bool motor2_move_forward;
unsigned long motor2_runningTime;

int Tone_data[8][8] = {
    {31, 33, 37, 41, 44, 49, 55, 62},
    {65, 73, 82, 87, 98, 110, 123, 0},
    {131, 147, 165, 175, 196, 220, 247, 0},
    {262, 294, 330, 349, 392, 440, 494, 0},
    {523, 587, 659, 698, 784, 880, 988, 0},
    {1047, 1175, 1319, 1397, 1568, 1760, 1976, 0},
    {2093, 2349, 2637, 2794, 3136, 3520, 3951, 0},
    {4148, 4699, 0, 0, 0, 0, 0, 0},
};
//music start:
void music()
{
  ontone.tone(Tone_data[5][0], 200);
  ontone.tone(Tone_data[5][1], 200);
  ontone.tone(Tone_data[5][0], 200);
  ontone.tone(Tone_data[5][1], 200);
  ontone.tone(Tone_data[5][0], 200);
  ontone.tone(Tone_data[5][1], 200);
}

void setup()
{ //The startup time is approximate to 1.6S.
  // put your setup code here, to run once:
  LABBOT_Board.initial_all_ioports();
  Serial.begin(9600);
  //Input:
  startButton.create(START);

  //Output:
  //LED:
  pinMode(D1_BOARD, OUTPUT); //Attach LED1 to D1 pin
  digitalWrite(D1_BOARD, LOW);
  pinMode(D2_BOARD, OUTPUT); //Attach LED2 to D2 pin
  digitalWrite(D2_BOARD, LOW);
  //BEEP
  ontone.port(BEEP);
  //Motor
  motor1.attach(MOTOR1_INA, MOTOR1_INB); //Attach INA to the PWM output port
  motor2.attach(MOTOR2_INA, MOTOR2_INB); //Attach INB to the general ioport for direction ctrl
  motor1.set(CLKWISE, 0);
  motor2.set(CLKWISE, 0);
}

void loop()
{
  // put your main code here, to run repeatedly:
  startButton.detection(); //Check if the button is pressed.
  /*-----Fcn:These codes are used for program selection.-----*/
  if (startButton.isPress())
  {
    //If the START button is pressed,activate the program
    //selection procedure.
    program_selection = 1;
    pgm_index = 1;
  }

  if (program_selection)
  {
    do
    {
      //Program selection procedure
      switch (pgm_index)
      {
      case 1:
        ontone.tone(Tone_data[3][0], 500);
        break;
      case 2:
        ontone.tone(Tone_data[3][1], 500);
        break;
      case 3:
        ontone.tone(Tone_data[3][2], 500);
        break;
      case 4:
        ontone.tone(Tone_data[3][3], 500);
        break;
      case 5:
        ontone.tone(Tone_data[3][4], 500);
        break;
      case 6:
        ontone.tone(Tone_data[3][5], 500);
        break;
      case 7:
        ontone.tone(Tone_data[3][6], 500);
        break;
      case 8:
        ontone.tone(Tone_data[4][0], 500);
        break;
      case 9:
        ontone.tone(Tone_data[4][1], 500);
        break;
      case 10:
        ontone.tone(Tone_data[4][2], 500);
        break;
      case 11:
        ontone.tone(Tone_data[4][3], 500);
        break;
      }
      pgm_index++; //Increasing while in the loop
      if (pgm_index == 12)
        pgm_index = 1; //Return to the beginning.
    } while (!digitalRead(START));
    //Continue the loop while the pushbutton remains low.
    program_selection = 0; //Exit
    //Recover the index before leaving.
    if (pgm_index == 1)
      pgm_index = 11;
    else
      pgm_index--;
    //Check the result.
    //Serial.print("pgm_index:");
    //Serial.println(pgm_index);
    pgm_switching = 1; //This bit indicates a pgm switching event occurs,
  }

  if (pgm_switching)
  {
    pgm_switching = 0;
    //The initalization of each individual subprogram.
    switch (pgm_index)
    {
    case 1:
      /*------propeller-driven aircraft-------*/
      aircraft = 0;
      while (1)
      {                          // put your main code here, to run repeatedly:
        startButton.detection(); //Check if the button is pressed.
        /*--START the program--*/
        if (startButton.isPress())
        {
          //Propeller-driven aircraft.
          aircraft = 1;
        } //Play the song for once.

        /*--Execute the program---*/
        if (aircraft)
        {
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
          digitalWrite(D1_BOARD, HIGH);
          digitalWrite(D2_BOARD, HIGH);
          ontone.star();
          digitalWrite(D1_BOARD, LOW);
          digitalWrite(D2_BOARD, LOW);
          delay(100);
          //Reverse the motors
          motor1.set(COUNTER_CLKWISE, 255);
          motor2.set(COUNTER_CLKWISE, 255);
          do
          {
            motor1_isRegulated = motor1.regulation();
            motor2_isRegulated = motor2.regulation();
          }
          //Wait until the speed finishes regulating.
          while (!motor1_isRegulated && !motor2_isRegulated);
          digitalWrite(D1_BOARD, HIGH);
          digitalWrite(D2_BOARD, HIGH);
          ontone.star();
          digitalWrite(D1_BOARD, LOW);
          digitalWrite(D2_BOARD, LOW);
          delay(100);
        }
      }
      break;
    case 2:
      /*----------frog------------------------------*/
      pushButton1.create(D3_BOARD);
      frog_mode = 0;
      frog_is_jumping = 0;
      while (1)
      {
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
      break;
    case 3:
      //IR sensor
      pinMode(A1_BOARD, INPUT);
      pinMode(A2_BOARD, INPUT);
      self_tracing_vehicle = 0;
      while (1)
      {
        /*-------------self_tracing_vehicle----------------*/
        startButton.detection(); //Check if the button is pressed.
        /*--START the program--*/
        if (startButton.isPress())
        {
          self_tracing_vehicle = 1;
        }

        if (self_tracing_vehicle)
        {
          /*----------SELF_TRACKING_LOGIC-----------------------------*/
          if (analogRead(A1_BOARD) > 900 && analogRead(A2_BOARD) > 900)
          { //Both left IR sensor and right IR sensor detect the white floor,go straight forward;
            //AD voltage high:4.96V;
            //VDD=4.96V;
            // ( 4.76 / 4.96 ) * 1023 = 0.96 * 1023 = 982

            //AD voltage low:2.12V
            // ( 2.12 / 4.96 ) * 1023 = 0.43 * 1023 = 440
            motor1.set(CLKWISE, 255);
            motor2.set(CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
          }
          else if (analogRead(A1_BOARD) > 900 && analogRead(A2_BOARD) < 500)
          { //Right IR sensor has detected the black stripe,left IR sensor has detected the white floor,
            //turn left under this condition.
            motor1.set(COUNTER_CLKWISE, 255); //Left wheel
            motor2.set(CLKWISE, 255);         //Right wheel
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, LOW);
          }
          else if (analogRead(A1_BOARD) < 500 && analogRead(A2_BOARD) > 900)
          {
            //Left IR sensor has detected the black stripe,right IR sensor has detected the white floor,
            //turn right under this condition.
            motor1.set(CLKWISE, 255);         //Left wheel
            motor2.set(COUNTER_CLKWISE, 255); //Right wheel
            digitalWrite(D2_BOARD, HIGH);
            digitalWrite(D1_BOARD, LOW);
          }

          else if (analogRead(A1_BOARD) < 500 && analogRead(A2_BOARD) < 500)
          { //Right IR sensor has detected the black stripe,left IR sensor has detected the black stripe,
            //stop under this condition.
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
          }

          motor1.regulation();
          motor2.regulation();
        }
        /*----------------------------------------------*/
      }
      break;
    case 4:
    case 5:
    case 8:
    case 10:
    case 11:
      //Input:
      //Joystick
      joystick.attach(A3_BOARD, A4_BOARD);
      joystickCtrlledVehicle = 0;
      while (1)
      {
        /*-----------------------------------------*/
        startButton.detection(); //Check if the button is pressed.

        /*--START the program--*/
        if (startButton.isPress())
        {
          joystickCtrlledVehicle = 1;
        }

        /*--Run the program--*/
        if (joystickCtrlledVehicle)
        {
          switch (joystick.getResult())
          {
          case NORTH:
            motor1.set(CLKWISE, 255);
            motor2.set(CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case SOUTH:
            motor1.set(COUNTER_CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case WEST:
            motor1.set(CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case MIDDLE:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
            break;
          case EAST:
            motor1.set(COUNTER_CLKWISE, 255);
            motor2.set(CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case NORTH_WEST:
            motor1.set(CLKWISE, 255);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, LOW);
            break;
          case NORTH_EAST:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 255);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case SOUTH_WEST:
            motor1.set(COUNTER_CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 0);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, LOW);
            break;
          case SOUTH_EAST:
            motor1.set(COUNTER_CLKWISE, 0);
            motor2.set(COUNTER_CLKWISE, 255);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, HIGH);
            break;
          }

          motor1.regulation();
          motor2.regulation();
        }

        /*--------------------------------*/
      }
      break;
    case 6:
    case 9:
      pushButton1.create(D3_BOARD);
      //IR sensor
      pinMode(A1_BOARD, INPUT);
      pinMode(A2_BOARD, INPUT);
      /*----------obstacle avoidance vehicle------------*/
      while (1)
      {
        startButton.detection(); //Check if the button is pressed.
        if (startButton.isPress())
        {
          if (!avoidObstacleVehicle)
          {
            avoidObstacleVehicle = 1;
            samplingRateForDebouncing = millis();
            //Reset the tasks.
            avoidObstacleInFront = 0;
            avoidObstacleInFront_wait350msFlag = 0;
            avoidObstacleOnTheFloor = 0;
            avoidObstacleOnTheFloor_wait350msFlag = 0;
            //Turn on the LED
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            //Activate the vehicle
            motor1.set(CLKWISE, 255);
            motor2.set(CLKWISE, 255);
          }
          else
          {
            avoidObstacleVehicle = 0;
            //Turn off the LED
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
            //Stop the vehicle
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
          }
        }
        /*--------------------------------*/
        if (avoidObstacleVehicle)
        {
          if ((millis() - samplingRateForDebouncing) > 0) //1ms
          {
            samplingRateForDebouncing = millis();

            /*----Toggle D4_BOARD-----
      if (digitalRead(D4_BOARD))
        digitalWrite(D4_BOARD, LOW);
      else
        digitalWrite(D4_BOARD, HIGH);
        /*--------------------*/
            if (analogRead(A1_BOARD) < 200) //No object in front of the IR sensor.
            {
              analogReadSelflock1 = 0;
              analogReadDebouneCount1 = 0;
            }
            else
            { //(analogRead(A1_BOARD)>200
              //If an obstacle in front of the car is detected,
              //this value will rise.
              if (!analogReadSelflock1)
              {
                analogReadDebouneCount1++;
                if (analogReadDebouneCount1 == 50) //50ms
                {
                  analogReadSelflock1 = 1;
                  avoidObstacleInFront = 1;
                }
              }
            }
            /*-------------------------------------*/
            if (analogRead(A2_BOARD) > 500)
            {
              analogReadSelflock2 = 0;
              analogReadDebouneCount2 = 0;
            }
            else
            { //(analogRead(A2_BOARD)<500
              //If the black stripe under the car is detected,
              //this value will decrease.
              if (!analogReadSelflock2)
              {
                analogReadDebouneCount2++;
                if (analogReadDebouneCount2 == 50) //50ms
                {
                  analogReadSelflock2 = 1;
                  avoidObstacleOnTheFloor = 1;
                }
              }
            }
            /*--------------------------------*/
          }

          if (avoidObstacleInFront)
          { //If an obstacle in front of the car is detected,
            //move backward,then wait for a while.
            avoidObstacleInFront = 0;
            //Indicator
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
            //Move backward.
            motor1.set(COUNTER_CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
            avoidObstacleInFront_wait350msFlag = 1;
            avoidObstacleInFront_turnRight = 1;
            // reset the timer
            avoidObstacleInFront_wait350msTimer = millis();
          }

          if (avoidObstacleInFront_wait350msFlag)
          {
            if ((millis() - avoidObstacleInFront_wait350msTimer) > 350)
            {
              if (avoidObstacleInFront_turnRight)
              {
                avoidObstacleInFront_turnRight = 0; //Execution
                //Turn right after 350ms
                //Indicator
                digitalWrite(D1_BOARD, LOW);
                digitalWrite(D2_BOARD, HIGH);
                //Turn right
                motor1.set(CLKWISE, 255);
                motor2.set(COUNTER_CLKWISE, 255);
                //Wait for another 350ms.
                avoidObstacleInFront_wait350msTimer = millis();
              }
              else
              { //Wait for 350ms,then go straight forward
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, HIGH);
                //Move forward
                motor1.set(CLKWISE, 255);
                motor2.set(CLKWISE, 255);
                avoidObstacleInFront_wait350msFlag = 0;
              }
            }
          }
          /*-----------------------------------------*/
          if (avoidObstacleOnTheFloor)
          {                              //If a black stripe under the car is detected,
                                         //move backward,then wait for a while.
            avoidObstacleOnTheFloor = 0; //Run the program for once.
                                         //Indicator
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
            //Move backward.
            motor1.set(COUNTER_CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
            avoidObstacleOnTheFloor_wait350msFlag = 1;
            avoidObstacleOnTheFloor_turnLeft = 1;
            // reset the timer
            avoidObstacleOnTheFloor_wait350msTimer = millis();
          }

          if (avoidObstacleOnTheFloor_wait350msFlag)
          {
            if ((millis() - avoidObstacleOnTheFloor_wait350msTimer) > 350)
            {
              if (avoidObstacleOnTheFloor_turnLeft)
              {
                avoidObstacleOnTheFloor_turnLeft = 0; //Execution
                //Turn left after 350ms
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, LOW);
                //Turn left
                motor1.set(COUNTER_CLKWISE, 255);
                motor2.set(CLKWISE, 255);
                //Wait for another 350ms.
                avoidObstacleOnTheFloor_wait350msTimer = millis();
              }
              else
              { //Go straight forward after 350ms
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, HIGH);
                //Move forward
                motor1.set(CLKWISE, 255);
                motor2.set(CLKWISE, 255);
                avoidObstacleOnTheFloor_wait350msFlag = 0;
              }
            }
          }

          /*------------------------------------*/
        } //End of  if (avoidObstacleVehicle)

        /*--------------------------------*/
        if (pushButton1.isPress())
        {

          //wait for 150ms
          shaking_wait150msFlag = 1;
          shaking_wait150msTimer = millis();
          shakingState = TURN_LEFT;
          //Indicator
          digitalWrite(D1_BOARD, HIGH);
          digitalWrite(D2_BOARD, LOW);
          //Turn left
          motor1.set(COUNTER_CLKWISE, 255);
          motor2.set(CLKWISE, 255);
        }

        if (pushButton1.isRelease())
        {
          shaking_wait150msFlag = 0;
          //Indicator
          digitalWrite(D1_BOARD, LOW);
          digitalWrite(D2_BOARD, LOW);
          motor1.set(CLKWISE, 0);
          motor2.set(CLKWISE, 0);
        }

        if (shaking_wait150msFlag)
        {
          if ((millis() - shaking_wait150msTimer) > 250) //Exceed 150ms
          {
            shaking_wait150msTimer = millis(); //Set the timer value equal to the current time.
            if (shakingState == TURN_LEFT)
            {
              shakingState = TURN_RIGHT;
              //Indicator
              digitalWrite(D1_BOARD, LOW);
              digitalWrite(D2_BOARD, HIGH);
              motor1.set(CLKWISE, 255);
              motor2.set(COUNTER_CLKWISE, 255);
            }
            else //shakingState=TURN_RIGHT
            {
              shakingState = TURN_LEFT;
              //Indicator
              digitalWrite(D1_BOARD, HIGH);
              digitalWrite(D2_BOARD, LOW);
              //Turn left
              motor1.set(COUNTER_CLKWISE, 255);
              motor2.set(CLKWISE, 255);
            }
          }
        }
        /*----------------------------*/
        motor1.regulation();
        motor2.regulation();
      }
      break;
    case 7:
    /*----------interactive_robot-----------------------*/
      pushButton1.create(D3_BOARD);
      pushButton2.create(D4_BOARD);
      while (1)
      {
        /*-------START the program-------*/
        startButton.detection(); //START button is detected in the main loop.
        if (startButton.isPress())
        {
          interactive_robot = 1;
          startTime = millis();
          //Set the inital value of effective time interval as 2s
          timeInterval = 2000;
          timeIntervalCount = 0;

          //Generate the randomized condition:
          randomNbr = random(10); //Generate a random number from 0 to 9
                                  //Prompt the user
          if (randomNbr < 5)
          {
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, LOW);
          }
          else
          {
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, HIGH);
          }
        }

        /*----run the program--------------*/
        if (interactive_robot)
        {
          if (millis() - startTime < timeInterval) //Within the effective time
          {
            if (randomNbr < 5)
            {
              if (pushButton1.isPress())
              { //Correct,congratulations!
                digitalWrite(D1_BOARD, LOW);
                //Motor1 rotation
                //Move forward,then move backward.
                motor1_rotation = 1;
                motor1_move_forward = 1;
                motor1_runningTime = millis();
                //Move forward
                motor1.set(CLKWISE, 255);
              }
              /*------------------*/
              if (pushButton2.isPress())
              { //Wrong,prompt the user.
                ontone.tone(Tone_data[5][0], 200);
              }
              /*-------------------*/
            }
            else
            { //randomNbr>=5
              if (pushButton1.isPress())
              { //Wrong,prompt the user.
                ontone.tone(Tone_data[5][0], 200);
              }
              /*------------------*/
              if (pushButton2.isPress())
              {
                //Correct,congratulations!
                digitalWrite(D2_BOARD, LOW);
                //Motor2 rotation
                //Move forward,then move backward.
                motor2_rotation = 1;
                motor2_move_forward = 1;
                motor2_runningTime = millis();
                //Move forward
                motor2.set(CLKWISE, 255);
              }
              /*-------------------*/
            }
          }
          else
          { //Exceed the effective time,reset
            if (timeIntervalCount < 5)
              timeIntervalCount++;
            else
            { //Count for 5 times,then decrease the time interval.
              timeIntervalCount = 0;
              if (timeInterval > 500)
                timeInterval -= 100; //Decrease 100 for every 5 time intervals.
            }
            startTime = millis();   //Reset the start time.
                                    //Reset the true condition:
            randomNbr = random(10); //Generate a random number from 0 to 9
                                    //Prompt the user
            if (randomNbr < 5)
            {
              digitalWrite(D1_BOARD, HIGH);
              digitalWrite(D2_BOARD, LOW);
            }
            else
            {
              digitalWrite(D1_BOARD, LOW);
              digitalWrite(D2_BOARD, HIGH);
            }
          }

          if (motor1_rotation)
          {
            if (millis() - motor1_runningTime > 200) //exceed 200ms
            {
              if (motor1_move_forward)
              {
                motor1_move_forward = 0;
                //Move backward.
                motor1.set(COUNTER_CLKWISE, 255);
                //Reset the running time.
                motor1_runningTime = millis();
              }
              else
              {
                //digitalWrite(D1_BOARD,HIGH);
                //Stop
                motor1.set(CLKWISE, 0);
                motor1_rotation = 0;
              }
            }
          }

          if (motor2_rotation)
          {
            if (millis() - motor2_runningTime > 200) //exceed 200ms
            {
              if (motor2_move_forward)
              {
                motor2_move_forward = 0;
                //Move backward.
                motor2.set(COUNTER_CLKWISE, 255);
                //Reset the running time.
                motor2_runningTime = millis();
              }
              else
              {
                //digitalWrite(D2_BOARD,HIGH);
                //Stop
                motor2.set(COUNTER_CLKWISE, 0);
                motor2_rotation = 0;
              }
            }
          }
          /*------------------------------------------*/
          motor1.regulation();
          motor2.regulation();
        }
      }
      break;
    }
  }
}
