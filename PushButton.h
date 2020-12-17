#ifndef PushButton_h
#define PushButton_h

#include <Arduino.h> //Include common macros definitions,standard methods used
                     //in arduino.
//Using Timer2 for pushbutton state detection,4 active-high
//pushbuttons can be attached on this timer.

#define TIMER_ENABLE_RECEIVE_INTR (TIMSK2 = _BV(OCIE2A))
// Output Compare Match A Interrupt Enable

struct pb_params_t //Declaraction of the structure.
{
    uint8_t pinNbr;
    bool rising_edge;
    bool rising_edge_selflock;
    bool falling_edge;
    bool falling_edge_selflock;
    bool trigger_flag;
    bool short_press;
    bool long_press;
    uint16_t rising_edge_debounce_counter;
    uint16_t falling_edge_debounce_counter;
    bool reset_pushbutton_detection;
    bool double_click_check;
    uint16_t double_click_effective_time_counter;
    uint8_t falling_edge_count;
    bool double_click;
    bool single_click;
    bool isactive;//Check if the channel is active.
}; 



class PushButton
{
public:
    PushButton(){}
    void create(int pin);
    bool isPress();
    bool isRelease();
    bool isShortPress();
    bool isLongPress();
    bool isDoubleClick();
    bool isSingleClick();
private:   
    void enableTimer2(void);
    uint8_t pb_index;
};

#endif
