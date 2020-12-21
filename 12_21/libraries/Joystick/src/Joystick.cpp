#include "Joystick.h"

void Joystick::attach(int pin_x, int pin_y)
{
  //Set to input mode
  pinMode(pin_x, INPUT);
  this->axis_x.pin = pin_x;
  pinMode(pin_y, INPUT);
  this->axis_y.pin = pin_y;
  /*---------------Setup for smoothing filter------------------------------*/
  this->axis_x.readIndex = 0; // the index of the current reading
  this->axis_x.total = 0;     // the running total
  this->axis_x.average = 0;   // the average
  this->axis_x.readings_count = 0;
  //count the readings until the array is filled up,then start average calculation.
  this->axis_y.readIndex = 0; // the index of the current reading
  this->axis_y.total = 0;     // the running total
  this->axis_y.average = 0;   // the average
  this->axis_y.readings_count = 0;
  //count the readings until the array is filled up,then start average calculation.
  this->samplingRate = 0;
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    this->axis_x.readings[thisReading] = 0;
    this->axis_y.readings[thisReading] = 0;
  }
  //After power on,the input result("joystick_direction" in this example)is undetermined,thus,
  //there is a time delay between the default input result and the actual input result.
  //The output should remain still until the first reliable result is obtained.
  //(In this example,the motor should not be turn-on).
  this->axis_x.level = MID_LEVEL; //Default settings,
                                //prevent from mistriggering after power on.
  this->axis_x.low_level_debounce_counter = 0;
  this->axis_x.high_level_debounce_counter = 0;
  this->axis_y.level = MID_LEVEL;
  this->axis_y.low_level_debounce_counter = 0;
  this->axis_y.high_level_debounce_counter = 0;
}

