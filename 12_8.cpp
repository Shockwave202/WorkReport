//Program structure:
 if (Pushbutton1.isPress())
  {
    program_selection_procedure = 1;
    pgm_index = 1;
  }

  if (program_selection_procedure)
  {
    do
    {
      //Program selection procedure
      switch (pgm_index)
      {
      case 1:
        ontone.tone(Tone_data[3][0], 500);
        break;
      case 2:
        ontone.tone(Tone_data[3][1], 500);
        break;
      case 3:
        ontone.tone(Tone_data[3][2], 500);
        break;
      case 4:
        ontone.tone(Tone_data[3][3], 500);
        break;
      case 5:
        ontone.tone(Tone_data[3][4], 500);
        break;
      case 6:
        ontone.tone(Tone_data[3][5], 500);
        break;
      case 7:
        ontone.tone(Tone_data[3][6], 500);
        break;
      case 8:
        ontone.tone(Tone_data[4][0], 500);
        break;
      case 9:
        ontone.tone(Tone_data[4][1], 500);
        break;
      case 10:
        ontone.tone(Tone_data[4][2], 500);
        break;
      case 11:
        ontone.tone(Tone_data[4][3], 500);
        break;
      }
      pgm_index++; //Increasing while in the loop
      if (pgm_index == 12)
        pgm_index = 1; //Return to the beginning.
    } while (!Pushbutton1.isRelease());
    program_selection_procedure = 0; //Get out of here
    //After getting out,recover the index
    if (pgm_index == 1)
      pgm_index = 11;
    else
      pgm_index--;
    //Check the result.
    Serial.print("pgm_index:");
    Serial.println(pgm_index);
	pgm_switching=1;//This bit indicates a pgm switching event occurs,
	                //put some setup code to run once:
  }
  
  switch(pgm_index)//11
{
case 1://Frog
      if(pgm_switching)//put some setup code to run once:
	  {pgm_switching=0;
	   pinMode(D1_BOARD, OUTPUT);//Attach 2 LEDs.
       pinMode(D2_BOARD, OUTPUT);
       Pushbutton2.create(D3_BOARD);//Attach a pushbutton to D3 pin on the board      		  
       Frog_robot();		  
		  
	  }
break;
case 2:
break;	
//...	
	
	
case 11:
break;		
}
  
 bool aircraft;	
//Propeller-driven aircraft.
    pinMode(D1_BOARD,OUTPUT);//Attach LED1 to D1 pin
    digitalWrite(D1_BOARD,LOW);
    pinMode(D2_BOARD,OUTPUT);//Attach LED2 to D2 pin
    digitalWrite(D2_BOARD,LOW);
    Pushbutton1.create(START);

 /*--START the program--*/
  if (Pushbutton1.isPress())
  {
   //Propeller-driven aircraft.
   aircraft=1;
  } //Play the song for once.


if(aircraft)
  {
    MRMotor.M_pri(M1, F, 255);
    MRMotor.M_pri(M2, F, 255);
    digitalWrite(2,HIGH);
    digitalWrite(10,HIGH);
    ontone.star();
    digitalWrite(2,LOW);
    digitalWrite(10,LOW);
    delay(100);
    MRMotor.M_pri(M1, B, 255);
    MRMotor.M_pri(M2, B, 255);
    digitalWrite(2,HIGH);
    digitalWrite(10,HIGH);
    ontone.star();
    digitalWrite(2,LOW);
    digitalWrite(2,LOW);
    delay(100);	    
  } 
  
    //Frog program setup
  pinMode(D1_BOARD, OUTPUT); //Attach 2 LEDs.
  pinMode(D2_BOARD, OUTPUT);
  Pushbutton1.create(START);
  Pushbutton2.create(D3_BOARD); //Attach a pushbutton to D3 pin on the board
  frog_mode = 0;
  frog_is_jumping = 0;

  //Frog program loop
 //Mode selection.
  if (Pushbutton2.isPress()) //Single click
  {
    frog_mode = 1;
  }
  if (Pushbutton2.isDoubleClick()) //Effective time interval:1S
  //Start the program
  {
    frog_mode = 2;
  }

  /*--START the program--*/
  if (Pushbutton1.isPress())
  {
    if (frog_mode)//If mode has been selected,start the program.
      frog_is_jumping = 1;

  } //Play the song for once.

  if (frog_is_jumping)
  {
    digitalWrite(D1_BOARD, HIGH); //Enlight LEDs
    digitalWrite(D2_BOARD, HIGH);
    MRMotor.M_pri(M2, F, 255); //Activate the motors
    MRMotor.M_pri(M1, F, 255);
    //Play the song.
    do
    {
      ontone.star();
      if (frog_mode == 2) //Play again
        ontone.star();
    } while (Pushbutton2.isPress());
    //If the button has been pressed during singing,play again.
    MRMotor.M_pri(M1, F, 0); //Deactivate 
    MRMotor.M_pri(M2, F, 0);
    digitalWrite(2, LOW);
    digitalWrite(10, LOW);
    frog_mode=0;//Reset the mode 
    frog_is_jumping = 0;
  }
  
  //Self tracking vehicle
  if(analogRead(A1_BOARD)>900 && analogRead(A2_BOARD)>900)
{//Both left IR sensor and right IR sensor detect the white floor,go straight forward;
        MRMotor.M_pri(M1, F, 150);
        MRMotor.M_pri(M2, F, 150);
        digitalWrite(2,LOW);
        digitalWrite(10,LOW);
		
}
else if (analogRead(A1_BOARD)<200 && analogRead(A2_BOARD)>900)
{
//Left IR sensor has detected the black stripe,right IR sensor has detected the white floor,
//turn left under this condition.	
        MRMotor.M_pri(M1, F, 150);
        MRMotor.M_pri(M2, B, 150);
        digitalWrite(10,HIGH);	
	
}
else if (analogRead(A1_BOARD)>900 && analogRead(A2_BOARD)<200)
{//Right IR sensor has detected the black stripe,left IR sensor has detected the white floor,
//turn right under this condition.
          MRMotor.M_pri(M1, B, 150);
          MRMotor.M_pri(M2, F, 150);
          digitalWrite(2,HIGH);

}
else if (analogRead(A1_BOARD)<200 && analogRead(A2_BOARD)<200)
{//Right IR sensor has detected the black stripe,left IR sensor has detected the black stripe,
//stop under this condition.
          MRMotor.M_pri(M1, B, 0);
          MRMotor.M_pri(M2, F, 0);
          digitalWrite(2,HIGH);
		  digitalWrite(10,LOW);

}
  
  
  
