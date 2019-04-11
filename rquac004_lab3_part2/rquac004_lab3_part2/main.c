/*
 * rquac004@ucr.edu
 * gpera002@ucr.edu
 * Lab Section: 028
 * Exercise Description: 
 * 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * Created: 4/9/2019 7:47:30 PM
 * Author : mrvec
 */ 

#include <avr/io.h>

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // config
	DDRB = 0xFF; PORTB = 0x00; // config
	DDRC = 0xFF; PORTC = 0x00; // config
	
	unsigned char hold1 = 0x00;
	unsigned char hold2 = 0x00;
	unsigned char hold3 = 0x00;
	unsigned char hold4 = 0x00;
	unsigned char level = 0x00;
	unsigned char output = 0x00;
    while (1) 
    {
		hold1 = PINA & 0x01;
		hold2 = PINA & 0x02;
		hold3 = PINA & 0x04;
		hold4 = PINA & 0x08;
		level = hold1 + hold2 + hold3 + hold4;
		if (level <= 0){
			output = 0;
		}
		else if ((level == 1) || (level == 2)){
			output = SetBit(output, 5, 1);
		}
		else if ((level == 3) || (level == 4)){
			output = SetBit(output, 5, 1);
			output = SetBit(output, 4, 1);
		}
		else if ((level == 5) || (level == 6)){
			for (unsigned char i = 5; i >= 3; i = i - 1){
				output = SetBit(output, i, 1);
			}
		}
		else if ((level >= 7) && (level <= 9)){
			for (unsigned char i = 5; i >= 2; i = i - 1){
				output = SetBit(output, i, 1);
			}
		}
		else if ((level >= 10) && (level <= 12)){
			for (unsigned char i = 5; i >= 1; i = i - 1){
				output = SetBit(output, i, 1);
			}
		}
		else if ((level >= 13) && (level <=15)){
			for (unsigned char i = 5; i >= 0; i = i - 1){
				output = SetBit(output, i, 1);
			}
		}
		if (level <= 4){
			output = SetBit(output, 6, 1);
		}
		PORTC = output;
    }
}

