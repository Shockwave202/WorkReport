
#include "PushButton.h"                //Macros definition,internel user-defined datatypes definition,
                                       //class declaration.
struct pb_params_t pb_params[4];       //Declaraction of the structure.
static volatile int8_t pb_channel = 0; //For int
uint8_t pb_count = 0;

static boolean isTimer2Active()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    if (pb_params[i].isactive)
      return true;
  }
  return false;
}

void PushButton::create(int pin)
{
  //Allocate the index to the pb.
  this->pb_index = pb_count++;
  //Set to input mode
  pinMode(pin, INPUT_PULLUP);

  pb_params[this->pb_index].pinNbr = pin;
  pb_params[this->pb_index].rising_edge = 0;
  pb_params[this->pb_index].rising_edge_selflock = 0;
  pb_params[this->pb_index].falling_edge = 0;
  pb_params[this->pb_index].falling_edge_selflock = 0;
  pb_params[this->pb_index].rising_edge_debounce_counter = 0;
  pb_params[this->pb_index].falling_edge_debounce_counter = 0;
  pb_params[this->pb_index].reset_pushbutton_detection = 1;
  pb_params[this->pb_index].double_click_check = 0;
  pb_params[this->pb_index].double_click_effective_time_counter = 0;
  pb_params[this->pb_index].falling_edge_count = 0;
  pb_params[this->pb_index].double_click = 0;
  pb_params[this->pb_index].single_click = 0;
  pb_params[this->pb_index].isactive = 0;
  //Enable common hardware
  if (isTimer2Active() == false)
    enableTimer2();
  pb_params[this->pb_index].isactive = true;
  // this must be set after the check for isTimer2Active
}

void PushButton::enableTimer2(void)
//Enable timer2 and set the time interval at 1MS.
{
  // the interrupt Service Routine fires every 1MS

  //noInterrupts();
  cli();

  // Setup pulse clock timer interrupt
  // Prescale /8 (16M/8 = 0.5 microseconds per tick)
  // Therefore, the timer interval can range from 0.5 to 128 microseconds
  // Depending on the reset value (255 to 0)

  //timerConfigForReceive();
  TCCR2A = _BV(WGM21);
  TCCR2B = _BV(CS22); //clk T2S /64 (from prescaler)
  //OCR2A = TIMER_COUNT_TOP / 8;
  OCR2A = 250;
  TCNT2 = 0;

  // Timer2 Overflow Interrupt Enable
  TIMER_ENABLE_RECEIVE_INTR;

  //Clear interrupt flag
  //TIMER_RESET_INTR_PENDING;

  //interrupts();
  sei();
}

