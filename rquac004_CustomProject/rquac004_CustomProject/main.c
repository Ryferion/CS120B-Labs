/*
 * rquac004_CustomProject.c
 *
 * Created: 5/22/2019 2:25:06 PM
 * Author : mrvec
 */ 

//**INCLUDES**//
#define F_CPU 20000000UL
#include <avr/io.h>
#include <stdio.h>
#include <avr/eeprom.h>

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "bit.h"


#define ipin PINA
#define iport PORTA

void ADC_Init();
int ADC_Read(char);

#define LCD_Data_Dir DDRC
#define LCD_Command_Dir DDRD
#define LCD_Data_Port PORTC
#define LCD_Command_Port PORTD
#define RS2 PD6		/* Define Register Select signal pin */
#define RW2 PD5			/* Define Read/Write signal pin */
#define EN2 PD7			/* Define Enable signal pin */

unsigned int score = 0; //
	
unsigned char pos = 0;
unsigned char switching = 0;
unsigned char length = 0; //if the length is greater than 7 = lose
unsigned char period = 75; //10 periods = 1 powerup spawns
unsigned char transitionTime = 0; // time it takes for the snake to go from top to bottom
unsigned char top = 0;
unsigned char iff = 1;
unsigned char able = 0;
unsigned char shift = 0;
unsigned char blah = 0;

char buffer[20];
unsigned char character = 0;


struct Node{
	
	char x;
	char y;
	//struct Node *after;
	
};

struct bEnemy{
	
	char x;
	char y;
	char type;
	
};

struct tEnemy{
	
	char x;
	char y;
	char type;
	
};

struct Node body[10];
struct tEnemy topEnemies[30];
struct bEnemy bottomEnemies[30];
unsigned char bCount;
unsigned char tCount;

void generateHead();
void generateBody();
//**TIMER**//

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
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

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
//**TIMER**//

//**LCD DISPLAY**//
 
void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port= cmnd;
	LCD_Command_Port &= ~(1<<RS2);	/* RS2=0 command reg. */
	LCD_Command_Port &= ~(1<<RW2);	/* RW=0 Write operation */
	LCD_Command_Port |= (1<<EN2);	/* Enable pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN2);
	_delay_ms(2);
}

void LCD_Char (unsigned char char_data)  /* LCD data write function */
{
	LCD_Data_Port= char_data;
	LCD_Command_Port |= (1<<RS2);	/* RS2=1 Data reg. */
	LCD_Command_Port &= ~(1<<RW2);	/* RW=0 write operation */
	LCD_Command_Port |= (1<<EN2);	/* Enable Pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN2);
	_delay_ms(2);			/* Data write delay */
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Command_Dir = 0xFF;		/* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;		/* Make LCD data port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_Command (0x38);		/* Initialization of 16X2 LCD in 8bit mode */
	LCD_Command (0x0C);		/* Display ON Cursor OFF */
	LCD_Command (0x06);		/* Auto Increment cursor */
	LCD_Command (0x01);		/* clear display */
	_delay_ms(2);			/* Clear display command delay> 1.63 ms */
	//LCD_Command (0x80);		/* Cursor at home position */
}


void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)  /* Send string to LCD with xy position */
{
	if (row == 0 && pos<20)
	LCD_Command(pos|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<20)
	LCD_Command(pos|0xC0);	/* Command of first row and required position<16 */
	else if (row == 2 && pos<20)
	LCD_Command(0x94 + pos);
	else if (row == 3 && pos<20)
	LCD_Command(0xD4 + pos);
	LCD_String(str);		/* Call LCD string function */
}

void LCD_ClearTop()
{
	LCD_Command (0x01);		/* clear display */
	LCD_Command (0x80 + shift);
}

void LCD_ClearBottom()
{
	LCD_Command (0x01);		/* clear display */
	LCD_Command (0xD4 + shift);
	
}

void LCD_Clear()
{
	if (pos == 0){
		LCD_ClearTop();
		} else {
		LCD_ClearBottom();
	}
}


 

