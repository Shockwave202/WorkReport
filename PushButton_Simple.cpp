
#include "PushButton_Simple.h"

void PushButton_Simple::create(int pin)
{
  //Set to input with pull-up resistor
  pinMode(pin, INPUT_PULLUP);
  this->pb.pinNbr = pin;
  this->pb.buttonState = LOW;
  this->pb.lastButtonState = LOW;
  this->pb.lastDebounceTime = 0;
  this->pb.debounceDelay = 50;
  this->pb.fallingEdge = 0;
  this->pb.risingEdge = 0;
  this->pb.reset = 1;
}

void PushButton_Simple::detection()
{
  /*------------Fcn:Pushbutton detection with debouncing timer.-------------*/
  //Description:START is an active-low pushbutton.
  // read the state of the switch into a local variable:
  int reading = digitalRead(this->pb.pinNbr);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != this->pb.lastButtonState)
  {
    // reset the debouncing timer
    this->pb.lastDebounceTime = millis();
  }

  if ((millis() - this->pb.lastDebounceTime) > this->pb.debounceDelay)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != this->pb.buttonState)
    {
      this->pb.buttonState = reading;

      if (this->pb.reset)
        this->pb.reset = 0;
      else
      {
        if (this->pb.buttonState == LOW)
          this->pb.fallingEdge = 1;
        else
          this->pb.risingEdge = 1;
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  this->pb.lastButtonState = reading;
}

bool PushButton_Simple::isPress()
{
  if (this->pb.fallingEdge)
  {
    this->pb.fallingEdge = 0;
    return true;
  }
  else
    return false;
}

bool PushButton_Simple::isRelease()
{
  if (this->pb.risingEdge)
  {
    this->pb.risingEdge = 0;
    return true;
  }
  else
    return false;
}
