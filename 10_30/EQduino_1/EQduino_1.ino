
//This program serves the purpose of ctrlling a remote-ctrlled toy car,the car is equipped with
//a infrared remote-ctrl receiver,2 DC motors with gear boxes,2 IR sensor.
//The direction can be changed by using remote ctrl transmitter,when an obstacle in front of the car
//is being detected by the IR sensor,the car will block.

#include <IRremote.h>
#include <Motor.h>
#include <IRsensor.h>
//Create the objects
//Create an IRremote object
IRremote IRremote1;
Motor Motor1, Motor2; //Create two Motor objects.
//Create 2 IRsensor objects
IRsensor IRsensor1,IRsensor2;

//Global variables definition
uint8_t cmd;
uint8_t data_code;
uint8_t custom_code;

bool carIsRun; //Indicate the car is running forward.
bool blockIsDetected;
uint8_t IRsensorDebounceCounter;

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  //Connect the remote ctrl receiver to the arduino digital pin.
  IRremote1.attach(2);
  IRremote1.enableIRIn(); //Enable infrared remote ctrl receiving fcn.
                          // put your setup code here, to run once:
  Motor1.attach(8, 5);    //Connect the digital pin to the motor driver.
  Motor2.attach(7, 6);
  IRsensor1.attach(A0); //Connect the IRsensor to the arduino analog input pin.
  IRsensor2.attach(A1); 

  //Clear the ctrl variables.
  carIsRun = 0;
  blockIsDetected = 0;
  IRsensorDebounceCounter = 0;
}

void loop()
{
  // put your main code here, to run repeatedly:

  //Block detection
  if (IRsensor1.getResult() > 50||IRsensor2.getResult() > 50)
  {
    IRsensorDebounceCounter++;
    if (IRsensorDebounceCounter == 100) //200MS
    {
      IRsensorDebounceCounter = 0;
      blockIsDetected = 1;
      if (carIsRun)
      {
        Motor1.stop(); //Block the car
        Motor2.stop();
      }
    }
  }
  else
  {
    IRsensorDebounceCounter = 0;
    blockIsDetected = 0;
  }

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);

  if (IRremote1.isReady())
  {
    //Each time receving the data code(cmd),compare with the previous cmd,
    //if the cmd changes,refresh and execute the cmd,
    //if the receving cmd is the same as the previous one,
    //just ignore the cmd so as to prevent from retriggering.
    //Check if the cmd changes
    if (cmd != IRremote1.getResult()) //The cmd will execute only when it is different from the previous one.
    {                                 //Refresh the cmd
      cmd = IRremote1.getResult();
      //Execute the cmd
      data_code = cmd & 0x1f; //Clear the highest 3 bits of the receiving data
      custom_code = cmd >> 5; //Right shift 5 bits.
      Serial.println(data_code, HEX);
      if (custom_code == 1) //Custom code identification
      {

        switch (data_code)
        {
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
          break;
        case 0x0c:              //L up
          if (!blockIsDetected) //The car can be activated only when there is
                                //no block in front of it.
          {
            carIsRun = 1;
            Motor1.rotateClkwise();
            Motor2.stop();
          }

          break;
        case 0x0d: //R up
          if (!blockIsDetected)
          {
            carIsRun = 1;
            Motor1.stop();
            Motor2.rotateClkwise();
          }

          break;
        case 0x0e: //L down
          if (!blockIsDetected)
          {
            carIsRun = 1;
            Motor1.rotateCounterClkwise();
            Motor2.stop();
          }

          break;
        case 0x0f: //R down
          if (!blockIsDetected)
          {
            carIsRun = 1;
            Motor1.stop();
            Motor2.rotateCounterClkwise();
          }

          break;
          //Combination:
        case 0x1c: //L_up +R_up
          if (!blockIsDetected)
          {
            carIsRun = 1;
            Motor1.rotateClkwise();
            Motor2.rotateClkwise();
          }

          break;
        case 0x13: //L_down +R_down
            Motor1.rotateCounterClkwise();
            Motor2.rotateCounterClkwise();

          break;
        case 0x16: //L_up +R_down
          if (!blockIsDetected)
          {
            carIsRun = 1;
            Motor1.rotateClkwise();
            Motor2.rotateCounterClkwise();
          }

          break;
        case 0x19: //L_down +R_up
          if (!blockIsDetected)
          {
            carIsRun = 1;
            Motor1.rotateCounterClkwise();
            Motor2.rotateClkwise();
          }

          break;
        case 0x1a: //L_up +L_down
                   //Invalid
          break;
        case 0x15: //END code
          carIsRun = 0;
          Motor1.stop();
          Motor2.stop();
          break;
        case 0x02: //F1
          break;
        }
      }
    }
  }
}
