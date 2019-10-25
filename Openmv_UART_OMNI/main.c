/*
 * Openmv_UART_OMNI.c
 *
 * Created: 01-10-2019 17:50:46
 * Author : Sameeksha
 */


#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include "lcd.h"
#define lcd_port PORTC

#include <math.h>
#include <stdlib.h> // to use the abs function else a warning occurs
#define BAUD 9600
#define BAUDRATE  ((F_CPU/(BAUD*16UL)-1))
#define lcd_port PORTC

int x1=0;
int y11=0,x2=0,y22=0;
int val1,val2,val3;
int r = 20;
int theta=0,new_theta=0;
int prev_angle=0,diff = 0;
float p=0,kp=3;
int s_continue = 1;
long map_value(long val,long fl,long fh,long tl,long th )
{
long v = ( ((val-fl)*(th-tl)) / (fh-fl));
return ((v+tl));
}
void initADC()
{
ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR);  // AVcc //  right adjusted
ADCSRA = (1<<ADEN)|(0<<ADATE)|(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // bit4 ADC EOC flag // prescalar- 111 - 128 division factor
ADCSRB = 0x00;
}

uint16_t readadc(uint8_t ch)
{
ADMUX = ADMUX & 0b11100000;    //Clearing all the mux
ADCSRB = ADCSRB & 0b11110111;  //------"-"-----------
ch = ch & 0b00001111;
if ( ch <= 7 )
{
ch = ch & 0b00000111; //
ADMUX = ADMUX | ch;
ADCSRB=0x00;
}
else
{
ch = ch-8;
ch = ch & 0b00000111;
ADMUX = ADMUX | ch;
ADCSRB=0x00;
ADCSRB = ADCSRB | (1<<MUX5);
}

ADCSRA = ADCSRA | (1<<ADSC);    //Bit 6 to start conversion-ADSC

while( !(ADCSRA & (1<<ADIF)) ); // Wait for conversion to complete

return(ADC);

}

void serialstart_3()
{
UBRR3H=BAUDRATE>>8;
UBRR3L=BAUDRATE;
UCSR3B=0b10011000;//enable RXEN TXEN
UCSR3C=0b00000110;// UCSZ1 UCSZ0
}
ISR(USART3_RX_vect)
{
//PORTB = UDR3;
lcd_cursor(0,2);
//lcd_print(1,1,UDR3,5);
theta = UDR3;
}
void go_left()
{
// wheel 1// PA0=0 PA1=1
PORTA |= (1<<PA0);
PORTA &= ~(1<<PA1);
// wheel 2//clockwise
PORTA |= (1<<PA2);
PORTA &=  ~(1<<PA3);
//wheel 3
PORTA |= (1<<PA4);
PORTA &= ~(1<<PA5);
}
void go_right()
{
//wheel 1
PORTA |= (1<<PA1);
PORTA &= ~(1<<PA0);
//wheel 2
PORTA |= (1<<PA2) ;
PORTA &=  ~(1<<PA3);
//wheel 3
PORTA |= (1<<PA4) ;
PORTA &=  ~(1<<PA5);
}
void straight()
{
//wheel1
PORTA |= (1<<PA0)|(1<<PA1);
//wheel 2
PORTA |= (1<<PA3);
PORTA &=  ~(1<<PA2);
//wheel 3
PORTA |= (1<<PA4) ;
PORTA &=  ~(1<<PA5);

}
void stop()
{
	PORTB = 0xFF;
	OCR3A = 0;
	OCR3B = 0;
	OCR3C = 0;
	PORTA |= (1<<PA0)|(1<<PA1)|(1<<PA2)|(1<<PA3)|(1<<PA4)|(1<<PA5);
}
void speed(int angle,int x1,int y11)
{
	p=kp*angle;
	val1 = (0.67*y11 + 0.33*p );
	val2 = (-0.58*x1 - 0.33*y11 + 0.33*p);
	val3 = (0.58*x1 - 0.33*y11 + 0.33*p);
	OCR3A = abs(val1);
	OCR3B = abs(val2);
	OCR3C = abs(val3);

// if(angle>-14 && angle<14)
// {
// //PORTB = 0xFF;
//
// straight();
// }
// else
	if(angle>=1 && angle<=52)
	{
	PORTA |= (1<<PA0) ;
	PORTA &=  ~(1<<PA1);
	//wheel 2
	PORTA |= (1<<PA3);
	PORTA &=  ~(1<<PA2);
	//wheel 3
	PORTA |= (1<<PA4) ;
	PORTA &=  ~(1<<PA5);
	}

   else if(angle<=-1 && angle>=-52)
  {
	  PORTA |= (1<<PA1) ;
	  PORTA &=  ~(1<<PA0);
	//wheel 2
	PORTA |= (1<<PA3);
	PORTA &=  ~(1<<PA2);
	//wheel 3
	PORTA |= (1<<PA4) ;
	PORTA &=  ~(1<<PA5);
  }
  else{
	  stop();
  }
}
 
int main(void)
{
//_delay_ms(3000);
sei();
initADC();
TCCR3A |= (1 << COM3A1) |(1 << COM3B1) | (1<< COM3C1) | (0 << WGM31) |(1<<WGM30);//fast pwm where 8 bit reso.
TCCR3B |=  ( 1 << WGM32 );//fast pwm
TCCR3B |= (1 << CS32)|(0 << CS31)|(0 << CS30);//256 prescaling.

DDRB = 0xFF;
DDRA = 0xFF;// motor inputs.(IN1 AND IN2)
DDRE = 0xFF;//pwm for motors
DDRJ = 0x00;//usart receive
DDRC = 0xFF;
//DDRB = 0xFF;//led output
serialstart_3();
lcd_init();
while (1)
{
	new_theta = map_value(theta,101,207,-53,54);
	//lcd_print(2,9,p,5);
	x1 = 200;
	y11 = new_theta;
  if(s_continue == 1)
  { 
	
	//PORTB = theta;
	if(new_theta == 54){
			speed(new_theta,x1,y11);
		//s_continue = 0;
	  }
	else{
	lcd_cursor(0,2);
	//lcd_print(2,2,new_theta,5);

	//x1 = abs(r*cos(new_theta));
	//y11 =  abs(r*sin(new_theta));
	

	speed(new_theta,x1,y11);
	//prev_angle = new_theta; 
	}
	//if values are positive go to right if negative  b=go to left.if between 0-2 drive straight

	}
 }
}