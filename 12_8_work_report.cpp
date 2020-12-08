//Self-tracking vehicle
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
Pushbutton Pushbutton1, Pushbutton2;
METALROBO_Motor MRMotor;
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

bool program_selection_procedure;
int pgm_index;
bool pgm_switching;

bool aircraft;
//Frog initial
uint8_t frog_mode;
bool frog_is_jumping;

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

//music start:
void music()
{
  ontone.tone(Tone_data[5][0], 200);
  ontone.tone(Tone_data[5][1], 200);
  ontone.tone(Tone_data[5][0], 200);
  ontone.tone(Tone_data[5][1], 200);
  ontone.tone(Tone_data[5][0], 200);
  ontone.tone(Tone_data[5][1], 200);
}

void setup()
{ //The startup time is approximate to 1.6S.
  // put your setup code here, to run once:
  LABBOT_Board.initial_all_ioports();

  /*--Motor_ORG-----*/
  MRMotor.MRMtr_pri_init();
  /*-----Tone_ORG--------*/
  ontone.port(MIC);
  Serial.begin(9600);

//INPUT:
  Pushbutton1.create(START);
  //Pushbutton2.create(D3_BOARD);
  pinMode(A1_BOARD, INPUT); //IR sensor
  pinMode(A2_BOARD, INPUT);
//OUTPUT:
//Indicators:
  pinMode(D1_BOARD, OUTPUT); //Attach LED1 to D1 pin
  digitalWrite(D1_BOARD, LOW);
  pinMode(D2_BOARD, OUTPUT); //Attach LED2 to D2 pin
  digitalWrite(D2_BOARD, LOW);
  self_tracing_vehicle = 1;
  count_1MS = 0;
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
  //Serial.println(analogRead(A1_BOARD));
  //_delay_ms(20);

  /*--START the program--*/
  if (Pushbutton1.isPress())
  {
/*---------------Motor1 CMD------------------------------*/    
    if (motor1_expected_direction) //CLKWISE
    {
      motor1_expected_direction = COUNTER_CLKWISE;
      motor1_expected_speed = 150;
    }
    else
    {
      //Counter clkwise rotation
      motor1_expected_direction = CLKWISE;
      motor1_expected_speed = 150;
    }


 /*------------------MOTOR2 CMD-------------------------------*/   
    if (motor2_expected_direction) //CLKWISE
    {
      motor2_expected_direction = COUNTER_CLKWISE;
      motor2_expected_speed = 150;
    }
    else
    {
      //Counter clkwise rotation
      motor2_expected_direction = CLKWISE;
      motor2_expected_speed = 150;
    }
 /*---------------------------------------------*/ 
  }

  if (self_tracing_vehicle)
  { 
 /*----------SELF_TRACKING_LOGIC-----------------------------*/   
    if (analogRead(A1_BOARD) > 900 && analogRead(A2_BOARD) > 900)
    { //Both left IR sensor and right IR sensor detect the white floor,go straight forward;
      //AD voltage high:4.96V;
      //VDD=4.96V;
      // ( 4.76 / 4.96 ) * 1023 = 0.96 * 1023 = 982

      //AD voltage low:2.12V
      // ( 2.12 / 4.96 ) * 1023 = 0.43 * 1023 = 440
      motor1_expected_direction = CLKWISE;
      motor1_expected_speed = 255;
	    motor2_expected_direction = CLKWISE;
      motor2_expected_speed = 255;
      digitalWrite(D1_BOARD, HIGH);
      digitalWrite(D2_BOARD, HIGH);
    }
     else if (analogRead(A1_BOARD) > 900 && analogRead(A2_BOARD) < 500)
    { //Right IR sensor has detected the black stripe,left IR sensor has detected the white floor,
      //turn right under this condition.
	  motor1_expected_direction = CLKWISE ;
      motor1_expected_speed = 255;
	  motor2_expected_direction = COUNTER_CLKWISE;
      motor2_expected_speed = 255;
      digitalWrite(D1_BOARD, HIGH);
      digitalWrite(D2_BOARD, LOW );
    }
    else if (analogRead(A2_BOARD) > 900  && analogRead(A1_BOARD) < 500)
    {
      //Left IR sensor has detected the black stripe,right IR sensor has detected the white floor,
      //turn left under this condition.
      motor1_expected_direction = COUNTER_CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = CLKWISE;
      motor2_expected_speed = 255;
      digitalWrite(D2_BOARD, HIGH);
      digitalWrite(D1_BOARD, LOW);
    }
   
else if (analogRead(A1_BOARD)<500 && analogRead(A2_BOARD)<500)
{//Right IR sensor has detected the black stripe,left IR sensor has detected the black stripe,
//stop under this condition.
      motor1_expected_direction = CLKWISE;
      motor1_expected_speed = 0;
	  motor2_expected_direction = CLKWISE;
      motor2_expected_speed = 0;
      digitalWrite(D1_BOARD,LOW);
      digitalWrite(D2_BOARD,LOW);
}   
  
 /*------------------------------------------*/   
  /*----Toggle D4_BOARD-----*/
    count_1MS++;
    if (count_1MS == 3)
    {
      count_1MS = 0;
      /*---------------------
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
            motor1_current_speed-=5;
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
            motor1_current_speed+=5;
            analogWrite(MOTOR1_INA, motor1_current_speed);
          }
          else if (motor1_current_speed > motor1_expected_speed)
          {
            motor1_current_speed-=5;
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
            motor1_current_speed+=5;
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
            motor1_current_speed+=5;
            analogWrite(MOTOR1_INA, motor1_current_speed);
          }
          else if (motor1_current_speed > motor1_complementary_speed)
          {
            motor1_current_speed-=5;
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
            motor2_current_speed-=5;
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
            motor2_current_speed+=5;
            analogWrite(MOTOR2_INA, motor2_current_speed);
          }
          else if (motor2_current_speed > motor2_expected_speed)
          {
            motor2_current_speed-=5;
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
            motor2_current_speed+=5;
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
            motor2_current_speed+=5;
            analogWrite(MOTOR2_INA, motor2_current_speed);
          }
          else if (motor2_current_speed > motor2_complementary_speed)
          {
            motor2_current_speed-=5;
            analogWrite(MOTOR2_INA, motor2_current_speed);
          }
        }
      }
/*-----------------------------------------------*/

    }
  }
}
