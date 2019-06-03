/*
 * TempReader.c
 *
 * Created: 5/23/2019 10:00:59 AM
 * Author : ucrcse
 */ 
//code source  :https://github.com/Andre-Castro/EE120BSpr17/tree/master/acast050_FinalProject
#define F_CPU 8000000
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include "nokia5110.c"

#define LCD_Port	PORTC
#define CE			PINC5	//PINB0		// SCE chip enable, active low		->	SS
#define RST			PINC6	//PINB1		// reset pin
#define DC			PINC4	//PINB2		// Mode Select, data (high)/character(low)
#define DIN			PINC3	//PINB3		// data in							->	MOSI
#define CLK			PINC2	//PINB4		// clocking the data to the LCD		->	SCLK
#define DHT11_PIN 6
#define LCD_DataDir	DDRC


/**
 * Sending data to LCD
 * @bytes: data
 * @is_data: transfer mode: 1 - data; 0 - command;
 */

uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;
void Request()				/* Microcontroller send start pulse/request */
{
	DDRA |= (1<<DHT11_PIN);
	PORTA &= ~(1<<DHT11_PIN);	/* set to low pin */
	_delay_ms(20);			/* wait for 20ms */
	PORTA |= (1<<DHT11_PIN);	/* set to high pin */
}
void Response()				/* receive response from DHT11 */
{
	DDRA &= ~(1<<DHT11_PIN);
	while(PINA & (1<<DHT11_PIN));
	while((PINA & (1<<DHT11_PIN))==0);
	while(PINA & (1<<DHT11_PIN));
}
uint8_t Receive_data()			/* receive data */
{
	for (int q=0; q<8; q++)
	{
		while((PINA & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_ms(30);
		if(PINA & (1<<DHT11_PIN))/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01);	/* then its logic HIGH */
		else			/* otherwise its logic LOW */
		c = (c<<1);
		while(PINA & (1<<DHT11_PIN));
	}
	return c;
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; //Input
	DDRB = 0x00; PORTB = 0xFF; //Ouptut
	DDRC = 0xFF; PORTC = 0x00; 
	DDRD = 0xFF; PORTD = 0x00;
	nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_write_string("IT'S WORKING!",1);
    nokia_lcd_set_cursor(0, 10);
    nokia_lcd_write_string("Nice!", 3);
    nokia_lcd_render();
	
    /* Replace with your application code */
    while (1) 
    {
		Request();		/* send start pulse */
		Response();		/* receive response */
		I_RH=Receive_data();	/* store first eight bit in I_RH */
		D_RH=Receive_data();	/* store next eight bit in D_RH */
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		D_Temp=Receive_data();	/* store next eight bit in D_Temp */
		CheckSum=Receive_data();/* store next eight bit in CheckSum */
		//PORTB = I_Temp;
    }
}

