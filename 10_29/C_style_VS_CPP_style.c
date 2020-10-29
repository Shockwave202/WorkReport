//5 pushbuttons are attached to the IO pins,
//The timer will check these pins periodically so as to
//inform the input signal to the ctrller.

//C style

//A full set of ctrl variables is required for each port.
sbit sw1_selflock;
sbit sw1_falling_edge;
uint8_t sw1_debounce_counter;
sbit sw2_selflock;
sbit sw2_falling_edge;
uint8_t sw2_debounce_counter;
sbit sw3_selflock;
sbit sw3_falling_edge;
uint8_t sw3_debounce_counter;
sbit sw4_selflock;
sbit sw4_falling_edge;
uint8_t sw4_debounce_counter;
sbit sw5_selflock;
sbit sw5_falling_edge;
uint8_t sw5_debounce_counter;

/*--------SW1 detection----------------------*/
if (SW1)
{
    sw1_selflock = 0； 
    sw1_debounce_counter=0;
}
else
{
    if (!sw1_selflock)
    {
        sw1_debounce_counter++;
        if (sw1_debounce_counter == 50)
        {
            sw1_selflock = 1;
            sw1_falling_edge = 1;
        }
    }
}

/*--------SW2 detection----------------------*/
if (SW2)
{
    sw2_selflock = 0； 
    sw2_debounce_counter=0;
}
else
{
    if (!sw2_selflock)
    {
        sw2_debounce_counter++;
        if (sw2_debounce_counter == 50)
        {
            sw2_selflock = 1;
            sw2_falling_edge = 1;
        }
    }
}

/*--------SW3 detection----------------------*/
if (SW3)
{
    sw3_selflock = 0； 
    sw3_debounce_counter=0;
}
else
{
    if (!sw3_selflock)
    {
        sw3_debounce_counter++;
        if (sw3_debounce_counter == 50)
        {
            sw3_selflock = 1;
            sw3_falling_edge = 1;
        }
    }
}

/*--------SW4 detection----------------------*/
if (SW4)
{
    sw4_selflock = 0； 
    sw4_debounce_counter=0;
}
else
{
    if (!sw4_selflock)
    {
        sw4_debounce_counter++;
        if (sw4_debounce_counter == 50)
        {
            sw4_selflock = 1;
            sw4_falling_edge = 1;
        }
    }
}

/*--------SW5 detection----------------------*/
if (SW5)
{
    sw5_selflock = 0； 
    sw5_debounce_counter=0;
}
else
{
    if (!sw5_selflock)
    {
        sw5_debounce_counter++;
        if (sw5_debounce_counter == 50)
        {
            sw5_selflock = 1;
            sw5_falling_edge = 1;
        }
    }
}
