#include "OLED_Driver.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <stdio.h>

#define INTERFACE_4WIRE_SPI 1
#define INTERFACE_3WIRE_SPI !(INTERFACE_4WIRE_SPI)

#if INTERFACE_4WIRE_SPI
  #if INTERFACE_3WIRE_SPI
    #error "SPI Setting Error !!"
  #endif
#elif INTERFACE_3WIRE_SPI

#else
  #error "SPI Setting Error !!"
#endif



uint8_t color_byte[2],color_fill_byte[2];

uint8_t oled_cs = 8;
uint8_t oled_dc = 24;
uint8_t oled_rst= 25;

void OLED_CS(uint8_t x) {
  digitalWrite(oled_cs, x);
}

void OLED_DC(uint8_t x) {
  digitalWrite(oled_dc, x);
}

void OLED_RST(uint8_t x)  {
  digitalWrite(oled_rst, x);
}

#if INTERFACE_3WIRE_SPI

uint8_t oled_sck = 11;
uint8_t oled_din = 10;

void OLED_SCK(uint8_t x)  {
  digitalWrite(oled_sck, x);
}

void OLED_DIN(uint8_t x)  {
  digitalWrite(oled_din, x);
}

#endif

void Set_Color(uint16_t color)  {
  color_byte[0] = (uint8_t)(color >> 8);
  color_byte[1] = (uint8_t)(color & 0x00ff);
}


void Set_FillColor(uint16_t color)  {
  color_fill_byte[0] = (uint8_t)(color >> 8);
  color_fill_byte[1] = (uint8_t)(color & 0x00ff);
}


void Write_Command(uint8_t cmd)  {
  
  OLED_CS(LOW);
  
#if  INTERFACE_4WIRE_SPI
  
  uint8_t read_data;

  OLED_DC(LOW);
  
  read_data = wiringPiSPIDataRW(0,&cmd,1);
  if(read_data < 0)
    perror("wiringPiSPIDataRW\r\n");
  
#elif INTERFACE_3WIRE_SPI
  
  uint8_t i;
  uint16_t hwData = 0;
  
  hwData = (uint16_t)cmd & ~0x0100;

  for(i = 0; i < 9; i ++) {
    OLED_SCK(LOW);
    if(hwData & 0x0100) {
      OLED_DIN(HIGH);
    }
    else  {
      OLED_DIN(LOW);
    }
    OLED_SCK(HIGH);
    hwData <<= 1;
  }

#endif
  
  OLED_CS(HIGH);
}


void Write_Data(uint8_t dat) {
  
  OLED_CS(LOW);
  
#if INTERFACE_4WIRE_SPI
  
  uint8_t data_read;

  OLED_DC(HIGH);
  
  data_read = wiringPiSPIDataRW(0,&dat,1);
  
  if(data_read < 0)
    perror("WiringPiSPIDataRW error!\r\n");

#elif INTERFACE_3WIRE_SPI
  
  uint8_t i;
  uint16_t hwData = 0;
  
  hwData = (uint16_t)dat | 0x0100;
	
  for(i = 0; i < 9; i ++) {
    OLED_SCK(LOW);
    if(hwData & 0x0100) {
      OLED_DIN(HIGH);
    }
    else  {
      OLED_DIN(LOW);
    }
    OLED_SCK(HIGH);
    hwData <<= 1;
  }
  
#endif
  
  OLED_CS(HIGH);
  
}

/*
void Write_Datas(uint8_t* dat_p, uint16_t length) {
  
  OLED_CS(LOW);
  
#if INTERFACE_4WIRE_SPI
  
  uint8_t data_read;

  OLED_DC(HIGH);
  
  data_read = wiringPiSPIDataRW(0,dat_p,length);

  if(data_read < 0)
    perror("WiringSPIDataRW error!\r\n");
  
#elif INTERFACE_3WIRE_SPI
  
  uint8_t i,j;
  uint16_t hwData = 0;
  
  for(i = 0; i < length; i++) {
  
    hwData = (uint16_t)dat_p[i] | 0x0100;
    
    for(j = 0; j < 9; j ++) {
      OLED_SCK(LOW);
      if(hwData & 0x0100) {
        OLED_DIN(HIGH);
      }
      else {
        OLED_DIN(LOW);
      }
      OLED_SCK(HIGH);
      hwData <<= 1;
    }
  }
#endif
  
  OLED_CS(HIGH);
  
}
*/

