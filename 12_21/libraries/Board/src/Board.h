#ifndef Board_h
#define Board_h

#include <Arduino.h> //Include common macros definitions,standard methods used
                     //in arduino.

//Pin definitions
// constants won't change. They're used here to set pin numbers:
const int RX = 0; //INPUT_PULLUP
const int TX = 1;
const int D1_BOARD = 2;
const int MOTOR4_INA = 3;//PWM
const int D4_ARDUINO = 4;
const int MOTOR1_INA = 5;//PWM
const int MOTOR2_INA = 6;//PWM
const int MOTOR3_INB = 7;
const int MOTOR4_INB = 8;
const int D9_ARDUINO = 9;//PWM
const int D2_BOARD = 10;//PWM
const int MOTOR3_INA = 11;//PWM
const int D3_BOARD = 12;//MISO
const int D4_BOARD = 13;//SCK
//const int XTAL1 = 14;
//const int XTAL2 = 15;
const int START = A0; //INPUT_PULLUP
const int BEEP = A1;
const int MOTOR1_INB = A2;
const int MOTOR2_INB = A3;
const int A1_BOARD = A4;//SDA
const int A2_BOARD = A5;//SCL
const int A3_BOARD= A6;
const int A4_BOARD= A7;

class Board
{
public:
 void initial_all_ioports()
{
  //Initial all ioports
  //PORTD
  pinMode(0, INPUT_PULLUP); //RX
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH); //TX
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  //PORTB
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  pinMode(12, OUTPUT);//MISO
  digitalWrite(12 , LOW);
  pinMode(13, OUTPUT);//SCK
  digitalWrite(13, LOW);
  //PORTC
  pinMode(A0, INPUT_PULLUP); //SW
  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, LOW);
  pinMode(A3, OUTPUT);
  digitalWrite(A3, LOW);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT_PULLUP);
  pinMode(A7, INPUT_PULLUP);

} 

};

#endif
