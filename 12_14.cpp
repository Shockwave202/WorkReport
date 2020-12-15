
//Program for obstascle avoidance vehicle ctrl.

samplingRateForDebouncing;

             samplingRateForDebouncing++; 
           if(samplingRateForDebouncing==1000)
		   {samplingRateForDebouncing=0;
	    /*----Toggle D4_BOARD-----
      if (digitalRead(D4_BOARD))
        digitalWrite(D4_BOARD, LOW);
      else
        digitalWrite(D4_BOARD, HIGH);
        /*--------------------*/
	    if(analogRead(A1_BOARD)<500)//No object in front of the IR sensor.
		  {analogReadSelflock=0;
		   analogReadDebouneCount=0;  
		  }
	   else {//(analogRead(A1_BOARD)>500
	         //If an obstacle in front of the car is detected,
			 //this value will rise.
		   if(!analogReadSelflock)
		   {analogReadDebouneCount++;
		   if(analogReadDebouneCount==50)//50ms
		     {analogReadSelflock=1;
			  avoidObstacleInFront=1; 
			 }   			   
		   } 
	   }  
	   /*-------------------------------------*/
	   if(analogRead(A2_BOARD)<500)//No object in front of the IR sensor.
		  {analogReadSelflock=0;
		   analogReadDebouneCount=0;  
		  }
	   else {//(analogRead(A1_BOARD)>500
	         //If an obstacle in front of the car is detected,
			 //this value will rise.
		   if(!analogReadSelflock)
		   {analogReadDebouneCount++;
		   if(analogReadDebouneCount==50)//50ms
		     {analogReadSelflock=1;
			  avoidObstacleOnTheFloor=1; 
			 }   			   
		   } 
	   }    
	 /*--------------------------------*/  
	   
		   }

         
		  

  
   
    if (avoidObstacleInFront) {//If an obstacle in front of the car is detected,
	                           //move backward,then wait for a while.
      avoidObstacleInFront=0;
  //Move backward.
      motor1_expected_direction = COUNTER_CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = COUNTER_CLKWISE;
	  motor2_expected_speed = 255;
	  avoidObstacleInFront_wait700ms=1;
	  // reset the timer
      avoidObstacleInFront_wait700msTimer = millis();
	}
	
	//Wait for 700ms,then turn right 
	if(avoidObstacleInFront_wait700ms)
	{if ((millis() - avoidObstacleInFront_wait700msTimer) > 700)
		{avoidObstacleInFront_wait700ms=0;
      //Turn right 
	  motor1_expected_direction = CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = COUNTER_CLKWISE;
	  motor2_expected_speed = 255; 	
		}	
	}
	
 if (avoidObstacleOnTheFloor) {//If a black stripe under the car is detected,
	                           //move backward,then wait for a while.
      avoidObstacleOnTheFloor=0;//Run the program for once.
      //Move backward.
      motor1_expected_direction = COUNTER_CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = COUNTER_CLKWISE;
	  motor2_expected_speed = 255;
	  avoidObstacleOnTheFloor_wait700msFlag=1;
	  // reset the timer
      avoidObstacleOnTheFloor_wait700msTimer = millis();
	}	
	
	//Wait for 700ms,then turn right 
	if(avoidObstacleOnTheFloor_wait700msFlag)
	{if ((millis() - avoidObstacleOnTheFloor_wait700msTimer) > 700)
		{avoidObstacleOnTheFloor_wait700msFlag=0;
      //Turn right 
	  motor1_expected_direction = CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = COUNTER_CLKWISE;
	  motor2_expected_speed = 255; 	
		}	
	}


   if(pushButton2.isPress())
   {
       //wait for 150ms
	  shaking_wait150msFlag=1;
	  shaking_wait150msTimer = millis();
	  shakingState=TURN_LEFT; 
      motor1_expected_direction = CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = COUNTER_CLKWISE;
	  motor2_expected_speed = 255; 
 	  
   }
   
   if(pushButton2.isRelease())
   { shaking_wait150msFlag=0;
	 digitalWrite(MOTOR1_INB, LOW); 
	 analogWrite(MOTOR1_INA,0); 
	   
   }
  
	   if(shaking_wait150msFlag)
	   {
		  if ((millis() - shaking_wait150msTimer) > 150)//Exceed 150ms 
		 { 
          shaking_wait150msTimer=millis();//Set the timer value equal to the current time.
	 if(shakingState==TURN_LEFT)
	   {shakingState=TURN_RIGHT;
	  motor1_expected_direction = COUNTER_CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = CLKWISE;
	  motor2_expected_speed = 255;       
	   }
	 else //shakingState=TURN_RIGHT
	 {shakingState=TURN_LEFT;
	  motor1_expected_direction = CLKWISE;
      motor1_expected_speed = 255;
	  motor2_expected_direction = COUNTER_CLKWISE;
	  motor2_expected_speed = 255;  	 
		 
	 }		  
		  
	}		 
   }
