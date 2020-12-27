
#include <SoftwareSerial.h>
#include <Servo.h>
#include <EEPROM.h>
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
// Create a Software serial object.
SoftwareSerial mySerial(9, 4); // RX, TX
Servo myservo;                 // create servo object to control a servo
// twelve servo objects can be created on most boards

int rfReading;
int rfDataCode;

bool servoRegulation;
unsigned long servoRegulationRate;
int servoExpectedAngle;
int servoCurrentAngle;

//#define MIN_POSITION_CEILING 1162 //the greatest value to which the minPosition can be assigned.
#define MIN_POSITION_CEILING 1056 //the greatest value to which the minPosition can be assigned.

//#define MIN_POSITION_FLOOR 544    //the least value to which the minPosition can be assigned.
#define MIN_POSITION_FLOOR 660    //the least value to which the minPosition can be assigned.

#define DEFAULT_MIN_POSITION 853 //default minmum servo position when no adjustment is applied.
#define DEFAULT_MAX_POSITION 2089
#define POSITION_RANGE 1236 //the range of the servo position.

int maxPosition;
int minPosition;
//Change servo position every fixed time interval.
unsigned long adjustPositionCount;
bool zeroPositionCorrection;
bool zeroPositionCorrection_direction;

#define INC 1
bool signal_on;
bool signal_state;
unsigned long signal_count;
int addr;

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

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world?");

  // attaches the servo on D3_BOARD to the servo object
  //Default servo position range:
  minPosition = DEFAULT_MIN_POSITION;
  maxPosition = DEFAULT_MAX_POSITION;
  //minPosition = 1500;
  //maxPosition = 2301;
  //Calling this fcn will set the threshold of the internal pulse width automatically.
  myservo.attach(D3_BOARD, minPosition, maxPosition);
  //Set the angle
  servoCurrentAngle = 180;
  myservo.write(servoCurrentAngle);
  Serial.println(myservo.readMicroseconds());
  //Serial.println(EEPROM.length());


for(int i=0;i<10;i++)
{
EEPROM.write(i, i);
delay(100);
}
 signal_on=1;
 signal_state=INC;
signal_count=millis();
addr=0;
//Serial.println(addr);
  
}

