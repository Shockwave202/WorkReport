#include "METALROBO_Module.h"
///////////////////////////////////////////
//					US					
///////////////////////////////////////////
void US::port(unsigned char _pin)
{
	US_pin=_pin;
}
unsigned int US::read(void)
{
	pinMode(US_pin, OUTPUT);
	digitalWrite(US_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(US_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(US_pin, LOW);
	pinMode(US_pin, INPUT);
	distance = pulseIn(US_pin, HIGH);
	delayMicroseconds(200);
	distance = distance / (18.0);
	
}
///////////////////////////////////////////
//					4 BIT FND					
///////////////////////////////////////////
void LEDdisp::port(unsigned char _SCL,unsigned char _SDA)
{
	LED_SCL=_SCL;
	LED_SDA=_SDA;
	pinMode(LED_SCL,OUTPUT);
	pinMode(LED_SDA,OUTPUT);
}
void LEDdisp::TM1650_start(void)
{
  digitalWrite(LED_SDA,HIGH);
  digitalWrite(LED_SCL,HIGH);
  digitalWrite(LED_SDA,LOW);
}
void LEDdisp::TM1650_stop(void)
{
  digitalWrite(LED_SDA,LOW);
  digitalWrite(LED_SCL,HIGH);
  digitalWrite(LED_SDA,HIGH);
}
unsigned char LEDdisp::TM1650_ACK(void)
{
  unsigned char _ACK;
  pinMode(LED_SDA,INPUT);
  _ACK=digitalRead(LED_SDA);
  pinMode(LED_SDA,OUTPUT);
  digitalWrite(LED_SCL,HIGH);
  digitalWrite(LED_SCL,LOW);
  return _ACK;
}
void LEDdisp::TM1650_write_byte(unsigned char _DATA)
{
  unsigned char i;
  for(i=0;i<8;i++)
  {
    digitalWrite(LED_SCL,LOW);
    digitalWrite(LED_SDA,(_DATA & 0x80)>>7);
    _DATA<<=1;
    digitalWrite(LED_SCL,HIGH);
    digitalWrite(LED_SCL,LOW);
  }
  TM1650_ACK();
}
void LEDdisp::bright(unsigned char _grade)
{
  unsigned char disp_en=0;
  if(_grade)
    disp_en=1;
  if(_grade>7)
    _grade=0;
  TM1650_start();
  TM1650_write_byte(0x48);
  TM1650_write_byte((_grade<<4) + 0x04 + disp_en);
  TM1650_stop();
}
void LEDdisp::TM1638_write_data(unsigned char _ADDR,unsigned char _DATA)
{
  TM1650_start();
  TM1650_write_byte(_ADDR);
  TM1650_write_byte(_DATA);
  TM1650_stop();
}
void LEDdisp::sgl(unsigned _NO,unsigned char _DATA)
{
  if(_DATA<10)
    TM1638_write_data((_NO*2)+102,number[_DATA]);
  /*else if(47<_DATA<=58)
    TM1638_write_data((_NO*2)+102,number[_DATA-48]);
  else if(64<_DATA<71)
    TM1638_write_data((_NO*2)+102,number[_DATA-48]);*/
  else
    TM1638_write_data((_NO*2)+102,number[17]);
}
void LEDdisp::all(unsigned int _DATA)
{
	unsigned int number_temp;
	unsigned char bits=0,number_sgl[4];
	number_temp=_DATA;
	do{
		number_temp/=10;
		bits++;
	}while(number_temp);
	number_sgl[0]=_DATA/1000;
	number_sgl[1]=_DATA%1000/100;
	number_sgl[2]=_DATA%100/10;
	number_sgl[3]=_DATA%10;
	for(unsigned char i=0;i<bits;i++)
	{
		sgl(4-i,number_sgl[4-i-1]);
	}
	for(unsigned char i=0;i<(4-bits);i++)
	{
		sgl(4-i-bits,20);
	}
}
void LEDdisp::outputData(unsigned char _SCL,unsigned char _SDA,unsigned int _DATA)
{
	unsigned int number_temp;
	unsigned char bits=0,number_sgl[4];
	if((LED_SCL!=_SCL) || (LED_SDA!=_SDA))
	{
		LED_SCL=_SCL;
		LED_SDA=_SDA;
		pinMode(LED_SCL,OUTPUT);
		pinMode(LED_SDA,OUTPUT);
	}
	number_temp=_DATA;
	do{
		number_temp/=10;
		bits++;
	}while(number_temp);
	number_sgl[0]=_DATA/1000;
	number_sgl[1]=_DATA%1000/100;
	number_sgl[2]=_DATA%100/10;
	number_sgl[3]=_DATA%10;
	for(unsigned char i=0;i<bits;i++)
	{
		sgl(4-i,number_sgl[4-i-1]);
	}
	for(unsigned char i=0;i<(4-bits);i++)
	{
		sgl(4-i-bits,20);
	}
	
}
///////////////////////////////////////////
//					dimmer					
///////////////////////////////////////////
void dimmer::port(unsigned char _pin)
{
	dimmer_pin=_pin;
}
unsigned int dimmer::read(void)
{
	unsigned int dimmer_temp;
	if(dimmer_pin==0)
		dimmer_pin=A0;
	dimmer_temp=map(analogRead(dimmer_pin),0,1023,0,100);
	return dimmer_temp;
	
}


//////////////////////////////////////////////
 //                  RGB-LED
/////////////////////////////////////////////	
void rgb_led::port(unsigned char rgb_led_red, unsigned char rgb_led_green, unsigned char rgb_led_blue)
{
 
	R_pin=rgb_led_red;
	G_pin=rgb_led_green;
	B_pin=rgb_led_blue;
    pinMode(R_pin, OUTPUT);
    pinMode(G_pin, OUTPUT);
    pinMode(B_pin, OUTPUT);
}	

void  rgb_led::color(unsigned char red,unsigned char green,unsigned char blue)
 {   
	if(red > 255)
		red =255;
	if(green > 255)
		green =255;
	if(blue > 255)
		blue =255;
	
   unsigned char red_temp,green_temp,blue_temp;
      red_temp=255-red;
	  green_temp=255-green;
	  blue_temp=255-blue;
     if(R_pin==0)
		 R_pin=11;
	 if(G_pin==0)
		 G_pin=12;
	 if(B_pin==0)
		 B_pin=13;
	analogWrite(R_pin, red_temp);
	analogWrite(G_pin, green_temp);
	analogWrite(B_pin, blue_temp);
	   
 } 
 ///////////////////////////////////////
 //             passive buzzer
 ////////////////////////////////////////
int Tone_on[8] = {1047,1175,1319,1397,1568,1760,1976,0};
// notes in the melody:
int melody_frog[] = {
NOTE_D5,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_D5,NOTE_D5,  //小青蛙
NOTE_D5,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_A4,NOTE_A4,
NOTE_A4,NOTE_D5,NOTE_A4,NOTE_D5,NOTE_C5,NOTE_C5,
NOTE_D5,NOTE_D5,NOTE_C5,NOTE_E5,NOTE_D5,NOTE_D5,
NOTE_D5,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_D5,NOTE_C5,NOTE_D5,
NOTE_D5,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_A4,NOTE_A4,
NOTE_A4,NOTE_D5,NOTE_A4,NOTE_D5,NOTE_C5,NOTE_A4,NOTE_C5,
NOTE_D5,NOTE_D5,NOTE_C5,NOTE_E5,NOTE_D5,NOTE_D5,
};
int noteDurations_frog[] = {
  4,4,4,4,2,2,
  4,4,4,4,2,2,
  4,4,4,4,2,2,
  4,4,4,4,2,2,
  4,4,4,4,4,4,2,
  4,4,4,4,2,2,
  4,4,4,4,4,4,2,
  4,4,4,4,2,2,
};
int frog_thisNote=0;
int frog_mylen = sizeof(melody_frog)/sizeof(melody_frog[0]);
char star_notes[] = "ccggaagffeeddc"; // a space represents a rest
int star_beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2 };
char star_names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
int star_tones[] = { 523,587,659,698,784,880,988,262 };

 void buzzer::port(unsigned char _pin)
 {
    buzzer_pin=_pin;	 
 }
 
 void buzzer::tone(int pin, uint16_t frequency, uint32_t duration)
{
  buzzer_pin = pin;
  int period = 1000000L / frequency;
  int pulse = period / 2;
  pinMode(buzzer_pin, OUTPUT);
  for (long i = 0; i < duration * 1000L; i += period) 
  {
    digitalWrite(buzzer_pin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(buzzer_pin, LOW);
    delayMicroseconds(pulse);
  }
} 
 
void buzzer::tone(uint16_t frequency, uint32_t duration)
{
  int period = 1000000L / frequency;
  int pulse = period / 2;
  pinMode(buzzer_pin, OUTPUT);
  for (long i = 0; i < duration * 1000L; i += period) 
  {
    digitalWrite(buzzer_pin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(buzzer_pin, LOW);
    delayMicroseconds(pulse);
  }
}

void buzzer::noTone(int pin)
{
  buzzer_pin = pin;
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
}

void buzzer::noTone()
{
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
}

void buzzer::frog(){
	for (frog_thisNote = 0; frog_thisNote < frog_mylen; frog_thisNote++) {
		  int noteDuration = 1000 / noteDurations_frog[frog_thisNote];
		  buzzer::tone(buzzer_pin, melody_frog[frog_thisNote], noteDuration);
		  int pauseBetweenNotes = noteDuration * 0.30;
		  delay(pauseBetweenNotes);
		  buzzer::noTone(buzzer_pin);
	}
}

void buzzer::on(){
  buzzer::tone(Tone_on[0], 200);
  buzzer::tone(Tone_on[1], 200);
  buzzer::tone(Tone_on[0], 200);
  buzzer::tone(Tone_on[1], 200);
  buzzer::tone(Tone_on[0], 200);
  buzzer::tone(Tone_on[1], 200);
}

void buzzer::star(){
  for (int i = 0; i < 15; i++) {
    if (star_notes[i] == ' ') {
      delay(star_beats[i] * 300);
    } else {
      for (int j = 0; j < 8; j++) {
        if (star_names[j] == star_notes[i]) {
          buzzer::tone(star_tones[j], star_beats[i] * 300);
        }
      }
    }
    delay(300 / 2); 
  }
}


