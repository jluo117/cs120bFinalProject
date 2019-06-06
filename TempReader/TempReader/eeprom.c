#include <avr/io.h>
#include <avr/eeprom.h>
#include "eeprom.h"

void EEPROM_Write(unsigned char address, unsigned char data)
{
    eeprom_write_byte(address, data);
    /*
	//Wait for previous write to finish
	while(EECR & (1 << EEPE));
	
	// Set up address and data registers
	EEAR = address;
	EEDR = data;
	
	// Enable writing
	EECR |= (1 << EEMPE);
	
	// Begin Write
	EECR |= (1 << EEPE);
	*/
}

unsigned char EEPROM_Read(unsigned char address)
{
    return eeprom_read_byte(address);
    /*
	//Wait for previous write to finish
	while(EECR & (1 << EEPE));
	
	// Set up address register
	EEAR = address;
	
	// Begin Read
	EECR |= (1 << EERE);
	
	//Return data register
	return EEDR;
	*/
}