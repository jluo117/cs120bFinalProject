#ifndef __eeprom_h__
#define __eeprom_h__

void EEPROM_Write(unsigned char address, unsigned char data);
unsigned char EEPROM_Read(unsigned char address);

#endif