void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
	unsigned char i;
	if(loc<8)
	{
		LCD_Command (0x40 + (loc*8));	/* Command 0x40 and onwards forces the device to point CGRAM address */
		for(i=0;i<8;i++)	/* Write 8 byte for generation of 1 character */
		LCD_Char(msg[i]);
	}
}


void LCD_Custom_Pos (char row, char pos, unsigned char str)  /* Send string to LCD with xy position */
{
	
	if (row == 0 && pos<20)
	LCD_Command(pos|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<20)
	LCD_Command((pos)|0xC0);	/* Command of first row and required position<16 */
	else if (row == 2 && pos<20)
	LCD_Command(0x94 + pos);
	else if (row == 3 && pos<20)
	LCD_Command(0xD4 + pos);
	LCD_Char(str);		/* Call LCD string function */
}


void test(){

}

//**LCD DISPLAY**//


//**CHARACTER**//
void initializeChar(){
	unsigned char Character1[8] = {
		0b00000,	0b00000,	0b00000,	0b00000,	0b00000,	0b00000,	0b00000,	0b00000
	};
	unsigned char Character2[8] = {
		0b00000,	0b00000,	0b11111,	0b11111,	0b11111,	0b00000,	0b00000,	0b00000
	};
	unsigned char Character3[8] = {
		0b00000,	0b11111,	0b11111,	0b11111,	0b00000,	0b00000,	0b00000,	0b00000
	};
	unsigned char Character4[8] = {
		0b00000,	0b00000,	0b11111,	0b11111,	0b11111,	0b00000,	0b00000,	0b00000
	};
	unsigned char Character5[8] = {
		0b00000,	0b00000,	0b00000,	0b11111,	0b11111,	0b11111,	0b00000,	0b00000
	};
	unsigned char Character6[8] = {
		0b00000,	0b00000,	0b00000,	0b00000,	0b11111,	0b11111,	0b11111,	0b00000
	};
	if (character == 0){
	LCD_Custom_Char(1, Character2);  /* Build Character1 at position 0 */
	} else if (character == 1){
	LCD_Custom_Char(1, Character3);  /* Build Character2 at position 1 */
	} else if (character == 2){
	LCD_Custom_Char(1, Character4);  /* Build Character3 at position 2 */
	}
	LCD_Custom_Char(0, Character1);
	LCD_Custom_Char(4, Character5);  /* Build Character5 at position 4 */
	LCD_Custom_Char(5, Character6);  /* Build Character5 at position 4 */

}
/*
void initializeBody(){
	unsigned char Bod1[8] = {
		0b00000,	0b00000,	0b00000,	0b11110,	0b11110,	0b11110,	0b00000,	0b00000
	};
	unsigned char Bod2[8] = {
		0b00000,	0b00000,	0b11110,	0b11110,	0b11110,	0b00000,	0b00000,	0b00000
	};
	unsigned char Bod3[8] = {
		0b00000,	0b11110,	0b11110,	0b11110,	0b00000,	0b00000,	0b00000,	0b00000
	};
	unsigned char Bod4[8] = {
		0b00000,	0b00000,	0b11110,	0b11110,	0b11110,	0b00000,	0b00000,	0b00000
	};
	unsigned char Bod5[8] = {
		0b00000,	0b00000,	0b00000,	0b11110,	0b11110,	0b11110,	0b00000,	0b00000
	};
	LCD_Custom_Char(1, Bod1);
	LCD_Custom_Char(2, Bod2);
	LCD_Custom_Char(3, Bod3);
	LCD_Custom_Char(4, Bod4);
	LCD_Custom_Char(5, Bod5);
}
*/

