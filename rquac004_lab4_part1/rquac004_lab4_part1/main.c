/*
 * rquac004_lab4_part1.c
 *
 * Created: 4/10/2019 5:30:22 PM
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
	Start, SwitchLight, SwitchLight2, WaitSwitch, WaitSwitch2
}state;

unsigned char hold = 0x00;

void toggle() {
	switch (state){//transition
		case Start:
			state = WaitSwitch;
			break;
		case WaitSwitch:
			while (hold == (PINA & 0x01)){
			if ((hold & 0x01) != 0x00){
				state = SwitchLight;
			} else {
				state = WaitSwitch;
			}
			}
			break;
		case WaitSwitch2:
			while (hold == (PINA & 0x01)){
			if ((hold & 0x01) != 0x00){
				state = SwitchLight2;
			} else {
				state = WaitSwitch2;
			}
			}
			break;
		default:
			state = WaitSwitch;
			break;
	}
	
	switch (state){//actions
		case SwitchLight:
			PORTB = 0x01;
			state = WaitSwitch2;
			break;
		case SwitchLight2:
			PORTB = 0x02;
			state = WaitSwitch;
			break;
		default:
			state = WaitSwitch;
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // config
	DDRB = 0xFF; PORTB = 0x00; // config

    while (1) 
    {
		hold = PINA & 0x01;
		toggle();
    }
}

