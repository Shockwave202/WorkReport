
#include "Board.h"
#include "PushButton_Simple.h"
#include "PushButton.h" //Use timer2
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
unsigned long samplingRateForDebouncing;
//IR_Sensor front
bool analogReadSelflock1;
uint8_t analogReadDebouneCount1;
bool frontSensorIsTriggered;
bool avoidObstacleFront;
bool avoidObstacleFront_turnRight;
unsigned long avoidObstacleFront_wait350ms;

//IR_Sensor below
bool analogReadSelflock2;
uint8_t analogReadDebouneCount2;
bool belowSensorIsTriggered;
bool avoidObstacleBelow;
bool avoidObstacleBelow_turnLeft;
unsigned long avoidObstacleBelow_wait350ms;

//Shaking
#define TURN_LEFT 0
#define TURN_RIGHT 1
bool shaking_wait150msFlag;
unsigned long shaking_wait150msTimer;
bool shakingState;

//interactive_robot
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

//avoidObstacleVehicle M2
bool avoidObstacleLeft;
//During the first period,the car moves backward;
//during the second period,the car turns right.
bool avoidObstacleLeft_turnRight;
//Indicate that the car turns right.
unsigned long avoidObstacleLeft_wait350ms;

bool avoidObstacleRight;
//During the first period,the car moves backward;
//during the second period,the car turns left.
bool avoidObstacleRight_turnLeft;
//Indicate that the car turns left.
unsigned long avoidObstacleRight_wait350ms;