void initializeFall(){
	unsigned char Fall1[8] = {
		0b01000,		0b11011,		0b11111,		0b11111,		0b11111,		0b00000,		0b00000,		0b00000
	};
	unsigned char Fall2[8] = {
		0b00100,		0b00101,		0b01101,		0b11111,		0b11111,		0b11111,		0b00000,		0b00000
	};
	unsigned char Fall3[8] = {
		0b00000,		0b01000,		0b01110,		0b11110,		0b11111,		0b11111,		0b10111,		0b00000
	};
	unsigned char Fall4[8] = {
		0b00000,		0b01000,		0b10010,		0b10111,		0b10110,		0b11111,		0b11111,		0b11111
	};

	LCD_Custom_Char(1, Fall1);
	LCD_Custom_Char(2, Fall2);
	LCD_Custom_Char(3, Fall3);
	LCD_Custom_Char(4, Fall4);

}

void initializeRise(){
	unsigned char Rise1[8] = {
		0b11111,		0b10111,		0b01101,		0b01000,		0b01001,		0b00000,		0b00000,		0b00000
	};
	unsigned char Rise2[8] = {
		0b00001,		0b11111,		0b01111,		0b01101,		0b01101,		0b00100,		0b00000,		0b00000
	};
	unsigned char Rise3[8] = {
		0b00000,		0b00000,		0b11011,		0b11111,		0b01110,		0b01100,		0b01000,		0b00000
	};
	unsigned char Rise4[8] = {
		0b00000,		0b00000,		0b00100,		0b01100,		0b11110,		0b11110,		0b11111,		0b01110
	};
	LCD_Custom_Char(1, Rise1);
	LCD_Custom_Char(2, Rise2);
	LCD_Custom_Char(3, Rise3);
	LCD_Custom_Char(4, Rise4);

}
//**CHARACTER**//




//header

void obstacleSpawnDecider();//obstacle spawn decider
//has to be greater than transition time
//obstacle top has spawned wait transition time before spawning obstacle bottom
//else wait transition time spawn obstacle top
void obstacleSpawnTop();
//obstacle spawn top
//rand() % 3 height can be only between half and 1 pixel tall
//rand() delay time, has to be greater than transition time
void obstacleSpawnBottom();
//obstacle spawn bottom
//same thing as spawn top but located at the bottom
//score
void pause();
//pause
//freeze everything and bring up text asking to continue
void goUp();
//go up
void goDown();
//go down
void transitioncheck();
//move?
void menuScreen(unsigned char level);
//menu screen
//lets user pick difficulty, easy medium hard
//easy = period is default
//medium = transition time is 1.5x as fast and period is multiplied by 1.5
//hard = transition time is 2x as fast and period is multiplied by 2
void losingScreen();
//lose screen
void loadGameStage();
//game stage
void difficultyIncrease(); // over time difficulty cranks up

void advanceTop();
void advanceBottom();
void collideCheck();
//end header

void obstacleSpawnDecider(){
	unsigned char pick = rand() % 50;
	if ((pick % 2) == 0){
		if (top == 0){
		obstacleSpawnTop();
		tCount = tCount + 1;
		top = 1;
		} else {
			obstacleSpawnBottom();
			bCount = bCount + 1;
			top = 0;
		}
	} else {
		if (top == 1){
		obstacleSpawnBottom();
		bCount = bCount + 1;
		top = 0;
		}
		else {
			obstacleSpawnTop();
			tCount = tCount + 1;
			top = 1;
		}
	}
}

void obstacleSpawnTop(){
	topEnemies[tCount].x = 19;
	topEnemies[tCount].y = 0;
	LCD_String_xy(topEnemies[tCount].y, topEnemies[tCount].x, "t");
}

void obstacleSpawnBottom(){
	bottomEnemies[bCount].x = 19;
	bottomEnemies[bCount].y = 3;
	LCD_String_xy(bottomEnemies[bCount].y, bottomEnemies[bCount].x, "b");
}

void pause(){
	
}

