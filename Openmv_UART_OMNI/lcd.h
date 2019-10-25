/*
 * lcd.c
 *
 * Created: 19-12-2018 13:30:55
 * Author : Sahil
 */ 

#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC
#include <avr/io.h>
#include <util/delay.h>

void lcd_reset();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_line1();
void lcd_line2();
void lcd_string(char*);
void lcd_print(char row, char column, unsigned int value, int digits);

unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;

char var[]="over digit";

//Function to Reset LCD
void lcd_set_4bit()
{
	_delay_ms(1);

	lcd_port&=~(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port = 0x30;				//Sending 3
	lcd_port|=(1<<EN);
	_delay_ms(1);					//Delay
	lcd_port&=~(1<<EN);

	_delay_ms(1);

	lcd_port&=~(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port = 0x30;				//Sending 3
	lcd_port|=(1<<EN);
	_delay_ms(1);					//Delay
	lcd_port&=~(1<<EN);

	_delay_ms(1);

	lcd_port&=~(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port = 0x30;				//Sending 3
	lcd_port|=(1<<EN);
	_delay_ms(1);					//Delay
	lcd_port&=~(1<<EN);

	_delay_ms(1);

	lcd_port&=~(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port = 0x20;				//Sending 2 to initialize LCD 4-bit mode
	lcd_port|=(1<<EN);
	_delay_ms(1);					//Delay
	lcd_port&=~(1<<EN);	
}

//Function to Initialize LCD
void lcd_init()
{
	lcd_set_4bit();
	_delay_ms(1);

	lcd_wr_command(0x28);			//LCD 4-bit mode and 2 lines.
	lcd_wr_command(0x01);
	lcd_wr_command(0x06);
	lcd_wr_command(0x0E);
	lcd_wr_command(0x80);
	
}

//Function to Write Command on LCD
void lcd_wr_command(unsigned char cmd)
{
	lcd_port=(cmd & 0xF0);			//send higher nibble

	lcd_port&=~(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port|=(1<<EN);	
	_delay_ms(2);
	lcd_port&=~(1<<EN);

	lcd_port=((cmd<<4)& 0xF0);		//send lower nibble

	lcd_port&=~(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port|=(1<<EN);
	_delay_ms(2);
	lcd_port&=~(1<<EN);
}

//Function to Write Data on LCD
void lcd_wr_char(char letter)
{
	lcd_port=(letter & 0xF0);			//send higher nibble

	lcd_port|=(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port|=(1<<EN);
	_delay_ms(1);
	lcd_port&=~(1<<EN);

	lcd_port=((letter<<4)& 0xF0);		 //send lower nibble

	lcd_port|=(1<<RS);
	lcd_port&=~(1<<RW);
	lcd_port|=(1<<EN);
	_delay_ms(1);
	lcd_port&=~(1<<EN);
}


//Function to Print String on LCD
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

//Position the LCD cursor at "row", "column".
void lcd_cursor (char row, char column)
{
	switch (row)
	{
		case 1: lcd_wr_command (0x80 + column - 1); break;
		case 2: lcd_wr_command (0xc0 + column - 1); break;
		default: break;
	}
}

//Function To Print Any input value upto the desired digit on LCD
void lcd_print (char row, char column, unsigned int value, int digits)
{
	unsigned char flag=0;
	if(row==0||column==0)
	{
		lcd_wr_command(0x80);
	}
	else
	{
		lcd_cursor(row,column);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		lcd_wr_char(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcd_wr_char(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcd_wr_char(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcd_wr_char(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcd_wr_char(unit);
	}
	if(digits>5)
	{
		lcd_string(var);
	}
}


