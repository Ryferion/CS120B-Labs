/*
 * rquac004@ucr.edu
 * gpera002@ucr.edu
 * Lab Section: 028
 * Exercise Description: Count the number of 1s on ports A and B and output that number on port C. 
 * 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * Created: 4/9/2019 5:56:32 PM
 */ 

#include <avr/io.h>

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}	



int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned char check = 0x00;
	unsigned char count = 0x00;
	
	while(1){

		unsigned char i = 0x00;
		unsigned char a = 0x01;
		count = 0x00;
		for (i = 0; i < 7; i = i + 1){ //run through everything
			check = PINA & (a << i);
			if(GetBit(check, i)) { // see if the current bit is a 1
				count = count + 1;
			}
		}

		PORTC = count;
	}
}