void RAM_Address(void)  {
  
  Write_Command(0x15);
  Write_Data(0x00);
  Write_Data(0x7f);

  Write_Command(0x75);
  Write_Data(0x00);
  Write_Data(0x7f);
}


void Clear_Screen(void)  {
  
  int i,j;
  
  uint8_t clear_byte[] = {0x00, 0x00};
  RAM_Address();
  Write_Command(0x5C);
  for(i=0;i<128;i++)  {
    for(j=0;j<128;j++)  {
      Write_Data(clear_byte[0]);//RAM data clear
      Write_Data(clear_byte[1]);
    }
  }
}
  

void Fill_Color(uint16_t color)  {
  
  int i,j;
  RAM_Address();
  Write_Command(0x5C);
  Set_Color(color);
  for(i = 0; i < 128; i++)  {
    for(j = 0; j < 128; j++)  {
      Write_Data(color_byte[0]);
      Write_Data(color_byte[1]);
    }
  }
}


void Set_Coordinate(uint16_t x, uint16_t y)  {

  if((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) 
    return;
  //Set x and y coordinate
  Write_Command(SSD1351_CMD_SETCOLUMN);
  Write_Data(x);
  Write_Data(SSD1351_WIDTH-1);
  Write_Command(SSD1351_CMD_SETROW);
  Write_Data(y);
  Write_Data(SSD1351_HEIGHT-1);
  Write_Command(SSD1351_CMD_WRITERAM);
}


void Set_Address(uint8_t column, uint8_t row)  {
  
  Write_Command(SSD1351_CMD_SETCOLUMN);  
  Write_Data(column);	//X start 
  Write_Data(column);	//X end 
  Write_Command(SSD1351_CMD_SETROW); 
  Write_Data(row);	//Y start 
  Write_Data(row+7);	//Y end 
  Write_Command(SSD1351_CMD_WRITERAM); 
}


void Write_text(uint8_t dat) {
  
  uint8_t i;
  
  for(i=0;i<8;i++)  {
    if (dat & 0x01) {
      Write_Data(color_byte[0]);
      Write_Data(color_byte[1]);
    }
    else  {
      Write_Data(0x00);
      Write_Data(0x00);
    }
    dat >>= 1;
  }
}


void Invert(uint8_t v) {
  
  if (v)
    Write_Command(SSD1351_CMD_INVERTDISPLAY);
  else
    Write_Command(SSD1351_CMD_NORMALDISPLAY);
}


void Draw_Pixel(int16_t x, int16_t y)
{
  // Bounds check.
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
  if ((x < 0) || (y < 0)) return;
  
  Set_Address(x, y);
  
  // transfer data
  Write_Data(color_byte[0]);
  Write_Data(color_byte[1]);  
}
  
int Device_Init(void) {
  
  if(wiringPiSetupGpio() < 0) { //use BCM2835 Pin number table
    return 1;
  }
  
  //GPIO config
  pinMode(26, OUTPUT);
  pinMode(oled_rst, OUTPUT);
  pinMode(oled_dc,  OUTPUT);
  pinMode(oled_cs,  OUTPUT);
  

  OLED_RST(HIGH);
  OLED_DC(HIGH);
  OLED_CS(HIGH);
  
#if INTERFACE_3WIRE_SPI
  
  pinMode(oled_sck, OUTPUT);
  pinMode(oled_din, OUTPUT);
  
  OLED_DC(LOW);
  OLED_SCK(HIGH);
  OLED_DIN(HIGH);

#elif INTERFACE_4WIRE_SPI
  
  wiringPiSPISetup(0, 9000000);
  
#endif
  
  OLED_CS(LOW);
  
  OLED_RST(LOW);
  delay(500);
  OLED_RST(HIGH);
  delay(500);
  
  Write_Command(0xfd);	// command lock
  Write_Data(0x12);
  Write_Command(0xfd);	// command lock
  Write_Data(0xB1);
  
  Write_Command(0xae);	// display off
  Write_Command(0xa4); 	// Normal Display mode
  
  Write_Command(0x15);	//set column address
  Write_Data(0x00);     //column address start 00
  Write_Data(0x7f);     //column address end 95
  Write_Command(0x75);	//set row address
  Write_Data(0x00);     //row address start 00
  Write_Data(0x7f);     //row address end 63	
  
  Write_Command(0xB3);
  Write_Data(0xF1);
  
  Write_Command(0xCA);	
  Write_Data(0x7F);
  
  Write_Command(0xa0);  //set re-map & data format
  Write_Data(0x74);     //Horizontal address increment
  
  Write_Command(0xa1);  //set display start line
  Write_Data(0x00);     //start 00 line
  
  Write_Command(0xa2);  //set display offset
  Write_Data(0x00);
  
  Write_Command(0xAB);	
  Write_Command(0x01);	
  
  Write_Command(0xB4);	
  Write_Data(0xA0);	  
  Write_Data(0xB5);  
  Write_Data(0x55);    
  
  Write_Command(0xC1);	
  Write_Data(0xC8);	
  Write_Data(0x80);
  Write_Data(0xC0);
  
  Write_Command(0xC7);	
  Write_Data(0x0F);
  
  Write_Command(0xB1);	
  Write_Data(0x32);
  
  Write_Command(0xB2);	
  Write_Data(0xA4);
  Write_Data(0x00);
  Write_Data(0x00);
  
  Write_Command(0xBB);	
  Write_Data(0x17);
  
  Write_Command(0xB6);
  Write_Data(0x01);
  
  Write_Command(0xBE);
  Write_Data(0x05);
  
  Write_Command(0xA6);
  
  Clear_Screen();
  Write_Command(0xaf);	 //display on
  
  return 0;
}

  
// Draw a horizontal line ignoring any screen rotation.
void Draw_FastHLine(int16_t x, int16_t y, int16_t length) {
  // Bounds check
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
    return;
  // X bounds check
  if (x+length > SSD1351_WIDTH) {
    length = SSD1351_WIDTH - x - 1;
  }
  if (length < 0)
    return;
  // set location
  Write_Command(SSD1351_CMD_SETCOLUMN);
  Write_Data(x);
  Write_Data(x+length-1);
  Write_Command(SSD1351_CMD_SETROW);
  Write_Data(y);
  Write_Data(y);
  // fill!
  Write_Command(SSD1351_CMD_WRITERAM);

  for (uint16_t i=0; i < length; i++)
  {
    Write_Data(color_byte[0]);
    Write_Data(color_byte[1]);
  }
}
  
  // Draw a vertical line ignoring any screen rotation.
void Draw_FastVLine(int16_t x, int16_t y, int16_t length)  {
  // Bounds check
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
    return;
  // X bounds check
  if (y+length > SSD1351_HEIGHT) {
    length = SSD1351_HEIGHT - y - 1;
  }
  if (length < 0)
    return;

  // set location
  Write_Command(SSD1351_CMD_SETCOLUMN);
  Write_Data(x);
  Write_Data(x);
  Write_Command(SSD1351_CMD_SETROW);
  Write_Data(y);
  Write_Data(y+length-1);
  // fill!
  Write_Command(SSD1351_CMD_WRITERAM);  
    
  for (uint16_t i=0; i < length; i++)
  {
    Write_Data(color_byte[0]);
    Write_Data(color_byte[1]);
  }
}