void loop()
{ // run over and over

  if (mySerial.available())
  {
    //Serial.println(mySerial.read());
    //Read the data from the RF module.
    rfReading = mySerial.read();

    //If the reading has changed,refresh the dataCode.
    if (rfReading != rfDataCode)
    {
      rfDataCode = rfReading;
      switch (rfDataCode) //Execute the command.
      {
      case 5: //KEY1
              //Forward:
        motor1.set(CLKWISE, 255);
        motor2.set(COUNTER_CLKWISE, 255);
        digitalWrite(D1_BOARD, HIGH);
        digitalWrite(D2_BOARD, HIGH);
        break;
      case 7: //KEY2
              //Backward:
        motor1.set(COUNTER_CLKWISE, 255);
        motor2.set(CLKWISE, 255);
        digitalWrite(D1_BOARD, HIGH);
        digitalWrite(D2_BOARD, HIGH);
        break;
      case 8: //KEY3
              //Turn left
        motor1.set(COUNTER_CLKWISE, 255);
        motor2.set(COUNTER_CLKWISE, 255);
        digitalWrite(D1_BOARD, HIGH);
        digitalWrite(D2_BOARD, LOW);
        break;
      case 6: //KEY4
              //Turn right
        motor1.set(CLKWISE, 255);
        motor2.set(CLKWISE, 255);
        digitalWrite(D1_BOARD, LOW);
        digitalWrite(D2_BOARD, HIGH);

        break;
      case 1: //KEY5
              //Servo clkwise rotation
        digitalWrite(D4_BOARD, HIGH);
        servoRegulation = 1;
        servoExpectedAngle = 180;
        servoRegulationRate = millis(); //Record the current time
        break;
      case 3: //KEY6
              //Servo counterClkwise rotation
        digitalWrite(D4_BOARD, HIGH);
        servoRegulation = 1;
        servoExpectedAngle = 0;
        servoRegulationRate = millis(); //Record the current time
        break;
      case 4: //KEY7
        zeroPositionCorrection = 1;
        zeroPositionCorrection_direction = COUNTER_CLKWISE ;
        adjustPositionCount = millis(); //Record the current time
        break;
      case 2: //KEY8
        zeroPositionCorrection = 1;
        zeroPositionCorrection_direction = CLKWISE;
        adjustPositionCount = millis(); //Record the current time
        break;
      case 10: //KEY_A
        break;
      case 9: //KEY_B
        break;
      case 11: //KEY_C
        break;
      case 12: //KEY_D
        break;
      case 69: //KEY1+KEY7
        break;
      case 37: //KEY1+KEY8
        break;
      case 71: //KEY2+KEY7
        break;
      case 39: //KEY2+KEY8
        break;
      case 72: //KEY3+KEY7
        break;
      case 40: //KEY3+KEY8
        break;
      case 70: //KEY4+KEY7
        break;
      case 38: //KEY4+KEY8
        break;
      case 21: //KEY1+KEY5
        break;
      case 53: //KEY1+KEY6
        break;
      case 23: //KEY2+KEY5
        break;
      case 55: //KEY2+KEY6
        break;
      case 24: //KEY3+KEY5
        break;
      case 56: //KEY3+KEY6
        break;
      case 22: //KEY4+KEY5
        break;
      case 54: //KEY4+KEY6
        break;
      case 15: //endCode.
               //Motor stop:
        motor1.set(CLKWISE, 0);
        motor2.set(CLKWISE, 0);
        digitalWrite(D1_BOARD, LOW);
        digitalWrite(D2_BOARD, LOW);
        //Servo stop
        digitalWrite(D4_BOARD, LOW);
        servoRegulation = 0;
        zeroPositionCorrection=0;
        break;
      }
    }
  }
  motor1.regulation();
  motor2.regulation();
  if (servoRegulation)
  {
    if (millis() - servoRegulationRate > 20) //Count for 20ms
    {
      servoRegulationRate = millis(); //Reset the timer
      if (servoCurrentAngle < servoExpectedAngle)
      {
        servoCurrentAngle++;
        myservo.write(servoCurrentAngle);
        Serial.println(myservo.readMicroseconds());
      }
      else if (servoCurrentAngle > servoExpectedAngle)
      {
        servoCurrentAngle--;
        myservo.write(servoCurrentAngle);
        Serial.println(myservo.readMicroseconds());
      }
    }
  }
  if (zeroPositionCorrection)
  {
    if (millis() - adjustPositionCount > 10) //Count for 10ms
    {  adjustPositionCount = millis(); //Reset the timer
      if (zeroPositionCorrection_direction == COUNTER_CLKWISE)
      {
         if (minPosition > MIN_POSITION_FLOOR)
        {
          minPosition--;
          maxPosition = minPosition + POSITION_RANGE;
          myservo.attach(D3_BOARD, minPosition, maxPosition);
          myservo.write(servoCurrentAngle);
          Serial.println(minPosition);
          Serial.println(myservo.readMicroseconds());
        }
      }
      else
      { //CLKWISE
         if (minPosition < MIN_POSITION_CEILING)
        {
          minPosition++;
          maxPosition = minPosition + POSITION_RANGE;
          myservo.attach(D3_BOARD, minPosition, maxPosition);
          //Write angle in the fcn.
          myservo.write(servoCurrentAngle);
          Serial.println(minPosition);
          Serial.println(myservo.readMicroseconds());
        }
        
      
      }
    }
  }

//    if (signal_on)
//  {
//  if(millis()-signal_count>20)//count for 20ms
//     {signal_count=millis();
//      if (signal_state) //INC
//      {
//        if (value < 9)
//        {
//          value++;
//         Serial.println(value);
//        }
//        else if (value == 9)
//        {
//          value++;
//          Serial.println(value);
//          signal_state ^= 1; //Toggle state
//        }
//      }
//    
//      else
//      { //DEC
//        if (value > 1)
//        {
//          value--;
//          Serial.println(value);
//        }
//        else if (value == 1)
//        {
//          value--;
//          Serial.println(value);
//          signal_state ^= 1; //Toggle state
//        }
//      }
//    }
//  }

  if (signal_on)
  {
  if(millis()-signal_count>200)//count for 20ms
     {signal_count=millis();
    Serial.println(EEPROM.read(addr));
  addr++;
  if(addr==10)
    addr=0;
    }
  }

  
  //  if (Serial.available()) {
  //    mySerial.write(Serial.read());
  //  }
}
