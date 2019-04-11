/*
 * rquac004_lab4_part1.c
 *
 * Created: 4/11/2019 3:36:56 PM
 * Author : mrvec
 */ 

#include <avr/io.h>

/*
start
v
pb0 = 1 < pa0 = 0
v
button - pa0 = 1
v
pb0 = 0
pb1 = 1 < pa0 = 0
button - pa0 = 1
v
pb0 = 1 < pa0 = 0
pb1 = 0
*/

enum States {
	Start, WaitPress, PressX, PressY, PressHash, PressInside, Unlock, Lock
}state;

unsigned char hold = 0x00;
unsigned char hold1 = 0x00;
unsigned char hold2 = 0x00;
unsigned char hold3 = 0x00;
unsigned char count = 0x00;

void toggle() {
	switch (state){//transition
		case Start:
			state = WaitPress;
			break;
		case WaitPress:
			if ((hold != 0x00) || (hold1 != 0x00) || (hold2 != 0x00)){
				state = PressX;
			} else if (hold3 != 0x00){
				state = Lock;
			} else {
				state = WaitPress;
			}
			break;
		case PressX:
			while (hold == (PINA & 0x01)){
				if (hold != 0x00){
					count = 1;
					PORTC = count;
					state = PressY;
				} else if (hold3 != 0x00){
					state = Lock;
				}
			}
			break;
		case PressY:
			while (hold1 == (PINA & 0x02)){
				if (hold1 != 0x00){
					count = 2;
					PORTC = count;
					state = PressHash;
				} else if (hold3 != 0x00){
					state = Lock;
				}
			}
		break;
		case PressHash:
			while (hold2 == (PINA & 0x04)){
				if (hold2 != 0x00){
					count = 4;
					PORTC = count;
					state = Unlock;
				} else if (hold3 != 0x00){
					state = Lock;
				}
			}
		break;
		default:
			state = Start;
			break;
	}
	
	switch (state){//actions
		case Unlock:
			PORTB = 1;
			PORTC = 0;
			state = WaitPress;
			break;
		case Lock:
			count = 0;
			PORTB = 0;
			PORTC = 0;
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
		PORTB = 0;
    while (1) 
    {
		hold = PINA & 0x01;
		hold1 = PINA & 0x02;
		hold2 = PINA & 0x04;
		hold3 = PINA & 0x80;
		toggle();
    }
}

