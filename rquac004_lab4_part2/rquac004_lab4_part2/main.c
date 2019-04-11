/*
 * rquac004_lab4_part1.c
 *
 * Created: 4/10/2019 6:57:27 PM
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
	Start, WaitPress, PressOne, PressTwo, PressBoth
}state;

unsigned char hold = 0x00;
unsigned char hold1 = 0x00;
unsigned char count = 0x00;

void toggle() {
	switch (state){//transition
		case Start:
			state = WaitPress;
			break;
		case WaitPress:
			while (hold == (PINA & 0x01)){
				if ((hold != 0x00) && (hold1 != 0x00)){
					state = PressBoth;
				}
				if (hold != 0x00){
					state = PressOne;
				} else if (hold1 != 0x00) {
					state = PressTwo;
				}
				if ((hold != 0x00) && (hold1 != 0x00)){
					state = PressBoth;
				}
			}
			break;
		default:
			state = WaitPress;
			break;
	}
	
	switch (state){//actions
		case PressOne:
			if (count <= 9){
				count = count + 1;
				PORTC = count;
			}
			state = WaitPress;	
			break;
		case PressTwo:
			if (count >= 0){
				count = count - 1;
				PORTC = count;
			}
			state = WaitPress;
			break;
		case PressBoth:
			count = 0;
			PORTC = count;
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
		count = 7;
		PORTC = 7;
    while (1) 
    {
		hold = PINA & 0x01;
		hold1 = PINA & 0x02;

		toggle();
    }
}

