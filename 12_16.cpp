//Interactive robot.
//Based on the different response of the user,
//the robot will take corresponding actions.

#include "Board.h"
#include "Pushbutton.h"
#include "METALROBO_Module.h" //For buzzer

//Create objs.
Board LABBOT_Board;
//Input:
Pushbutton Pushbutton1, Pushbutton2;
//Output:
#define CLKWISE 1
#define COUNTER_CLKWISE 0
uint16_t motorSpeedRegulationRate;
//Motor1 Ctrl
bool motor1_current_direction;
bool motor1_expected_direction;
uint8_t motor1_current_speed;
uint8_t motor1_expected_speed;
uint8_t motor1_complementary_speed;
//Motor2 Ctrl
bool motor2_current_direction;
bool motor2_expected_direction;
uint8_t motor2_current_speed;
uint8_t motor2_expected_speed;
uint8_t motor2_complementary_speed;
//Create a buzzer obj.
buzzer ontone;
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

void setup()
{
  // put your setup code here, to run once:
  //The startup time is approximate to 1.6S.
  LABBOT_Board.initial_all_ioports();
  Serial.begin(9600);
  //INPUT:
  Pushbutton1.create(D3_BOARD);
  Pushbutton2.create(D4_BOARD);
  //Output:
  //Indicators:
  pinMode(D1_BOARD, OUTPUT); //Attach LED1 to D1 pin
  digitalWrite(D1_BOARD, LOW);
  pinMode(D2_BOARD, OUTPUT); //Attach LED2 to D2 pin
  digitalWrite(D2_BOARD, LOW);
  //Actuators:
  //Motor1
  motor1_current_direction = CLKWISE;
  motor1_expected_direction = CLKWISE;
  motor1_current_speed = 0;
  motor1_expected_speed = 0;
  motor1_complementary_speed = 0;
  digitalWrite(MOTOR1_INB, LOW);
  analogWrite(MOTOR1_INA, motor1_current_speed);
  //Motor2
  motor2_current_direction = CLKWISE;
  motor2_expected_direction = CLKWISE;
  motor2_current_speed = 0;
  motor2_expected_speed = 0;
  motor2_complementary_speed = 0;
  digitalWrite(MOTOR2_INB, LOW);
  analogWrite(MOTOR2_INA, motor2_current_speed);

  startTime = millis();
  //Set the inital value of effective time interval as 5s
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

  ontone.port(BEEP);
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (millis() - startTime < timeInterval) //Within the effective time
  {
    if (randomNbr < 5)
    {
      if (Pushbutton1.isPress())
      { //Correct,congratulations!
        digitalWrite(D1_BOARD, LOW);
        //Motor1 rotation
        //Move forward,then move backward.
        motor1_rotation = 1;
        motor1_move_forward = 1;
        motor1_runningTime = millis();
        //Move forward
        motor1_expected_direction = CLKWISE;
        motor1_expected_speed = 255;
      }
      /*------------------*/
      if (Pushbutton2.isPress())
      { //Wrong,prompt the user.
        ontone.tone(Tone_data[5][0], 200);
      }
      /*-------------------*/
    }
    else
    { //randomNbr>=5
      if (Pushbutton1.isPress())
      { //Wrong,prompt the user.
        ontone.tone(Tone_data[5][0], 200);
      }
       /*------------------*/
      if (Pushbutton2.isPress())
      {
        //Correct,congratulations!
        digitalWrite(D2_BOARD, LOW);
        //Motor2 rotation
        //Move forward,then move backward.
        motor2_rotation = 1;
        motor2_move_forward = 1;
        motor2_runningTime = millis();
        //Move forward
        motor2_expected_direction = CLKWISE;
        motor2_expected_speed = 255;
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
        motor1_expected_direction = COUNTER_CLKWISE;
        motor1_expected_speed = 255;
        //Reset the running time.
        motor1_runningTime = millis();
      }
      else
      {
        //digitalWrite(D1_BOARD,HIGH);
        //Stop
        motor1_expected_direction = CLKWISE;
        motor1_expected_speed = 0;
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
        motor2_expected_direction = COUNTER_CLKWISE;
        motor2_expected_speed = 255;
        //Reset the running time.
        motor2_runningTime = millis();
      }
      else
      {
        //digitalWrite(D2_BOARD,HIGH);
        //Stop
        motor2_expected_direction = COUNTER_CLKWISE;
        motor2_expected_speed = 0;
        motor2_rotation = 0;
      }
    }
  }

  /*------------------------------------------*/

  motorSpeedRegulationRate++;
  if (motorSpeedRegulationRate == 160) //1ms
  {
    motorSpeedRegulationRate = 0;
    /*----Toggle D4_BOARD-----*/
      if (digitalRead(D4_BOARD))
        digitalWrite(D4_BOARD, LOW);
      else
        digitalWrite(D4_BOARD, HIGH);
        /*--------------------*/

    /*--------------------MOTOR1--------------------*/
    if (motor1_current_direction == CLKWISE)
    {
      //Direction changing
      if (motor1_expected_direction == COUNTER_CLKWISE)
      { //Slow down
        if (motor1_current_speed > 0)
        {
          motor1_current_speed -= 5;
          analogWrite(MOTOR1_INA, motor1_current_speed);
        }
        else
        {
          motor1_current_direction = COUNTER_CLKWISE;
          motor1_current_speed = 255;
          motor1_complementary_speed = 255 - motor1_expected_speed;
          digitalWrite(MOTOR1_INB, HIGH);
          analogWrite(MOTOR1_INA, motor1_complementary_speed);
        }
      }
      else
      {
        //Speed regulation
        if (motor1_current_speed < motor1_expected_speed)
        {
          motor1_current_speed += 5;
          analogWrite(MOTOR1_INA, motor1_current_speed);
        }
        else if (motor1_current_speed > motor1_expected_speed)
        {
          motor1_current_speed -= 5;
          analogWrite(MOTOR1_INA, motor1_current_speed);
        }
      }
    }
    else //motor1_current_direction=COUNTER_CLKWISE
    {
      //Direction changing
      if (motor1_expected_direction == CLKWISE)
      { //Slow down
        if (motor1_current_speed < 255)
        {
          motor1_current_speed += 5;
          analogWrite(MOTOR1_INA, motor1_current_speed);
        }
        else
        {
          motor1_current_direction = CLKWISE;
          motor1_current_speed = 0;
          digitalWrite(MOTOR1_INB, LOW);
          analogWrite(MOTOR1_INA, motor1_current_speed);
        }
      }
      else
      {
        //Speed regulation
        motor1_complementary_speed = 255 - motor1_expected_speed;
        if (motor1_current_speed < motor1_complementary_speed)
        {
          motor1_current_speed += 5;
          analogWrite(MOTOR1_INA, motor1_current_speed);
        }
        else if (motor1_current_speed > motor1_complementary_speed)
        {
          motor1_current_speed -= 5;
          analogWrite(MOTOR1_INA, motor1_current_speed);
        }
      }
    }
    /*-------------------MOTOR2-------------------------------*/
    if (motor2_current_direction == CLKWISE)
    {
      //Direction changing
      if (motor2_expected_direction == COUNTER_CLKWISE)
      { //Slow down
        if (motor2_current_speed > 0)
        {
          motor2_current_speed -= 5;
          analogWrite(MOTOR2_INA, motor2_current_speed);
        }
        else
        {
          motor2_current_direction = COUNTER_CLKWISE;
          motor2_current_speed = 255;
          motor2_complementary_speed = 255 - motor2_expected_speed;
          digitalWrite(MOTOR2_INB, HIGH);
          analogWrite(MOTOR2_INA, motor2_complementary_speed);
        }
      }
      else
      {
        //Speed regulation
        if (motor2_current_speed < motor2_expected_speed)
        {
          motor2_current_speed += 5;
          analogWrite(MOTOR2_INA, motor2_current_speed);
        }
        else if (motor2_current_speed > motor2_expected_speed)
        {
          motor2_current_speed -= 5;
          analogWrite(MOTOR2_INA, motor2_current_speed);
        }
      }
    }
    else //motor2_current_direction=COUNTER_CLKWISE
    {
      //Direction changing
      if (motor2_expected_direction == CLKWISE)
      { //Slow down
        if (motor2_current_speed < 255)
        {
          motor2_current_speed += 5;
          analogWrite(MOTOR2_INA, motor2_current_speed);
        }
        else
        {
          motor2_current_direction = CLKWISE;
          motor2_current_speed = 0;
          digitalWrite(MOTOR2_INB, LOW);
          analogWrite(MOTOR2_INA, motor2_current_speed);
        }
      }
      else
      {
        //Speed regulation
        motor2_complementary_speed = 255 - motor2_expected_speed;
        if (motor2_current_speed < motor2_complementary_speed)
        {
          motor2_current_speed += 5;
          analogWrite(MOTOR2_INA, motor2_current_speed);
        }
        else if (motor2_current_speed > motor2_complementary_speed)
        {
          motor2_current_speed -= 5;
          analogWrite(MOTOR2_INA, motor2_current_speed);
        }
      }
    }
    /*-----------------------------------------------*/
  }
}