void goDown(){
	if (switching == 1){
		pos = 1;
		TimerSet(10);
		//unsigned char h = 0;
		for (unsigned char h = 0; h < length; h = h + 1){
			for (unsigned char lel = 0; lel < 4; lel = lel + 1){
				//body[h].y = lel;
				//for (h = 0; h < length; h = h + 1){
				body[h].y = lel;
				LCD_Custom_Pos(body[h].y, body[h].x, 1);
			}
							while(!TimerFlag);
							TimerFlag = 0;
			for (unsigned char lel = 0; lel < 4; lel = lel + 1){
				//body[h].y = lel;
				//for (h = 0; h < length; h = h + 1){
				body[h].y = lel;
				LCD_Custom_Pos(body[h].y, body[h].x, 0);
			}


				/*
							if (body[h].y - 1 == 3){
								for (unsigned char u = 0; u < length; u = u + 1){
									LCD_Custom_Pos(body[u].y, body[u].x, 1);
								}
							}
			if (body[h].y > 0){
			LCD_String_xy(body[h].y-1, body[h].x, '\0');
			while(!TimerFlag);
			TimerFlag = 0;
			LCD_String_xy(body[h].y-1, body[h].x-1, '\0');
			while(!TimerFlag);
			TimerFlag = 0;
			LCD_String_xy(body[h].y-1, body[h].x-2, '\0');
			while(!TimerFlag);
			TimerFlag = 0;
			LCD_String_xy(body[h].y-1, body[h].x-3, '\0');
			}
			while(!TimerFlag);
			TimerFlag = 0;
							if (body[h].y == 3){
								for (unsigned char u = 0; u < length; u = u + 1){
									LCD_Custom_Pos(body[u].y, body[u].x, 1);
								}
							}
							if (body[h].y - 1 == 3){
								for (unsigned char u = 0; u < length; u = u + 1){
									LCD_Custom_Pos(body[u].y, body[u].x, 1);
								}
							}
							*/
			//LCD_Clear();
		}
		//LCD_Clear();
						advanceTop();
						advanceBottom();
	}
}

void goUp(){
	if (switching == 1){
		pos = 0;
		TimerSet(10);
		//unsigned char h = 0;
		for (unsigned char h = 0; h < length; h = h + 1){
			for (unsigned char lel = 4; lel > 0; lel = lel - 1){
				//body[h].y = lel;
				//for (h = 0; h < length; h = h + 1){
					body[h].y = lel - 1;
					LCD_Custom_Pos(body[h].y, body[h].x, 1);
				}
								while(!TimerFlag);
								TimerFlag = 0;
			for (unsigned char lel = 4; lel > 0; lel = lel - 1){
				//body[h].y = lel;
				//for (h = 0; h < length; h = h + 1){
				body[h].y = lel - 1;
				LCD_Custom_Pos(body[h].y, body[h].x, 0);
			}

			/*
							if (body[h].y - 1 == 0){
								for (unsigned char u = 0; u < length; u = u + 1){
									LCD_Custom_Pos(body[u].y, body[u].x, 1);
								}
							}
							
			if (body[h].y > 0){
			LCD_String_xy(body[h].y-1, body[h].x, '\0');
			while(!TimerFlag);
			TimerFlag = 0;
			LCD_String_xy(body[h].y-1, body[h].x-1, '\0');
			while(!TimerFlag);
			TimerFlag = 0;
			LCD_String_xy(body[h].y-1, body[h].x-2, '\0');
			while(!TimerFlag);
			TimerFlag = 0;
			LCD_String_xy(body[h].y-1, body[h].x-3, '\0');
			}
			while(!TimerFlag);
			TimerFlag = 0;
							if (body[h].y == 0){
								for (unsigned char u = 0; u < length; u = u + 1){
									LCD_Custom_Pos(body[u].y, body[u].x, 1);
								}
							}
							if (body[h].y - 1 == 0){
								for (unsigned char u = 0; u < length; u = u + 1){
									LCD_Custom_Pos(body[u].y, body[u].x, 1);
								}
							}
							*/
			//LCD_Clear();
		}
		//LCD_Clear();
						advanceTop();
						advanceBottom();
	}
}

