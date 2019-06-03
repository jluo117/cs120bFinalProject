/*
 * ATmega16_SPI_7_segment_display.c
 *
 * http://www.electronicwings.com
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "SPI_Master_H_file.h"

void display(char address, char data)
{
	SPI_Write(address);
	SPI_Write(data);
	SPI_Restart();
}

void display_init()
{
	display(0x0C, 0x01);	/* normal mode operation */
	display(0x09, 0xFF);	/* decode mode */
	display(0x0A, 0x0F);	/* set intensity at high level */
	display(0x0B, 0x07);	/* set scan limit */
}

void display_test()
{
	display(0x0F, 0x01);	/* display test mode */
}

void display_clear()
{
	display(0x0F, 0x00);	/* display normal mode */
	for (int i=0;i<9;i++)
	{
		display(i, 0x0F);
	}
}

int main(void)
{
	SPI_Init();				/* initialize SPI */
	_delay_ms(1);
	SPI_SS_Enable();		/* enable SS pin */
	display_test();			/* test the all digit are display */
	display_init();			/* initialize display */
	_delay_ms(1000);
	display_clear();		/* clear all digit of all display */

	
	for (int b=0;b<10;b++)
	{
		for (int i=0;i<10;i++)
		{
			display(0x01, i);
			_delay_ms(300);
		}
		if(b<9)
		display(0x02, b+1);
	}

	SPI_SS_Disable();
	while(1);
}