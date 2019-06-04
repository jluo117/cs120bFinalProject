/*
 * TempReader.c
 *
 * Created: 5/23/2019 10:00:59 AM
 * Author : ucrcse
 */ 
//code source  :https://github.com/Andre-Castro/EE120BSpr17/tree/master/acast050_FinalProject (refrence to get started)
//Nokia Source https://github.com/LittleBuster/avr-nokia5110 (Nokia libary)

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nokia5110.c"

volatile int num = 100;
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.
volatile int timePast = 0;
// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

/**
 * Sending data to LCD
 * @bytes: data
 * @is_data: transfer mode: 1 - data; 0 - command;
 */
int targetNum;
enum joyStick {down,up,left,right,still} Joystate;
enum gameState{inGame,Win,init,Lose} state;
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

void A2D_init(){
	
	ADCSRA |= (1 >> ADEN) | (1 << ADSC) | (1 << ADATE);
}
void ADCInit(){
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1<< ADPS1) | (1 << ADPS0);
}
uint16_t adc_read(uint8_t ch){
	ch &= 0b00000111;
	ADMUX = (ADMUX & 0xF8)| ch;
	ADCSRA |= (1 <<ADSC);
	while(ADCSRA & (1 << ADSC));
	return (ADC);
}

void joystickState(){
	int16_t y_axies = adc_read(1);
	int16_t x_axis = adc_read(0);
	if(y_axies >= 1023){
		Joystate = down;
	}
	else if (y_axies <= 300){
		Joystate = up;
	}
	

	else{
		Joystate = still;
	}
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
void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001  s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
const unsigned short PERIOD = 250;
void writeValues(int myValue){
	char snum[5];
	itoa(myValue, snum, 10);
	nokia_lcd_clear();
    nokia_lcd_write_string("Bomb Planted",1);
    nokia_lcd_set_cursor(0, 10);
	nokia_lcd_write_string(snum,3);
    
	nokia_lcd_render();
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
void tick(){
	char unsigned D = 0;
	D = PIND;
	D = D & 0x01;
    switch (state) {
        case inGame:
            if (targetNum == num){
                state = Win;
            }
            if (timePast >= 175){
                state = Lose;
            }
            break;
        case Win:
        	if (D == 1){
        		state = init;
        	}
            break;
            //return to init state
        case Lose:
        	if (D== 1){
        		state = init;
        	}
            break;
        case init:
            state = inGame;
            break;
            // return to init state
        default:
            break;
    }
    switch (state){
        case inGame:
            joystickState();
            updateNum();
            writeValues(num);
            timePast++;
            break;
        case Win:
            nokia_lcd_clear();
            nokia_lcd_write_string("Bomb Defused",1);
            nokia_lcd_set_cursor(0, 10);
            nokia_lcd_write_string("CT Wins", 3);
            break;
        case Lose:
            nokia_lcd_clear();
            nokia_lcd_write_string("Target Destroy",1);
            nokia_lcd_set_cursor(0, 10);
            nokia_lcd_write_string("T Wins", 3);
            break;
        case init:
            timePast = 0;
            num = 100;
    }
    
}
int main(void)
{
	ADCInit();
	targetNum = rand() % (150 + 1 - 0) + 0;
	TimerSet(250);
	DDRA = 0x00; PORTA = 0xFF; //Input
	DDRB = 0x00; PORTB = 0xFF; //Ouptut
	DDRC = 0xFF; PORTC = 0x00; 
	DDRD = 0x00; PORTD = 0xFF;
	nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_write_string("IT'S WORKING!",1);
    nokia_lcd_set_cursor(0, 10);
    nokia_lcd_write_string("Nice!", 3);
	writeValues(100);
	state = inGame;
    /* Replace with your application code */
    while (1) 
    {
        tick();
		/* store next eight bit in CheckSum */
		//PORTB = I_Temp;
    }
}

