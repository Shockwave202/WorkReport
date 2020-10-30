
  Motor motor1,motor2;//Create two motor objects.                


void setup() {
  // put your setup code here, to run once:
motor1.attach(8,5);//Connect the digital pin to the motor driver.
motor2.attach(7,6);

}

void loop() {
  // put your main code here, to run repeatedly:

motor1.rotateClkwise();
motor2.stop();



}
