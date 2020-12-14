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
Pushbutton Pushbutton1;

#define ACTIVE 1
#define IDLE 0
bool state;

//Input:

//Output:
//Motor1 Ctrl
#define CLKWISE 1
#define COUNTER_CLKWISE 0
bool motor1_current_direction;
bool motor1_expected_direction;
uint8_t motor1_current_speed;
uint8_t motor1_expected_speed;
uint8_t motor1_complementary_speed;

//Proportional ctrl
bool proportional_ctrl;
uint16_t regulation_rate;
const float kp = 5;
const float ki = 0.002;
const float kd = 10.0;
float expected_value;
float actual_value;
float deviation;
float pwm_duty;
float previous_deviation;
float sum_deviation;

void setup()
{ //The startup time is approximate to 1.6S.
  // put your setup code here, to run once:
  LABBOT_Board.initial_all_ioports();
  Serial.begin(9600);
  Pushbutton1.create(START);
  pinMode(A1_BOARD, INPUT); //Potentiometer for expected value
  pinMode(A2_BOARD, INPUT); //Potentiometer for actual value
  //Actuators:
  motor1_current_direction = CLKWISE;
  motor1_expected_direction = CLKWISE;
  motor1_current_speed = 0;
  motor1_expected_speed = 0;
  motor1_complementary_speed = 0;
  digitalWrite(MOTOR1_INB, LOW);
  analogWrite(MOTOR1_INA, motor1_current_speed);

  state = IDLE;
  //Proportional ctrl
  proportional_ctrl = 1;

  state = ACTIVE; //Initial state
  proportional_ctrl = 1;
  //proportional_ctrl = 0;
  regulation_rate = 0;
  expected_value = 250.0;
  actual_value = 0.0;
  deviation = 0.0;
  previous_deviation = 0.0;
  sum_deviation=0.0;
  pwm_duty = 0.0;
}

void loop()
{
  // put your main code here, to run repeatedly:

  /*--START the program--*/
  if (Pushbutton1.isPress())
  {
    if (state) //state=ACTIVE
    {
      state = IDLE;

      //expected_value = 250.0;
      //analogWrite(MOTOR1_INA, 255);
    }
    else //state=IDLE
    {
      state = ACTIVE; //Initial state

      //expected_value = 100.0;
      //analogWrite(MOTOR1_INA, 0);
    }
  }

  /*----------Proportional ctrl--------------------*/
  if (proportional_ctrl)
  {
    regulation_rate++;
    if (regulation_rate == 1000) //2MS(1000)
    {
      regulation_rate = 0;

      /*----Toggle D4_BOARD-----*/
      if (digitalRead(D4_BOARD))
        digitalWrite(D4_BOARD, LOW);
      else
        digitalWrite(D4_BOARD, HIGH);
      /*-------------------------------*/

      expected_value = analogRead(A1_BOARD);
      map(expected_value, 0, 1023, 0, 255);
      actual_value = analogRead(A2_BOARD);
      map(actual_value, 0, 1023, 0, 255);
      if (expected_value > actual_value)
      {
        deviation = expected_value - actual_value;
        //Speed regulation
      pwm_duty = kp * deviation + kd * (deviation - previous_deviation);
      //pwm_duty = kp * deviation;
       sum_deviation+=deviation;
      //pwm_duty = kp * deviation + ki * sum_deviation;
        if (pwm_duty > 255)
          pwm_duty = 255;
        //Direction changing
        digitalWrite(MOTOR1_INB, LOW);
      }
      else
      {
        deviation = actual_value - expected_value;
        //Speed regulation
        pwm_duty = 255 - kp * deviation - kd * (deviation - previous_deviation);
        //pwm_duty = 255 - kp * deviation;

        sum_deviation-=deviation;
        //pwm_duty = kp * deviation + ki * sum_deviation;
        if (pwm_duty < 0.1)
          pwm_duty = 0;
        //The complementary value is applied during reverse direction.
        //Direction changing
        digitalWrite(MOTOR1_INB, HIGH);
      }
      analogWrite(MOTOR1_INA, pwm_duty);
      previous_deviation = deviation;
      //Serial.print("deviation:");
      //Serial.println(deviation);
      //Serial.print("previous_deviation:");
      //Serial.println(previous_deviation);
      //Serial.print("sum_deviation:");
      //Serial.println(sum_deviation);
      /*----------------------------------*/
    }
  }
}
/*-----------Positional PID pseudocode---------------
previous_error := 0  //上一次偏差
integral := 0   //积分和

//循环 
//采样周期为dt
loop:
 //setpoint 设定值
 //measured_value 反馈值
    error := setpoint − measured_value //计算得到偏差
    integral := integral + error × dt //计算得到积分累加和
    derivative := (error − previous_error) / dt //计算得到微分
    output := Kp × error + Ki × integral + Kd × derivative //计算得到PID输出
    previous_error := error //保存当前偏差为下一次采样时所需要的历史偏差
    wait(dt) //等待下一次采用
    goto loop
/*------------------------

typedef struct
{
  float Kp;                       //比例系数Proportional
  float Ki;                       //积分系数Integral
  float Kd;                       //微分系数Derivative

  float Ek;                       //当前误差
  float Ek1;                      //前一次误差 e(k-1)
  float Ek2;                      //再前一次误差 e(k-2)
  float LocSum;                   //累计积分位置
}PID_LocTypeDef;

/************************************************
函数名称 ： PID_Loc
功    能 ： PID位置(Location)计算
参    数 ： SetValue ------ 设置值(期望值)
            ActualValue --- 实际值(反馈值)
            PID ----------- PID数据结构
返 回 值 ： PIDLoc -------- PID位置
作    者 ： strongerHuang
*************************************************
float PID_Loc(float SetValue, float ActualValue, PID_LocTypeDef *PID)
{
  float PIDLoc;                                  //位置

  PID->Ek = SetValue - ActualValue;
  PID->LocSum += PID->Ek;                         //累计误差

  PIDLoc = PID->Kp * PID->Ek + (PID->Ki * PID->LocSum) + PID->Kd * (PID->Ek1 - PID->Ek);

  PID->Ek1 = PID->Ek;  return PIDLoc;
}
/*--------------------------*/