uint8_t Joystick::getResult()
{
  //Sampling axis_x and axis_y every 1ms.
  if (millis() - this->samplingRate > 0) //1ms.
  {
    this->samplingRate = millis();
    /*----Toggle D4_BOARD-----
    if (digitalRead(D4_BOARD))
      digitalWrite(D4_BOARD, LOW);
    else
      digitalWrite(D4_BOARD, HIGH);
    /*-------------------------------*/

    /*----------axis_x-----------*/
    // subtract the previous reading at this position.
    this->axis_x.total = this->axis_x.total - this->axis_x.readings[this->axis_x.readIndex];
    // read from the sensor:
    this->axis_x.readings[this->axis_x.readIndex] = analogRead(this->axis_x.pin);
    // add the reading to the total:
    this->axis_x.total = this->axis_x.total + this->axis_x.readings[this->axis_x.readIndex];
    // advance to the next position in the array:
    this->axis_x.readIndex = this->axis_x.readIndex + 1;

    // if we're at the end of the array...
    if (this->axis_x.readIndex >= numReadings)
    {
      // ...wrap around to the beginning:
      this->axis_x.readIndex = 0;
    }

    //At least 16 readings are required to perform an
    //average calculation.
    if (this->axis_x.readings_count < numReadings)
    {
      this->axis_x.readings_count++;
      //  Serial.print("readings_count:");
      // Serial.println(readings_count);
    }
    else
    {
      // calculate the average:
      //this->axis_x.average = this->axis_x.total / numReadings;
      this->axis_x.average = this->axis_x.total >> 4;
      // send it to the computer as ASCII digits
      //  Serial.print("average:");
      //  Serial.println(average);
      //If the result needs to be sent to the PC after
      //each read,at least 14ms is required.
      if (this->axis_x.average < 350)
      {
        if (this->axis_x.level != LOW_LEVEL)
        { //start debouncing
          this->axis_x.low_level_debounce_counter++;
          if (this->axis_x.low_level_debounce_counter == 50) //50ms
            this->axis_x.level = LOW_LEVEL;
        }
      }
      else if (this->axis_x.average > 700)
      {
        if (this->axis_x.level != HIGH_LEVEL)
        {
          this->axis_x.high_level_debounce_counter++;
          if (this->axis_x.high_level_debounce_counter == 50) //50ms
            this->axis_x.level = HIGH_LEVEL;
        }
      }
      else
      {
        this->axis_x.low_level_debounce_counter = 0;
        this->axis_x.high_level_debounce_counter = 0;
        this->axis_x.level = MID_LEVEL;
      }
    }

    /*-----------axis_y------------------*/
    // subtract the last reading:
    this->axis_y.total = this->axis_y.total - this->axis_y.readings[this->axis_y.readIndex];
    // read from the sensor:
    this->axis_y.readings[this->axis_y.readIndex] = analogRead(this->axis_y.pin);
    // add the reading to the total:
    this->axis_y.total = this->axis_y.total + this->axis_y.readings[this->axis_y.readIndex];
    // advance to the next position in the array:
    this->axis_y.readIndex = this->axis_y.readIndex + 1;

    // if we're at the end of the array...
    if (this->axis_y.readIndex >= numReadings)
    {
      // ...wrap around to the beginning:
      this->axis_y.readIndex = 0;
    }

    if (this->axis_y.readings_count < numReadings)
    {
      this->axis_y.readings_count++;
      //  Serial.print("readings_count:");
      // Serial.println(readings_count);
    }
    else
    {
      // calculate the average:
      //this->axis_y.average = this->axis_y.total / numReadings;
      this->axis_y.average = this->axis_y.total >> 4;
      // send it to the computer as ASCII digits
      //  Serial.print("average:");
      //  Serial.println(average);
      //If the result needs to be sent to the PC after
      //each read,at least 14ms is required.
      if (this->axis_y.average < 350)
      {
        if (this->axis_y.level != LOW_LEVEL)
        { //start debouncing
          this->axis_y.low_level_debounce_counter++;
          if (this->axis_y.low_level_debounce_counter == 50) //50ms
            this->axis_y.level = LOW_LEVEL;
        }
      }
      else if (this->axis_y.average > 700)
      {
        if (this->axis_y.level != HIGH_LEVEL)
        {
          this->axis_y.high_level_debounce_counter++;
          if (this->axis_y.high_level_debounce_counter == 50) //50ms
            this->axis_y.level = HIGH_LEVEL;
        }
      }
      else
      {
        this->axis_y.low_level_debounce_counter = 0;
        this->axis_y.high_level_debounce_counter = 0;
        this->axis_y.level = MID_LEVEL;
      }
    }
    //   Serial.print("this->axis_x.level:");
    //     Serial.println(this->axis_x.level);
    //  Serial.print("this->axis_y.level:");
    //     Serial.println(this->axis_y.level);
  }

  /*------joystick.direction determination-----------------------------*/
  if (this->axis_x.level == HIGH_LEVEL && this->axis_y.level == LOW_LEVEL)
    this->direction = SOUTH_WEST;
  else if (this->axis_x.level == MID_LEVEL && this->axis_y.level == LOW_LEVEL)
    this->direction = SOUTH;
  else if (this->axis_x.level == LOW_LEVEL && this->axis_y.level == LOW_LEVEL)
    this->direction = SOUTH_EAST;
  else if (this->axis_x.level == LOW_LEVEL && this->axis_y.level == MID_LEVEL)
    this->direction = EAST;
  else if (this->axis_x.level == MID_LEVEL && this->axis_y.level == MID_LEVEL)
    this->direction = MIDDLE;
  else if (this->axis_x.level == HIGH_LEVEL && this->axis_y.level == MID_LEVEL)
    this->direction = WEST;
  else if (this->axis_x.level == HIGH_LEVEL && this->axis_y.level == HIGH_LEVEL)
    this->direction = NORTH_WEST;
  else if (this->axis_x.level == MID_LEVEL && this->axis_y.level == HIGH_LEVEL)
    this->direction = NORTH;
  else if (this->axis_x.level == LOW_LEVEL && this->axis_y.level == HIGH_LEVEL)
    this->direction = NORTH_EAST;
  return this->direction;
}
