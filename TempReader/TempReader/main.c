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


#define DHT11_PIN 6


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

/**
 * Sending data to LCD
 * @bytes: data
 * @is_data: transfer mode: 1 - data; 0 - command;
 */
unsigned float y_axies = ADCSRA(1);
unsigned float x_axis = ADCSRA(0);
enum joyStick {down,up,left,right,still} Joystate;
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;
void A2D_init(){
	ADCSRA |= (1 >> ADEN) | (1 << ADSC) | (1 << ADATE);
}
void joystickState(){
	if(y_axies >= 1023 && x_axis <= 1023){
		Joystate = down;
	}
	else if (y_axies <= 25 && x_axis <= 1023){
		Joystate = up;
	}
	else if (x_axis >= 1023 && y_axies <= 1023){
		Joystate = left;
	}
	else if (x_axis >= 1023 && y_axies <= 1023){
		Joystate = right;
	}
	else{
		Joystate = still;
	}
}
void Request()				/* Microcontroller send start pulse/request */
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);	/* set to low pin */
	_delay_ms(20);			/* wait for 20ms */
	PORTD |= (1<<DHT11_PIN);	/* set to high pin */
}

void Response()				/* receive response from DHT11 */
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data()			/* receive data */
{
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01);	/* then its logic HIGH */
		else			/* otherwise its logic LOW */
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}
const unsigned short PERIOD = 10;
void writeValues(int myValue){
	char snum[5];
	itoa(myValue, snum, 10);
	nokia_lcd_clear();
	nokia_lcd_write_string(snum,3);
	nokia_lcd_render();
}
int getTemp(){
	Request();		/* send start pulse */
	Response();		/* receive response */
	I_RH=Receive_data();	/* store first eight bit in I_RH */
	D_RH=Receive_data();	/* store next eight bit in D_RH */
	I_Temp=Receive_data();	/* store next eight bit in I_Temp */
	D_Temp=Receive_data();
	return I_Temp	;/* store next eight bit in D_Temp */
	
}
void updateNum(){
	switch(Joystate){
		case up:
			num++;
			break;
		case down:
			num--;
			break;
		default:
		break;
	}
}
int num = 100;
int main(void)
{
	TimerSet(PERIOD);
	DDRA = 0x00; PORTA = 0xFF; //Input
	DDRB = 0x00; PORTB = 0xFF; //Ouptut
	DDRC = 0xFF; PORTC = 0x00; 
	DDRD = 0xFF; PORTD = 0x00;
	nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_write_string("IT'S WORKING!",1);
    nokia_lcd_set_cursor(0, 10);
    //nokia_lcd_write_string("Nice!", 3);
	writeValues(100);
	
    /* Replace with your application code */
    while (1) 
    {
		
		joystickState();
		updateNum();
		writeValues(num);
		
		/* store next eight bit in CheckSum */
		//PORTB = I_Temp;
    }
}

