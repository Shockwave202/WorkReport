//Verify each parts,test if everything works fine,
//then assemble the whole stuff.

//Generate a square wave of 1KHZ using timer2.
//---------------------------------------------------------
// Timer2 (8 bits)
#include "Board.h"
#include "Pushbutton.h"

#include <METALROBO_Module.h>
#include <METALROBO_up.h>
#include <Servo.h>

//Create objs.
Board LABBOT_Board;

//Input:
/*-----------Smoothing------------------------------*/
/*
  Smoothing

  Reads repeatedly from an analog input, calculating a running average and
  printing it to the computer. Keeps ten readings in an array and continually
  averages them.

  The circuit:
  - analog sensor (potentiometer will do) attached to analog input 0
*/

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 16;

int joystick_x_readings[numReadings]; // the readings from the analog input
int joystick_x_readIndex;             // the index of the current reading
int joystick_x_total;                 // the running total
int joystick_x_average;               // the average
int joystick_x_readings_count;

int joystick_y_readings[numReadings]; // the readings from the analog input
int joystick_y_readIndex;             // the index of the current reading
int joystick_y_total;                 // the running total
int joystick_y_average;               // the average
int joystick_y_readings_count;
//count the readings until the array is filled up,then start output.
uint16_t joystick_sampling_rate;
#define LOW_LEVEL 0
#define MID_LEVEL 1
#define HIGH_LEVEL 2
uint8_t joystick_x_level;
uint8_t joystick_x_low_level_debounce_counter;
uint8_t joystick_x_high_level_debounce_counter;
uint8_t joystick_y_level;
uint8_t joystick_y_low_level_debounce_counter;
uint8_t joystick_y_high_level_debounce_counter;
#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4
#define NORTH_EAST 5
#define NORTH_WEST 6
#define SOUTH_EAST 7
#define SOUTH_WEST 8
#define MIDDLE 0
uint8_t joystick_direction;

//Output:
bool self_tracing_vehicle;
uint16_t count_1MS;
//Motor1 Ctrl
#define CLKWISE 1
#define COUNTER_CLKWISE 0
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

void setup()
{ //The startup time is approximate to 1.6S.
  // put your setup code here, to run once:
  LABBOT_Board.initial_all_ioports();
  Serial.begin(9600);
  /*---------------Setup for smoothing filter------------------------------*/
  joystick_x_readIndex = 0; // the index of the current reading
  joystick_x_total = 0;     // the running total
  joystick_x_average = 0;   // the average
  joystick_x_readings_count = 0;
  //count the readings until the array is filled up,then start output.
  joystick_y_readIndex = 0; // the index of the current reading
  joystick_y_total = 0;     // the running total
  joystick_y_average = 0;   // the average
  joystick_y_readings_count = 0;
  //count the readings until the array is filled up,then start output.
  joystick_sampling_rate = 0;
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    joystick_x_readings[thisReading] = 0;
    joystick_y_readings[thisReading] = 0;
  }
  //joystick_x_level=MID_LEVEL;
  joystick_x_low_level_debounce_counter = 0;
  joystick_x_high_level_debounce_counter = 0;
  //joystick_y_level=MID_LEVEL;
  joystick_y_low_level_debounce_counter = 0;
  joystick_y_high_level_debounce_counter = 0;

//Actuators:
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
}

