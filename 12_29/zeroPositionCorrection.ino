
#include <SoftwareSerial.h>
#include <Servo.h>
#include <EEPROM.h>

#include "Board.h"
#include "PushButton_Simple.h"
#include "PushButton.h"       //Use timer2
#include "METALROBO_Module.h" //For the buzzer.
#include "Motor_Regulator.h"

//Create objs.
Board LABBOT_Board;
//Input:
PushButton_Simple startButton;

//Output:
buzzer ontone;
Motor_Regulator motor1, motor2;
// Create a Software serial object.
SoftwareSerial mySerial(9, 4); // RX, TX
Servo servo1;                  // create servo object to control a servo
Servo servo2, servo3, servo4, servo5, servo6;
// twelve servo objects can be created on most boards

int rfReading;
int rfDataCode;

//#define MIN_POSITION_CEILING 1162 //the greatest value to which the minPosition can be assigned.
#define MIN_POSITION_CEILING 1056 //the greatest value to which the minPosition can be assigned.

//#define MIN_POSITION_FLOOR 544    //the least value to which the minPosition can be assigned.
#define MIN_POSITION_FLOOR 550 //the least value to which the minPosition can be assigned.

#define DEFAULT_MIN_POSITION 800 //default minmum servo position when no adjustment is applied.
#define DEFAULT_MAX_POSITION 2140
#define POSITION_RANGE 1340 //the range of the servo position.

struct servoAddIn_t //This structure contains additional information
                    //used for ctrlling multiple servos.Each entry of the array
                    //should append to a servo object.
{
    unsigned long regulationRate;
    int minPosition;
    int maxPosition;
    int currentAngle;
    int expectedAngle;
    bool zeroPositionCorrection;
    unsigned long correctionRate;
    bool zeroPositionCorrection_direction;
    int pinNbr;

} servoAddIn[6];

int servoIndex;

int eeAddress, eeAddress1, eeAddress2;
byte checkingByte1, checkingByte2;

#define SELECTION_MODE 0
#define SETTING_MODE 1
#define RUNNING_MODE 2