void transitioncheck(){
//caase statementz
	int ADC_Value;
	ADC_Value = ADC_Read(1);
	
	if (~PINA & 0x08){
		losingScreen();
		able = 0;
	}
	
	if (~PINA & 0x64){
		able = 2;
	}
	if (ADC_Value > 120){
		able = 3;
	}
	if (ADC_Value < 90){
		able = 4;
	}
	if (shift <= 0){
		losingScreen();
	}

	if (able == 3){
		if (iff == 1){
			switching = 1;
			goDown();
			switching = 0;
			iff = 0;
			able = 0;
		} 
		able = 0;
	}
	if (able == 4){
		{
			switching = 1;
			goUp();
			switching = 0;
			iff = 1;
			able = 0;
		}
	} 
	
	if (able == 2){
		/*
		TimerSet(1000000000);
		while(!TimerFlag){
			LCD_String_xy(2, 6, "Paused");
			//if (~PINA & 0x64){
			//	break;
			//}
		}
		*/
		//TimerSet(period);
		//able = 0;
	}
	
}

void menuScreen(unsigned char level){
	if (level == 1){
		period = period;
	} else if (level == 2){
		period = period * 1.5;
		transitionTime = transitionTime * 1.5;
	} else if (level == 3){
		period = period * 2;
		transitionTime = transitionTime * 1.5;
	}
}

void losingScreen(){
	shift = 0;
	LCD_Command(0x01);
	LCD_Clear();
	LCD_String_xy(0, 0, "You lose!");
	sprintf(buffer, "Score: %d   ", score);
	LCD_String_xy(2, 0, buffer);
	eeprom_write_byte(0, score);
	unsigned char high;
	high = eeprom_read_byte(0);
	sprintf(buffer, "High Score: %d", high);
	LCD_String_xy(3, 0, buffer);
	TimerSet(1000000000);
	while (TimerFlag){
		if (~PINA & 0x08){
			main();
		}
	}
}

void loadGameStage(){
	
}

void difficultyIncrease(){
	if (period > 35){
	period = period - 5;
	//period = (period * 1.0001) ^ (period / 250); // ****CHANGE THIS****
	}
	else {
		period = period;
	}
}

void advanceTop(){
	unsigned char decide = rand() % 1;
	initializeChar();

	for (unsigned char hah = 0; hah < tCount; hah = hah + 1){
		topEnemies[hah].x = topEnemies[hah].x - 1;
		LCD_String_xy(topEnemies[hah].y, topEnemies[hah].x, "t");
		if (topEnemies[hah].x < 19){
		LCD_Custom_Pos(topEnemies[hah].y, topEnemies[hah].x + 1 + decide, 0);
		}
		if ((topEnemies[hah].x == 0) || (topEnemies[hah].x + 1 == 1)){
			LCD_Custom_Pos(topEnemies[hah].y, topEnemies[hah].x, 0);
			//topEnemies[hah].x = 19;
			//tCount = tCount - 1;
		}
	}
	
}

void advanceBottom(){
	unsigned char decide = rand() % 1;
	initializeChar();

	for (unsigned char hah = 0; hah < bCount; hah = hah + 1){
		bottomEnemies[hah].x = bottomEnemies[hah].x - 1;
		LCD_String_xy(bottomEnemies[hah].y, bottomEnemies[hah].x, "b");
		if (bottomEnemies[hah].x < 19){
		LCD_Custom_Pos(bottomEnemies[hah].y, bottomEnemies[hah].x + 1 + decide, 0);
		}
		if ((bottomEnemies[hah].x == 0) || (bottomEnemies[hah].x + 1 == 1)){
			LCD_Custom_Pos(bottomEnemies[hah].y, bottomEnemies[hah].x, 0);
			//bottomEnemies[hah].x = 19;
			//bCount = bCount - 1;
		}
	}
	
}
//**JOYSTICK**//


void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);

	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with ? will always keep the value
	// of ??etween 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing

	// start single convertion
	// write ? to ADSC
	ADCSRA |= (1<<ADSC);

	// wait for conversion to complete
	// ADSC becomes ? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));

	return (ADC);
}


