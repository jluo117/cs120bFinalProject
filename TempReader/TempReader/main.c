/*
 * TempReader.c
 *
 * Created: 5/23/2019 10:00:59 AM
 * Author : ucrcse
 */ 
#define F_CPU 8000000
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#define DHT11_PIN 6
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
	DDRA = 0x00;
	PORTA = 0xFF;
	DDRB = 0xFF;
	PORTB = 0x00;
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
		PORTB = I_Temp;
    }
}