//rubberBandCannon
bool rubberBandCannon;
bool ObstacleIsDetected;

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
{
  // put your setup code here, to run once:
  //The startup time is approximate to 1.6S.
  LABBOT_Board.initial_all_ioports();
  //Serial.begin(9600);//For program test.

  /*----Common modules applied to all the subprograms.---*/
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
    //Prompt the user that the subprogram has been selected.
    music();
    //The initalization of each individual subprogram.
    switch (pgm_index)
    {
    case 1:
      /*------propeller-driven aircraft-------*/
      //aircraft = 0;
      while (1)
      {                          // put your main code here, to run repeatedly:
        startButton.detection(); //Check if the button is pressed.
        /*--START the program--*/
        if (startButton.isPress())
        {
          //Propeller-driven aircraft.
          aircraft = 1;
        }

        /*--Execute the program---*/
        if (aircraft)
        {
          //Activate the motors
          //Forward:
          motor1.set(CLKWISE, 255);
          motor2.set(COUNTER_CLKWISE, 255);
          //Motor1 and Motor2 are located at the opposite
          //physical position,thus,when both rotate in reverse
          //direction,the vehicle will go forward/backward;
          //when both rotate in the same direction,the vehicle
          //will turn left/right.
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
          //Backward:
          motor1.set(COUNTER_CLKWISE, 255);
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
        }
      }
      break;
    case 2:
      /*----------frog------------------------------*/
      pushButton1.create(D3_BOARD);
      //frog_mode = 0;
      //frog_is_jumping = 0;
      while (1)
      {
        /*-------START the program-------*/
        startButton.detection(); //START button is detected in the main loop.
        if (startButton.isPress())
        {
          //If mode has been selected,start the program.
          if (frog_mode)
            frog_is_jumping = 1;
        }

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

        if (frog_is_jumping)
        {
          digitalWrite(D1_BOARD, HIGH); //Enlight LEDs
          digitalWrite(D2_BOARD, HIGH);

          //Forward:
          motor1.set(CLKWISE, 255);
          motor2.set(COUNTER_CLKWISE, 255);
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
          //Stop
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
          if (!self_tracing_vehicle)
            self_tracing_vehicle = 1;
          else
          {
            self_tracing_vehicle = 0;
            //Stop:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
          }
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
            //Forward:
            motor1.set(CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
          }
          else if (analogRead(A1_BOARD) > 900 && analogRead(A2_BOARD) < 500)
          { //Right IR sensor has detected the black stripe,left IR sensor has detected the white floor,
            //turn left under this condition.
            //Turn left:
            motor1.set(COUNTER_CLKWISE, 255); //Left wheel
            motor2.set(COUNTER_CLKWISE, 255); //Right wheel
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, LOW);
          }
          else if (analogRead(A1_BOARD) < 500 && analogRead(A2_BOARD) > 900)
          {
            //Left IR sensor has detected the black stripe,right IR sensor has detected the white floor,
            //turn right under this condition.
            //Turn right:
            motor1.set(CLKWISE, 255); //Left wheel
            motor2.set(CLKWISE, 255); //Right wheel
            digitalWrite(D2_BOARD, HIGH);
            digitalWrite(D1_BOARD, LOW);
          }

          else if (analogRead(A1_BOARD) < 500 && analogRead(A2_BOARD) < 500)
          { //Right IR sensor has detected the black stripe,left IR sensor has detected the black stripe,
            //stop under this condition.
            //Stop:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
          }
        }
        motor1.regulation();
        motor2.regulation();
        /*----------------------------------------------*/
      }
      break;
    case 4:
    case 5:
    case 8:
    case 11:
      //Input:
      //Joystick
      joystick.attach(A3_BOARD, A4_BOARD);
      //joystickCtrlledVehicle = 0;
      while (1)
      {
        /*-----------------------------------------*/
        startButton.detection(); //Check if the button is pressed.
        /*--START the program--*/
        if (startButton.isPress())
        {
          if (!joystickCtrlledVehicle)
            joystickCtrlledVehicle = 1;
          else
          {
            joystickCtrlledVehicle = 0;
            //Stop:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
          }
        }

        /*--Run the program--*/
        if (joystickCtrlledVehicle)
        {
          switch (joystick.getResult())
          {
          case NORTH:
            //Forward:
            motor1.set(CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case SOUTH:
            //Backward:
            motor1.set(COUNTER_CLKWISE, 255);
            motor2.set(CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case WEST:
          case NORTH_WEST:
          case SOUTH_EAST:
            //Turn left
            motor1.set(COUNTER_CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, LOW);
            break;
          case EAST:
          case NORTH_EAST:
          case SOUTH_WEST:
            //Turn right
            motor1.set(CLKWISE, 255);
            motor2.set(CLKWISE, 255);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case MIDDLE:
            //Stop:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
            break;
          }
        }
        motor1.regulation();
        motor2.regulation();
        /*--------------------------------*/
      }
      break;
    case 6:
      pushButton1.create(D3_BOARD);
      //IR sensor
      pinMode(A1_BOARD, INPUT);
      pinMode(A2_BOARD, INPUT);
      /*----------obstacle avoidance vehicle------------*/
      while (1)
      {
        // put your main code here, to run repeatedly:
        startButton.detection(); //Check if the button is pressed.
        if (startButton.isPress())
        {
          if (!avoidObstacleVehicle)
          { /*---Start the program---*/
            avoidObstacleVehicle = 1;
            samplingRateForDebouncing = millis();
            //Reset the tasks.
            frontSensorIsTriggered = 0;
            avoidObstacleFront = 0;
            avoidObstacleFront_turnRight = 0;

            belowSensorIsTriggered = 0;
            avoidObstacleBelow = 0;
            avoidObstacleBelow_turnLeft = 0;

            //Turn on the LED
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            //Forward:
            motor1.set(CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
          }
          else
          { /*---Stop the program---*/
            avoidObstacleVehicle = 0;
            //Turn off the LED
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
            //Stop:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
          }
        }

        /*-----Run the program---------*/
        if (avoidObstacleVehicle)
        {
          if ((millis() - samplingRateForDebouncing) > 0) //Count for 1ms
          {
            samplingRateForDebouncing = millis();

            //Front sensor detection
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
                  frontSensorIsTriggered = 1;
                }
              }
            }

            //Below sensor detection
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
                  belowSensorIsTriggered = 1;
                }
              }
            }
          }

          //If the front sensor is triggered,take the corresponding
          //obstacle avoidance action.
          if (frontSensorIsTriggered)
          {
            frontSensorIsTriggered = 0;
            //If the obstacle avoidance action is already active,
            //ignore the sensor signal.
            if (!avoidObstacleFront)
            {
              //Indicator
              digitalWrite(D1_BOARD, LOW);
              digitalWrite(D2_BOARD, LOW);
              //Backward:
              motor1.set(COUNTER_CLKWISE, 255);
              motor2.set(CLKWISE, 255);
              avoidObstacleFront = 1;
              avoidObstacleFront_turnRight = 0;
              // reset the timer
              avoidObstacleFront_wait350ms = millis();
            }
          }

          //obstcle avoidance action.
          if (avoidObstacleFront)
          {
            if ((millis() - avoidObstacleFront_wait350ms) > 350)
            {
              if (avoidObstacleFront_turnRight == 0)
              {
                avoidObstacleFront_turnRight = 1;
                //Indicator
                digitalWrite(D1_BOARD, LOW);
                digitalWrite(D2_BOARD, HIGH);
                //Turn right
                motor1.set(CLKWISE, 255);
                motor2.set(CLKWISE, 255);
                //Wait for another 350ms.
                avoidObstacleFront_wait350ms = millis();
              }
              else //avoidObstacleFront_turnRight=1;
              {
                avoidObstacleFront_turnRight = 0;
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, HIGH);
                //Forward:
                motor1.set(CLKWISE, 255);
                motor2.set(COUNTER_CLKWISE, 255);
                avoidObstacleFront = 0;
              }
            }
          }

          //If the below sensor is triggered,take the corresponding
          //obstacle avoidance action.
          if (belowSensorIsTriggered)
          {
            belowSensorIsTriggered = 0;
            if (!avoidObstacleBelow)
            {
              //Indicator
              digitalWrite(D1_BOARD, LOW);
              digitalWrite(D2_BOARD, LOW);
              //Backward:
              motor1.set(COUNTER_CLKWISE, 255);
              motor2.set(CLKWISE, 255);
              avoidObstacleBelow = 1;
              avoidObstacleBelow_turnLeft = 0;
              // reset the timer
              avoidObstacleBelow_wait350ms = millis();
            }
          }

          //obstacle avoidance action
          if (avoidObstacleBelow)
          {
            if ((millis() - avoidObstacleBelow_wait350ms) > 350)
            {
              if (avoidObstacleBelow_turnLeft == 0)
              {
                avoidObstacleBelow_turnLeft = 1;
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, LOW);
                //Turn left
                motor1.set(COUNTER_CLKWISE, 255);
                motor2.set(COUNTER_CLKWISE, 255);
                //Wait for another 350ms.
                avoidObstacleBelow_wait350ms = millis();
              }
              else //avoidObstacleBelow_turnLeft = 1;
              {
                avoidObstacleBelow_turnLeft = 0;
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, HIGH);
                //Forward:
                motor1.set(CLKWISE, 255);
                motor2.set(COUNTER_CLKWISE, 255);
                avoidObstacleBelow = 0;
              }
            }
          }
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
          motor2.set(COUNTER_CLKWISE, 255);
        }

        if (pushButton1.isRelease())
        {
          shaking_wait150msFlag = 0;
          //Indicator
          digitalWrite(D1_BOARD, LOW);
          digitalWrite(D2_BOARD, LOW);
          //Stop:
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
              //Turn right
              motor1.set(CLKWISE, 255);
              motor2.set(CLKWISE, 255);
            }
            else //shakingState=TURN_RIGHT
            {
              shakingState = TURN_LEFT;
              //Indicator
              digitalWrite(D1_BOARD, HIGH);
              digitalWrite(D2_BOARD, LOW);
              //Turn left
              motor1.set(COUNTER_CLKWISE, 255);
              motor2.set(COUNTER_CLKWISE, 255);
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
                motor2.set(COUNTER_CLKWISE, 255);
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
                motor1.set(COUNTER_CLKWISE, 0);
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
                motor2.set(CLKWISE, 255);
                //Reset the running time.
                motor2_runningTime = millis();
              }
              else
              {
                //digitalWrite(D2_BOARD,HIGH);
                //Stop
                motor2.set(CLKWISE, 0);
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
    case 9:
      /*------------avoidObstacleVehicle M2-------------*/
      //Input:
      pushButton1.create(D3_BOARD);
      pushButton2.create(D4_BOARD);
      while (1)
      {

        //Check if the button is pressed.
        startButton.detection();
        if (startButton.isPress())
        {
          if (!avoidObstacleVehicle)
          { /*---Start the program---*/
            avoidObstacleVehicle = 1;
            //Reset the tasks.
            avoidObstacleLeft = 0;
            avoidObstacleLeft_turnRight = 0;
            //avoidObstacleLeft_wait350ms=millis();

            avoidObstacleRight = 0;
            avoidObstacleRight_turnLeft = 0;
            //avoidObstacleRight_wait350ms=millis();

            //Turn on the LED
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            //Forward:
            motor1.set(CLKWISE, 255);
            motor2.set(COUNTER_CLKWISE, 255);
          }
          else
          { /*---Stop the program---*/
            avoidObstacleVehicle = 0;
            //Turn off the LED
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
            //Stop:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
          }
        }
        /*-------Run the program-----------*/
        if (avoidObstacleVehicle)
        {
          //Left obstacle detection
          if (pushButton1.isPress())
          { //If the obstacle avoidance program hasn't been triggered.
            if (!avoidObstacleLeft)
            {
              //Indicator
              digitalWrite(D1_BOARD, LOW);
              digitalWrite(D2_BOARD, LOW);
              //Backward:
              motor1.set(COUNTER_CLKWISE, 255);
              motor2.set(CLKWISE, 255);
              avoidObstacleLeft = 1;
              avoidObstacleLeft_turnRight = 0;
              // reset the timer
              avoidObstacleLeft_wait350ms = millis();
            }
          }

          //Obstacle avoidance action.
          if (avoidObstacleLeft)
          {
            if ((millis() - avoidObstacleLeft_wait350ms) > 350)
            {
              if (avoidObstacleLeft_turnRight == 0)
              {
                avoidObstacleLeft_turnRight = 1;
                //Indicator
                digitalWrite(D1_BOARD, LOW);
                digitalWrite(D2_BOARD, HIGH);
                //Turn right
                motor1.set(CLKWISE, 255);
                motor2.set(CLKWISE, 255);
                //Wait for another 350ms.
                avoidObstacleLeft_wait350ms = millis();
              }
              else //avoidObstacleLeft_turnRight=1;
              {
                avoidObstacleLeft_turnRight = 0;
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, HIGH);
                //Forward:
                motor1.set(CLKWISE, 255);
                motor2.set(COUNTER_CLKWISE, 255);
                avoidObstacleLeft = 0;
              }
            }
          }

          //Right obstacle detection
          if (pushButton2.isPress())
          { //If the obstacle avoidance program hasn't been triggered...
            if (!avoidObstacleRight)
            {
              digitalWrite(D1_BOARD, LOW);
              digitalWrite(D2_BOARD, LOW);
              //Backward:
              motor1.set(COUNTER_CLKWISE, 255);
              motor2.set(CLKWISE, 255);
              avoidObstacleRight = 1;
              avoidObstacleRight_turnLeft = 0;
              // reset the timer
              avoidObstacleRight_wait350ms = millis();
            }
          }

          //Obstacle avoidance action.
          if (avoidObstacleRight)
          {
            if ((millis() - avoidObstacleRight_wait350ms) > 350)
            {
              if (avoidObstacleRight_turnLeft == 0)
              {
                avoidObstacleRight_turnLeft = 1;
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, LOW);
                //Turn left
                motor1.set(COUNTER_CLKWISE, 255);
                motor2.set(COUNTER_CLKWISE, 255);
                //Wait for another 350ms.
                avoidObstacleRight_wait350ms = millis();
              }
              else //avoidObstacleRight_turnLeft=1;
              {
                avoidObstacleRight_turnLeft = 0;
                //Indicator
                digitalWrite(D1_BOARD, HIGH);
                digitalWrite(D2_BOARD, HIGH);
                //Forward:
                motor1.set(CLKWISE, 255);
                motor2.set(COUNTER_CLKWISE, 255);
                avoidObstacleRight = 0;
              }
            }
          }
        }
        else //avoidObstacleVehicle=0;
        {
          if (pushButton1.isPress())
          {
            //do nothing
          }
          if (pushButton2.isPress())
          {
            //do nothing
          }
        }
        /*----------------------------*/
        motor1.regulation();
        motor2.regulation();
      }
      break;
    case 10:
      /*------------rubberBandCannon-------------*/
      //Input:
      pushButton1.create(D3_BOARD);
      //IR sensor
      pinMode(A1_BOARD, INPUT);
      //Joystick
      joystick.attach(A3_BOARD, A4_BOARD);
      //rubberBandCannon=0;
      while (1)
      {
        // put your main code here, to run repeatedly:
        startButton.detection(); //Check if the button is pressed.
        /*-----Fcn:These codes are used for program selection.-----*/
        if (startButton.isPress())
        { /*---Start the program---*/
          if (!rubberBandCannon)
          {
            rubberBandCannon = 1;
            motor1_rotation = 0;
            motor1_move_forward = 0;
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
          }
          else
          { /*---Stop the program---*/
            rubberBandCannon = 0;
            //Stop:
            motor1.set(CLKWISE, 0);
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, LOW);
          }
        }

        if (rubberBandCannon)
        {
          //Launch parts(Motor1)
          if (pushButton1.isPress())
          {
            //Motor1 moves forward ,then moves backward.
            // if (!motor1_rotation && analogRead(A1_BOARD) < 200)
            if (!motor1_rotation)
            {
              //If Motor1 is stop,and no obstacle is been detected,
              //launch the motor.
              motor1_rotation = 1;
              motor1_move_forward = 1;
              motor1_runningTime = millis(); //Reset the timer.
              //Move forward
              motor1.set(COUNTER_CLKWISE, 255);
            }
          }

          if (motor1_rotation)
          {
            if (motor1_move_forward)
            {
              if (millis() - motor1_runningTime > 150) //count for 150ms
              {
                motor1_move_forward = 0;
                //Move backward.
                motor1.set(CLKWISE, 100);
                //Initialization for IR_sensor detection.
                samplingRateForDebouncing = millis(); //Reset the timer
                analogReadSelflock1 = 0;
                analogReadDebouneCount1 = 0;
                ObstacleIsDetected = 0;
              }
            }
            else
            { //motor1_move_backward
              //IR sensor detection.
              //If the analog voltage level rises on this pin,
              //the rising edge will be capatured.
              if ((millis() - samplingRateForDebouncing) > 0) //1ms
              {
                samplingRateForDebouncing = millis();
                if (analogRead(A1_BOARD) < 500) //No obstacle.
                {
                  analogReadSelflock1 = 0;
                  analogReadDebouneCount1 = 0;
                }
                else
                { //(analogRead(A1_BOARD)>500
                  //If an obstacle is detected,this value will rise.
                  if (!analogReadSelflock1)
                  {
                    analogReadDebouneCount1++;
                    if (analogReadDebouneCount1 == 5) //5ms
                    {
                      analogReadSelflock1 = 1;
                      ObstacleIsDetected = 1;
                    }
                  }
                }
              }

              //If the obstacle is detected,stopped the motor
              if (ObstacleIsDetected)
              {
                ObstacleIsDetected = 0;
                motor1_rotation = 0;
                //Stop Motor1
                motor1.set(CLKWISE, 0);
              }
            }
          }

          //Rotation parts(Motor2)
          switch (joystick.getResult())
          {
          case WEST:
          case NORTH_WEST:
          case SOUTH_WEST:
            //Turn left
            motor2.set(COUNTER_CLKWISE, 255);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, LOW);
            break;
          case EAST:
          case NORTH_EAST:
          case SOUTH_EAST:
            //Turn right
            motor2.set(CLKWISE, 255);
            digitalWrite(D1_BOARD, LOW);
            digitalWrite(D2_BOARD, HIGH);
            break;
          case NORTH:
          case SOUTH:
          case MIDDLE:
            //Stop:
            motor2.set(CLKWISE, 0);
            digitalWrite(D1_BOARD, HIGH);
            digitalWrite(D2_BOARD, HIGH);
            break;
          }
        }
        else
        {
          //rubber band cannon hasn't been activated.
          if (pushButton1.isPress())
          { //do nothing
          }
        }
        motor1.regulation();
        motor2.regulation();
      }
      break;
    }
  }
}