void loop()
{
  // put your main code here, to run repeatedly:

  /*-----------------Loop for smoothing filter---------------*/
  joystick_sampling_rate++;
  if (joystick_sampling_rate == 1000) //Approximate to 1ms(1000).
  {
    joystick_sampling_rate = 0;
    /*----Toggle D4_BOARD-----
    if (digitalRead(D4_BOARD))
      digitalWrite(D4_BOARD, LOW);
    else
      digitalWrite(D4_BOARD, HIGH);
    /*----------joystick_x -----------*/
    // subtract the last reading:
    joystick_x_total = joystick_x_total - joystick_x_readings[joystick_x_readIndex];
    // read from the sensor:
    joystick_x_readings[joystick_x_readIndex] = analogRead(A3_BOARD);
    // add the reading to the total:
    joystick_x_total = joystick_x_total + joystick_x_readings[joystick_x_readIndex];
    // advance to the next position in the array:
    joystick_x_readIndex = joystick_x_readIndex + 1;

    // if we're at the end of the array...
    if (joystick_x_readIndex >= numReadings)
    {
      // ...wrap around to the beginning:
      joystick_x_readIndex = 0;
    }

    if (joystick_x_readings_count < numReadings)
    {
      joystick_x_readings_count++;
      //  Serial.print("readings_count:");
      // Serial.println(readings_count);
    }
    else
    {
      // calculate the average:
      joystick_x_average = joystick_x_total / numReadings;
      // send it to the computer as ASCII digits
      //  Serial.print("average:");
      //  Serial.println(average);
      //If the result needs to be sent to the PC after
      //each read,at least 14ms is required.
      if (joystick_x_average < 350)
      {
        if (joystick_x_level != LOW_LEVEL)
        { //start debouncing
          joystick_x_low_level_debounce_counter++;
          if (joystick_x_low_level_debounce_counter == 25) //25ms*2 channel(x_channel,y_channel)=50ms
            joystick_x_level = LOW_LEVEL;
        }
      }
      else if (joystick_x_average > 700)
      {
        if (joystick_x_level != HIGH_LEVEL)
        {
          joystick_x_high_level_debounce_counter++;
          if (joystick_x_high_level_debounce_counter == 25) //25ms*2 channel(x_channel,y_channel)=50ms
            joystick_x_level = HIGH_LEVEL;
        }
      }
      else
      {
        joystick_x_low_level_debounce_counter = 0;
        joystick_x_high_level_debounce_counter = 0;
        joystick_x_level = MID_LEVEL;
      }
    }

    /*--------------------------joystick_y ------------------*/
    // subtract the last reading:
    joystick_y_total = joystick_y_total - joystick_y_readings[joystick_y_readIndex];
    // read from the sensor:
    joystick_y_readings[joystick_y_readIndex] = analogRead(A4_BOARD);
    // add the reading to the total:
    joystick_y_total = joystick_y_total + joystick_y_readings[joystick_y_readIndex];
    // advance to the next position in the array:
    joystick_y_readIndex = joystick_y_readIndex + 1;

    // if we're at the end of the array...
    if (joystick_y_readIndex >= numReadings)
    {
      // ...wrap around to the beginning:
      joystick_y_readIndex = 0;
    }

    if (joystick_y_readings_count < numReadings)
    {
      joystick_y_readings_count++;
      //  Serial.print("readings_count:");
      // Serial.println(readings_count);
    }
    else
    {
      // calculate the average:
      joystick_y_average = joystick_y_total / numReadings;
      // send it to the computer as ASCII digits
      //  Serial.print("average:");
      //  Serial.println(average);
      //If the result needs to be sent to the PC after
      //each read,at least 14ms is required.
      if (joystick_y_average < 350)
      {
        if (joystick_y_level != LOW_LEVEL)
        { //start debouncing
          joystick_y_low_level_debounce_counter++;
          if (joystick_y_low_level_debounce_counter == 25) //25ms*2 channel(x_channel,y_channel)=50ms
            joystick_y_level = LOW_LEVEL;
        }
      }
      else if (joystick_y_average > 700)
      {
        if (joystick_y_level != HIGH_LEVEL)
        {
          joystick_y_high_level_debounce_counter++;
          if (joystick_y_high_level_debounce_counter == 25) //25ms*2 channel(x_channel,y_channel)=50ms
            joystick_y_level = HIGH_LEVEL;
        }
      }
      else
      {
        joystick_y_low_level_debounce_counter = 0;
        joystick_y_high_level_debounce_counter = 0;
        joystick_y_level = MID_LEVEL;
      }
    }
    //   Serial.print("joystick_x_level:");
    //     Serial.println(joystick_x_level);
    //  Serial.print("joystick_y_level:");
    //     Serial.println(joystick_y_level);
  }

  /*------joystick_direction determination-----------------------------*/
  if (joystick_x_level == HIGH_LEVEL && joystick_y_level == LOW_LEVEL)
    joystick_direction = SOUTH_WEST;
  else if (joystick_x_level == MID_LEVEL && joystick_y_level == LOW_LEVEL)
    joystick_direction = SOUTH;
  else if (joystick_x_level == LOW_LEVEL && joystick_y_level == LOW_LEVEL)
    joystick_direction = SOUTH_EAST;
  else if (joystick_x_level == LOW_LEVEL && joystick_y_level == MID_LEVEL)
    joystick_direction = EAST;
  else if (joystick_x_level == MID_LEVEL && joystick_y_level == MID_LEVEL)
    joystick_direction = MIDDLE;
  else if (joystick_x_level == HIGH_LEVEL && joystick_y_level == MID_LEVEL)
    joystick_direction = WEST;
  else if (joystick_x_level == HIGH_LEVEL && joystick_y_level == HIGH_LEVEL)
    joystick_direction = NORTH_WEST;
  else if (joystick_x_level == MID_LEVEL && joystick_y_level == HIGH_LEVEL)
    joystick_direction = NORTH;
  else if (joystick_x_level == LOW_LEVEL && joystick_y_level == HIGH_LEVEL)
    joystick_direction = NORTH_EAST;

  switch (joystick_direction)
  {
  case NORTH:

    motor1_expected_direction = CLKWISE;
    motor1_expected_speed = 255;
    motor2_expected_direction = CLKWISE;
    motor2_expected_speed = 255;

    break;
  case SOUTH:
    motor1_expected_direction = COUNTER_CLKWISE;
    motor1_expected_speed = 255;
    motor2_expected_direction = COUNTER_CLKWISE;
    motor2_expected_speed = 255;
    break;
  case WEST:
    motor1_expected_direction = CLKWISE;
    motor1_expected_speed = 255;
    motor2_expected_direction = COUNTER_CLKWISE;
    motor2_expected_speed = 255;

    break;
  case MIDDLE:

    motor1_expected_direction = CLKWISE;
    motor1_expected_speed = 0;
    motor2_expected_direction = CLKWISE;
    motor2_expected_speed = 0;

    break;
  case EAST:

    motor1_expected_direction = COUNTER_CLKWISE;
    motor1_expected_speed = 255;
    motor2_expected_direction = CLKWISE;
    motor2_expected_speed = 255;

    break;
  case NORTH_WEST:

    motor1_expected_direction = CLKWISE;
    motor1_expected_speed = 255;
    motor2_expected_direction = CLKWISE;
    motor2_expected_speed = 0;

    break;
  case NORTH_EAST:

    motor1_expected_direction = CLKWISE;
    motor1_expected_speed = 0;
    motor2_expected_direction = CLKWISE;
    motor2_expected_speed = 255;

    break;
  case SOUTH_WEST:
    motor1_expected_direction = COUNTER_CLKWISE;
    motor1_expected_speed = 255;
    motor2_expected_direction = COUNTER_CLKWISE;
    motor2_expected_speed = 0;

    break;
  case SOUTH_EAST:
    motor1_expected_direction = COUNTER_CLKWISE;
    motor1_expected_speed = 0;
    motor2_expected_direction = COUNTER_CLKWISE;
    motor2_expected_speed = 255;

    break;
  }

 /*--------------------MOTOR1--------------------*/
      if (motor1_current_direction == CLKWISE)
      {
        //Direction changing
        if (motor1_expected_direction == COUNTER_CLKWISE)
        { //Slow down
          if (motor1_current_speed > 0)
          {
            motor1_current_speed--;
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
            motor1_current_speed++;
            analogWrite(MOTOR1_INA, motor1_current_speed);
          }
          else if (motor1_current_speed > motor1_expected_speed)
          {
            motor1_current_speed--;
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
            motor1_current_speed++;
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
            motor1_current_speed++;
            analogWrite(MOTOR1_INA, motor1_current_speed);
          }
          else if (motor1_current_speed > motor1_complementary_speed)
          {
            motor1_current_speed--;
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
            motor2_current_speed--;
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
            motor2_current_speed++;
            analogWrite(MOTOR2_INA, motor2_current_speed);
          }
          else if (motor2_current_speed > motor2_expected_speed)
          {
            motor2_current_speed--;
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
            motor2_current_speed++;
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
            motor2_current_speed++;
            analogWrite(MOTOR2_INA, motor2_current_speed);
          }
          else if (motor2_current_speed > motor2_complementary_speed)
          {
            motor2_current_speed--;
            analogWrite(MOTOR2_INA, motor2_current_speed);
          }
        }
      }
/*-----------------------------------------------*/
}
