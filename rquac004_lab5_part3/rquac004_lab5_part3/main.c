/*
 * rquac004@ucr.edu
 * gpera002@ucr.edu
 * Lab Section: 028
 * Exercise Description: 
 * 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * Created: 4/10/2019 6:57:27 PM
 * Author : mrvec
 */ 


#include <avr/io.h>


enum States {
	Start, WaitPress, One, Two, Three
}state;

unsigned char hold = 0x00;
unsigned char count = 0x00;

void toggle() {
	switch (state){//transition
		case Start:
			count = 1;
			state = WaitPress;
			break;
		case WaitPress:
			while (hold == (PINA & 0x01)){
				if (count == 1){
					state = One;
				}
				if (count == 2){
					state = Two;
				}
				if (count == 3) {
					state = Three;
				}
			}
			break;
		case One:
			PORTC = 0xAA;
			count = count + 1;
			state = WaitPress;	
			break;
		case Two:
			PORTC = 0x55;
			count = count + 1;
			state = WaitPress;
		break;
		case Three:
			PORTC = 0xC3;
			count = 1;
			state = WaitPress;
		break;
		default:
			state = WaitPress;
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // config
	DDRB = 0xFF; PORTB = 0x00; // config
	DDRC = 0xFF; PORTC = 0x00; // config
		count = 0;
		PORTC = 0;

	while (1) 
    {
		hold = ~PINA & 0x01;
		toggle();
    }
}