ISR(TIMER2_COMPA_vect)
{
  // reset timer interrupt flag if required

  /*----Toggle LED_TEST-----
  if (digitalRead(13))//D1_BOARD
    digitalWrite(13, LOW);
  else
    digitalWrite(13, HIGH);
  /*--------------------*/

  /*----------Pushbutton detection-------------*/
  /*--------SW---------*/
  pb_channel++; // increment to the next channel
  if (pb_channel == pb_count)
    pb_channel = 0;

  /*----test one channel-----
  if (pb_channel == 0)
  {                      
    if (digitalRead(A4)) //D1_BOARD
      digitalWrite(A4, LOW);
    else
      digitalWrite(A4, HIGH);
       }
  /*--------------------*/

  if (digitalRead(pb_params[pb_channel].pinNbr)) //High level
  {
    pb_params[pb_channel].falling_edge_selflock = 0;
    pb_params[pb_channel].falling_edge_debounce_counter = 0;
    if (!pb_params[pb_channel].rising_edge_selflock)
    {
      pb_params[pb_channel].rising_edge_debounce_counter++;
      if (pb_params[pb_channel].rising_edge_debounce_counter == 60 / pb_count) //pushbutton has been released.
      {
        pb_params[pb_channel].rising_edge_selflock = 1;
        if (pb_params[pb_channel].reset_pushbutton_detection)
          //uses up this bit after power on
          pb_params[pb_channel].reset_pushbutton_detection = 0;
        else
          pb_params[pb_channel].rising_edge = 1;
      }
    }
    if (pb_params[pb_channel].trigger_flag)
    {
      pb_params[pb_channel].trigger_flag = 0;
      pb_params[pb_channel].short_press = 1;
    }
  }
  else
  {
    pb_params[pb_channel].rising_edge_selflock = 0;
    pb_params[pb_channel].rising_edge_debounce_counter = 0;
    if (pb_params[pb_channel].falling_edge_selflock == 0)
    {
      pb_params[pb_channel].falling_edge_debounce_counter++;
      if (pb_params[pb_channel].falling_edge_debounce_counter == 60 / pb_count)
      {
        if (pb_params[pb_channel].reset_pushbutton_detection)
          //uses up this bit after reset
          pb_params[pb_channel].reset_pushbutton_detection = 0;
        else
        {
          //All of them are indicators,can be used to
          //transfer information under different circumstances.
          pb_params[pb_channel].trigger_flag = 1;
          pb_params[pb_channel].falling_edge = 1;
          pb_params[pb_channel].falling_edge_count++;
          if (pb_params[pb_channel].falling_edge_count == 3)
            pb_params[pb_channel].falling_edge_count = 1;
          if (!pb_params[pb_channel].double_click_check)
          {
            //Activate double_click_check:
            pb_params[pb_channel].double_click_check = 1;
            pb_params[pb_channel].double_click_effective_time_counter = 0;
            //digitalWrite(13, HIGH);
          }
        }
      }

      else if (pb_params[pb_channel].falling_edge_debounce_counter == 1000 / pb_count)
      {
        pb_params[pb_channel].falling_edge_selflock = 1;
        if (pb_params[pb_channel].trigger_flag)
        {
          pb_params[pb_channel].trigger_flag = 0;
          pb_params[pb_channel].long_press = 1;
        }
      }
    }
  } //end of if(!SW)

  if (pb_params[pb_channel].double_click_check)
  {
    pb_params[pb_channel].double_click_effective_time_counter++;
    if (pb_params[pb_channel].double_click_effective_time_counter == 600 / pb_count)
    {
      pb_params[pb_channel].double_click_check = 0;
      //Clear previous memory,time expired,all efforts
      //becomes in vain.
      if (pb_params[pb_channel].falling_edge_count == 1)
        pb_params[pb_channel].single_click = 1;
      pb_params[pb_channel].falling_edge_count = 0;
      //digitalWrite(13, LOW);
    }
    else
    { // pb_params.double_click_effective_time_counter < 600 / pb_count
      if (pb_params[pb_channel].falling_edge_count == 2)
      {
        pb_params[pb_channel].double_click_check = 0; //No reason to exist.
        pb_params[pb_channel].double_click = 1;
        //digitalWrite(13, LOW);
      }
    }
  }
}

bool PushButton::isPress()
{
  if (pb_params[this->pb_index].falling_edge)
  {
    pb_params[this->pb_index].falling_edge = 0;
    return true;
  }
  else
    return false;
}

bool PushButton::isRelease()
{
  if (pb_params[this->pb_index].rising_edge)
  {
    pb_params[this->pb_index].rising_edge = 0;
    return true;
  }
  else
    return false;
}

bool PushButton::isShortPress()
{
  if (pb_params[this->pb_index].short_press)
  {
    pb_params[this->pb_index].short_press = 0;
    return true;
  }
  else
    return false;
}

bool PushButton::isLongPress()
{
  if (pb_params[this->pb_index].long_press)
  {
    pb_params[this->pb_index].long_press = 0;
    return true;
  }
  else
    return false;
}

bool PushButton::isSingleClick()
{
  if (pb_params[this->pb_index].single_click)
  {
    pb_params[this->pb_index].single_click = 0;
    return true;
  }
  else
    return false;
}

bool PushButton::isDoubleClick()
{
  if (pb_params[this->pb_index].double_click)
  {
    pb_params[this->pb_index].double_click = 0;
    return true;
  }
  else
    return false;
}