int mode; // The program behaviour differently in different mode
bool mode_switching;

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
    pinMode(A1_BOARD, OUTPUT); //Attach LED1 to D1 pin
    digitalWrite(A1_BOARD, LOW);
    pinMode(A2_BOARD, OUTPUT); //Attach LED2 to D2 pin
    digitalWrite(A2_BOARD, LOW);
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

    // set the data rate for the SoftwareSerial port
    mySerial.begin(9600);

    //The first 2 bytes are used to check whether the EEPROM has been modified yet,
    //If the checking bytes don't match with the given values,which means the EEPROM hasn't
    //been modified,the servo associated variables should be assigned to the default values.
    //otherwise,read in the values from EEPROM.
    EEPROM.get(0, checkingByte1);
    EEPROM.get(1, checkingByte2);

    if (checkingByte1 != 0x55 || checkingByte2 != 0xaa)
    {

        //Assign the default value.
        for (int i = 0; i < 6; i++)
        {
            servoAddIn[i].minPosition = DEFAULT_MIN_POSITION;
            servoAddIn[i].maxPosition = DEFAULT_MAX_POSITION;
        }

        //Update EEPROM
        EEPROM.put(0, 0x55);
        EEPROM.put(1, 0xaa);

        eeAddress1 = 2;
        eeAddress2 = 4;
        for (int i = 0; i < 6; i++)
        {
            EEPROM.put(eeAddress1, servoAddIn[i].minPosition);
            EEPROM.put(eeAddress2, servoAddIn[i].maxPosition);
            eeAddress1 += 2 * sizeof(int); //Move to the next group of values.
            eeAddress2 += 2 * sizeof(int); //Move to the next group of values.
        }
    }
    else
    { //checkingByte1=0x55 && checkingByte2==0xaa
        //The EEPROM has already been written,read in the values from the EEPROM
        eeAddress1 = 2;
        eeAddress2 = 4;
        for (int i = 0; i < 6; i++)
        {
            Serial.println(EEPROM.get(eeAddress1, servoAddIn[i].minPosition));
            Serial.println(EEPROM.get(eeAddress2, servoAddIn[i].maxPosition));
            eeAddress1 += 2 * sizeof(int); //Move to the next group of values.
            eeAddress2 += 2 * sizeof(int); //Move to the next group of values.
        }
    }

    //Set all 6  servos to 90 degrees.
    for (int i = 0; i < 6; i++)
    {
        servoAddIn[i].expectedAngle = 90;
        servoAddIn[i].regulationRate = millis(); //Record the current time
    }
    servoAddIn[0].pinNbr = D1_BOARD;
    servoAddIn[1].pinNbr = D2_BOARD;
    servoAddIn[2].pinNbr = D3_BOARD;
    servoAddIn[3].pinNbr = D4_BOARD;
    servoAddIn[4].pinNbr = A1_BOARD;
    servoAddIn[5].pinNbr = A2_BOARD;

    //Calling this fcn will set the threshold of the internal pulse width automatically.
    servo1.attach(servoAddIn[0].pinNbr, servoAddIn[0].minPosition, servoAddIn[0].maxPosition);
    servo2.attach(servoAddIn[1].pinNbr, servoAddIn[1].minPosition, servoAddIn[1].maxPosition);
    servo3.attach(servoAddIn[2].pinNbr, servoAddIn[2].minPosition, servoAddIn[2].maxPosition);
    servo4.attach(servoAddIn[3].pinNbr, servoAddIn[3].minPosition, servoAddIn[3].maxPosition);
    servo5.attach(servoAddIn[4].pinNbr, servoAddIn[4].minPosition, servoAddIn[4].maxPosition);
    servo6.attach(servoAddIn[5].pinNbr, servoAddIn[5].minPosition, servoAddIn[5].maxPosition);

    mode = RUNNING_MODE;
    mode_switching = 1;
}
void loop()
{ // run over and over

    startButton.detection(); //Check if the button is pressed.

    /*------------------servo1 speed regulation--------------------------*/
    if (millis() - servoAddIn[0].regulationRate > 20) //Count for 20ms
    {
        servoAddIn[0].regulationRate = millis(); //Reset the timer

        if (servoAddIn[0].currentAngle < servoAddIn[0].expectedAngle)
        {
            servoAddIn[0].currentAngle++;
            servo1.write(servoAddIn[0].currentAngle);
        }
        else if (servoAddIn[0].currentAngle > servoAddIn[0].expectedAngle)
        {
            servoAddIn[0].currentAngle--;
            servo1.write(servoAddIn[0].currentAngle);
        }
    }

    /*------------------servo2 speed regulation--------------------------*/

    if (millis() - servoAddIn[1].regulationRate > 20) //Count for 20ms
    {
        servoAddIn[1].regulationRate = millis(); //Reset the timer

        if (servoAddIn[1].currentAngle < servoAddIn[1].expectedAngle)
        {
            servoAddIn[1].currentAngle++;
            servo2.write(servoAddIn[1].currentAngle);
        }
        else if (servoAddIn[1].currentAngle > servoAddIn[1].expectedAngle)
        {
            servoAddIn[1].currentAngle--;
            servo2.write(servoAddIn[1].currentAngle);
        }
    }

    /*------------------servo3 speed regulation--------------------------*/

    if (millis() - servoAddIn[2].regulationRate > 20) //Count for 20ms
    {
        servoAddIn[2].regulationRate = millis(); //Reset the timer

        if (servoAddIn[2].currentAngle < servoAddIn[2].expectedAngle)
        {
            servoAddIn[2].currentAngle++;
            servo3.write(servoAddIn[2].currentAngle);
        }
        else if (servoAddIn[2].currentAngle > servoAddIn[2].expectedAngle)
        {
            servoAddIn[2].currentAngle--;
            servo3.write(servoAddIn[2].currentAngle);
        }
    }

    /*------------------servo4 speed regulation--------------------------*/

    if (millis() - servoAddIn[3].regulationRate > 20) //Count for 20ms
    {
        servoAddIn[3].regulationRate = millis(); //Reset the timer

        if (servoAddIn[3].currentAngle < servoAddIn[3].expectedAngle)
        {
            servoAddIn[3].currentAngle++;
            servo4.write(servoAddIn[3].currentAngle);
        }
        else if (servoAddIn[3].currentAngle > servoAddIn[3].expectedAngle)
        {
            servoAddIn[3].currentAngle--;
            servo4.write(servoAddIn[3].currentAngle);
        }
    }

    /*------------------servo5 speed regulation--------------------------*/
    if (millis() - servoAddIn[4].regulationRate > 20) //Count for 20ms
    {
        servoAddIn[4].regulationRate = millis(); //Reset the timer

        if (servoAddIn[4].currentAngle < servoAddIn[4].expectedAngle)
        {
            servoAddIn[4].currentAngle++;
            servo5.write(servoAddIn[4].currentAngle);
        }
        else if (servoAddIn[4].currentAngle > servoAddIn[4].expectedAngle)
        {
            servoAddIn[4].currentAngle--;
            servo5.write(servoAddIn[4].currentAngle);
        }
    }

    /*------------------servo6 speed regulation--------------------------*/
    if (millis() - servoAddIn[5].regulationRate > 20) //Count for 20ms
    {
        servoAddIn[5].regulationRate = millis(); //Reset the timer

        if (servoAddIn[5].currentAngle < servoAddIn[5].expectedAngle)
        {
            servoAddIn[5].currentAngle++;
            servo6.write(servoAddIn[5].currentAngle);
        }
        else if (servoAddIn[5].currentAngle > servoAddIn[5].expectedAngle)
        {
            servoAddIn[5].currentAngle--;
            servo6.write(servoAddIn[5].currentAngle);
        }
    }

    /*----------------------RUNNING_MODE---------------------------*/
    if (mode == RUNNING_MODE)
    {
        //Entrance:
        if (mode_switching)
        {
            mode_switching = 0;
        }

        //Exit:
        if (startButton.isPress())
        {
            mode = SETTING_MODE;
            mode_switching = 1;
        }
    }

    /*----------------------SETTING_MODE-----------------------*/
    else if (mode == SETTING_MODE)
    {
        //Entrance:
        if (mode_switching)
        {
            mode_switching = 0;

            //Specify the default servo to be adjusted.
            servoIndex = 0;
            //Set servo1 to 90 degrees.
            servoAddIn[0].expectedAngle = 90;
            servoAddIn[0].regulationRate = millis(); //Record the current time
        }

        if (startButton.isPress())
        { //do nothing
        }

        if (mySerial.available())
        {
            //Read the data from the RF module.
            rfReading = mySerial.read();

            //If the reading has changed,refresh the dataCode.
            if (rfReading != rfDataCode)
            {
                rfDataCode = rfReading;
                switch (rfDataCode) //Execute the command.
                {
                case 5: //KEY1

                    //Save the values into EEPROM
                    eeAddress1 = 2;
                    eeAddress2 = 4;
                    for (int i = 0; i < 6; i++)
                    {
                        EEPROM.put(eeAddress1, servoAddIn[i].minPosition);
                        EEPROM.put(eeAddress2, servoAddIn[i].maxPosition);
                        eeAddress1 += 2 * sizeof(int); //Move to the next group of values.
                        eeAddress2 += 2 * sizeof(int); //Move to the next group of values.
                    }

                    //Exit.
                    mode = RUNNING_MODE;
                    mode_switching = 1;
                    break;
                case 7: //KEY2
                    Serial.println(servoIndex);

                    //Servo points to 90 degrees.
                    servoAddIn[servoIndex].expectedAngle = 90;
                    servoAddIn[servoIndex].regulationRate = millis(); //Record the current time
                    break;
                case 8: //KEY3
                    Serial.println(servoIndex);

                    //Servo points to 0 degrees.
                    servoAddIn[servoIndex].expectedAngle = 0;
                    servoAddIn[servoIndex].regulationRate = millis(); //Record the current time
                    break;
                case 6: //KEY4
                    Serial.println(servoIndex);

                    //Servo points to 180 degrees.
                    servoAddIn[servoIndex].expectedAngle = 180;
                    servoAddIn[servoIndex].regulationRate = millis(); //Record the current time
                    break;
                case 1: //KEY5
                    //Select the servo to be adjusted.
                    servoIndex++;
                    if (servoIndex == 6)
                        servoIndex = 0;
                    for (int i = servoIndex + 1; i > 0; i--)
                    {
                        ontone.tone(1047, 200);
                        delay(200);
                    }
                    Serial.println(servoIndex);
                    //Set the corresponding servo to 90 degrees
                    servoAddIn[servoIndex].expectedAngle = 90;
                    servoAddIn[servoIndex].regulationRate = millis(); //Record the current time
                    break;
                case 3: //KEY6
                    //Select the servo to be adjusted.
                    servoIndex--;
                    if (servoIndex < 0)
                        servoIndex = 5;
                    for (int i = servoIndex + 1; i > 0; i--)
                    {
                        ontone.tone(1047, 200);
                        delay(200);
                    }
                    Serial.println(servoIndex);
                    //Set the corresponding servo to 90 degrees
                    servoAddIn[servoIndex].expectedAngle = 90;
                    servoAddIn[servoIndex].regulationRate = millis(); //Record the current time
                    break;

                case 4: //KEY7
                    Serial.println(servoIndex);

                    servoAddIn[servoIndex].zeroPositionCorrection = 1;
                    servoAddIn[servoIndex].zeroPositionCorrection_direction = COUNTER_CLKWISE;
                    servoAddIn[servoIndex].correctionRate = millis(); //Record the current time
                    break;
                case 2: //KEY8
                    Serial.println(servoIndex);

                    servoAddIn[servoIndex].zeroPositionCorrection = 1;
                    servoAddIn[servoIndex].zeroPositionCorrection_direction = CLKWISE;
                    servoAddIn[servoIndex].correctionRate = millis(); //Record the current time
                    break;
                case 15: //endCode.
                    //Servo stop
                    servoAddIn[servoIndex].zeroPositionCorrection = 0;
                    break;
                }
            }
        }

        //Only servo1 can be adjusted for the moment.
        //In order to adjust the other servos,the servo object should be created first.
        if (servoAddIn[servoIndex].zeroPositionCorrection)
        {
            if (millis() - servoAddIn[servoIndex].correctionRate > 10) //Count for 10ms
            {
                servoAddIn[servoIndex].correctionRate = millis(); //Reset the timer

                if (servoAddIn[servoIndex].zeroPositionCorrection_direction == COUNTER_CLKWISE)
                {
                    if (servoAddIn[servoIndex].minPosition > MIN_POSITION_FLOOR)
                    {
                        servoAddIn[servoIndex].minPosition--;
                        servoAddIn[servoIndex].maxPosition = servoAddIn[servoIndex].minPosition + POSITION_RANGE;
                        switch (servoIndex)
                        {
                        case 0:
                            servo1.attach(servoAddIn[0].pinNbr, servoAddIn[0].minPosition, servoAddIn[0].maxPosition);
                            servo1.write(servoAddIn[0].currentAngle);
                            break;
                        case 1:
                            servo2.attach(servoAddIn[1].pinNbr, servoAddIn[1].minPosition, servoAddIn[1].maxPosition);
                            servo2.write(servoAddIn[1].currentAngle);
                            break;
                        case 2:
                            servo3.attach(servoAddIn[2].pinNbr, servoAddIn[2].minPosition, servoAddIn[2].maxPosition);
                            servo3.write(servoAddIn[2].currentAngle);
                            break;
                        case 3:
                            servo4.attach(servoAddIn[3].pinNbr, servoAddIn[3].minPosition, servoAddIn[3].maxPosition);
                            servo4.write(servoAddIn[3].currentAngle);
                            break;
                        case 4:
                            servo5.attach(servoAddIn[4].pinNbr, servoAddIn[4].minPosition, servoAddIn[4].maxPosition);
                            servo5.write(servoAddIn[4].currentAngle);
                            break;
                        case 5:
                            servo6.attach(servoAddIn[5].pinNbr, servoAddIn[5].minPosition, servoAddIn[5].maxPosition);
                            servo6.write(servoAddIn[5].currentAngle);
                            break;
                        }
                    }
                }
                else
                { //CLKWISE
                    if (servoAddIn[servoIndex].minPosition < MIN_POSITION_CEILING)
                    {
                        servoAddIn[servoIndex].minPosition++;
                        servoAddIn[servoIndex].maxPosition = servoAddIn[servoIndex].minPosition + POSITION_RANGE;
                        switch (servoIndex)
                        {
                        case 0:
                            servo1.attach(servoAddIn[0].pinNbr, servoAddIn[0].minPosition, servoAddIn[0].maxPosition);
                            servo1.write(servoAddIn[0].currentAngle);
                            break;
                        case 1:
                            servo2.attach(servoAddIn[1].pinNbr, servoAddIn[1].minPosition, servoAddIn[1].maxPosition);
                            servo2.write(servoAddIn[1].currentAngle);
                            break;
                        case 2:
                            servo3.attach(servoAddIn[2].pinNbr, servoAddIn[2].minPosition, servoAddIn[2].maxPosition);
                            servo3.write(servoAddIn[2].currentAngle);
                            break;
                        case 3:
                            servo4.attach(servoAddIn[3].pinNbr, servoAddIn[3].minPosition, servoAddIn[3].maxPosition);
                            servo4.write(servoAddIn[3].currentAngle);
                            break;
                        case 4:
                            servo5.attach(servoAddIn[4].pinNbr, servoAddIn[4].minPosition, servoAddIn[4].maxPosition);
                            servo5.write(servoAddIn[4].currentAngle);
                            break;
                        case 5:
                            servo6.attach(servoAddIn[5].pinNbr, servoAddIn[5].minPosition, servoAddIn[5].maxPosition);
                            servo6.write(servoAddIn[5].currentAngle);
                            break;
                        }
                    }
                }
            }
        }
    }
}