void ADC_Init()
{
	DDRA = 0x00;		/* Make ADC port as input */
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADMUX = 0x40;		/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x07);/* set input channel to read */
	ADCSRA |= (1<<ADSC);	/* start conversion */
	while((ADCSRA &(1<<ADIF))== 0);	/* monitor end of conversion interrupt flag */
	
	ADCSRA |= (1<<ADIF);	/* clear interrupt flag */
	ADC_value = (int)ADCL;	/* read lower byte */
	ADC_value = ADC_value + (int)ADCH*256;/* read higher 2 bits, Multiply with weightage */

	return ADC_value;		/* return digital value */
}

//**JOYSTICK**//

//**SNAKE BODY**//
//Node body[10];

/*
struct Node{
	
	int x;
	int y;
	struct Node *after;
	
}; */

void generateHead(){
	
	body[length].x = shift;
	body[length].y = 0;
	
};

void generateBody(){
	if (iff == 1){
	//body[index].after = spot;
	body[length].x = shift;
	body[length].y = 0;
	} else {
	body[length].x = shift;
	body[length].y = 3;
	}
	
};

void clearData(){
	
	score = 0; //
	
	pos = 0;
	switching = 0;
	length = 0; //if the length is greater than 7 = lose
	period = 75; //10 periods = 1 powerup spawns
	transitionTime = 0; // time it takes for the snake to go from top to bottom
	top = 0;
	iff = 1;
	able = 0;
	shift = 0;
	blah = 0;
	bCount = 0;
	tCount = 0;
	//memset(buffer, 0, sizeof(buffer));
	memset(topEnemies, 0, sizeof(topEnemies));
	memset(bottomEnemies, 0, sizeof(bottomEnemies));
	character = 0;
	
	DDRA = 0xFF; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_Init();
	LCD_Clear();
	
	TimerSet(1000);
	TimerOn();
}

void collideCheck(){
	for (unsigned char hah = 0; hah < bCount; hah = hah + 1){
		if ((body[blah].y == bottomEnemies[hah].y) && (body[blah].x == bottomEnemies[hah].x)){
			score = score - 1;
			shift = shift - 1;
			generateBody();
			length = length + 1;
			bottomEnemies[hah].x = 1;
			LCD_Custom_Pos(bottomEnemies[hah].y, bottomEnemies[hah].x, 0);
		}
	}
	
	for (unsigned char hah = 0; hah < tCount; hah = hah + 1){
		if ((body[blah].y == topEnemies[hah].y) && (body[blah].x == topEnemies[hah].x)){
			score = score - 1;
			shift = shift - 1;
			generateBody();
			length = length + 1;
			topEnemies[hah].x = 1;
			LCD_Custom_Pos(topEnemies[hah].y, topEnemies[hah].x, 0);
		}
	}
}
//**SNAKE BODY**//


int main()
{
	clearData();
	
	shift = 0;	
	LCD_String("tis a game i think"); //hi there how's life?
	shift = 9;
	while(!TimerFlag);
	TimerFlag = 0;
	LCD_Clear();

	initializeChar();
	generateHead();
	LCD_Custom_Pos(body[0].y, body[0].x, 1);
	length = length + 1;
	unsigned char temp = 0;
	TimerSet(period);
	
	while(1){
		TimerSet(period);
		ADC_Init();		/* Initialize ADC */
		
		
		for (blah = 0; blah < length; blah = blah + 1){
			transitioncheck();
			initializeChar();
			LCD_Custom_Pos(body[blah].y, body[blah].x, 1);
			collideCheck();
						while(!TimerFlag);
						TimerFlag = 0;
			//LCD_String_xy(body[h].y, body[h].x, '\0');
		}
		TimerSet(period);
			transitioncheck();
			advanceBottom();
			transitioncheck();
			advanceTop();
			transitioncheck();
			while(!TimerFlag);
			TimerFlag = 0;
			temp = temp + 1;
			transitioncheck();
			score = score + 1;
			if (temp % 5 == 0){
				obstacleSpawnDecider();
			}
			if (temp % 2){
				score = score + 1;
				difficultyIncrease();
			}
			transitioncheck();
			
	}
}
