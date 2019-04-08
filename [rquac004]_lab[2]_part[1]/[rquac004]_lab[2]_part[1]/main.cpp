/*
 * [rquac004]_lab[2]_part[1].cpp
 *
 * Created: 4/4/2019 12:48:03 PM
 * Author : mrvec
 */
/* 
#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs
					// Initialize output on PORTB to 0x00
	unsigned char temp_input = 0x00;
	while(1)
	{
		temp_input = PINA;
		PORTB = temp_input;	// Writes port B's 8 pins with the values
					// on port A's 8 pins
	}
}

*/

#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned char hold1 = 0x00; // Input
	unsigned char hold2 = 0x00; // Input
	//unsigned char out1 = 0x00; // Output
	while(1)
	{
		// 1) Read input
		hold1 = PINA & 0x01;
		hold2 = PINA & 0x02;
		
		// 2) Perform computation

		if ((hold1 == 0x01) && (hold2 == 0x00)) {
			PORTB = 0x01;
		} else {
			PORTB = 0x00;
		}

		//PORTB = out1;
	}
	return 0;
}
