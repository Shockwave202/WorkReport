

Pushbutton SW1,SW2,SW3,SW4,SW5;

void setup() {
  // put your setup code here, to run once:
SW1.attach(2);
SW2.attach(3);
SW3.attach(4);
SW4.attach(5);
SW5.attach(6);
}

void loop() {
  // put your main code here, to run repeatedly:
if(SW1.isFallingEdge())
  {
  }
if(SW2.isFallingEdge())
  {
  }
if(SW3.isFallingEdge())
  {
  }
  if(SW4.isFallingEdge())
  {
  }
  if(SW5.isFallingEdge())
  {
  }


}
 void fallingEdgeDetection();//For timer 
ISR?
{SW1.fallingEdgeDetection();
SW2.fallingEdgeDetection();
SW3.fallingEdgeDetection();
SW4.fallingEdgeDetection();
SW5.fallingEdgeDetection();


}


Pushbutton::Pushbutton()
{
this->selfLock=0;
this->fallingEdge=0;
this->debounceCounter=0;
}

void Pushbutton::attach(int pin)
{pinMode(pin,INPUT);
 this->pinNbr=pin;
}

void Pushbutton::fallingEdgeDetection()
{
if (digitalRead(this->pinNbr))
{
    this->selfLock = 0； 
    this->debounceCounter=0;
}
else
{
    if (!this->selfLock)
    {
        this->debounceCounter++;
        if (this->debounceCounter == 50)
        {
            this->selfLock = 1;
            this->fallingEdge = 1;
        }
    }
}
}

bool isFallingEdge()
   {if(this->fallingEdge)
      {this->fallingEdge=0;
      return true;
      }  
    else return false;
   }









































