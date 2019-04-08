/*
 * [rquac004]_lab[2]part[3].cpp
 *
 * Created: 4/7/2019 1:56:31 AM
 * Author : mrvec
 */ 
/*
 * [rquac004]_lab[2]_part[2].cpp
 *
 * Created: 4/5/2019 11:46:31 AM
 * Author : mrvec
 */ 
	
#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // config
	DDRB = 0xFF; PORTB = 0x00; // config
	DDRC = 0xFF; PORTC = 0x00; // config
	unsigned char hold1 = 0x00; // Input
	unsigned char hold2 = 0x00; // Input
	unsigned char hold3 = 0x00; // Input
	unsigned char hold4 = 0x00; // Input
	unsigned char cntavail = 0x00; // PORTC = cntavail;
	//unsigned char out1 = 0x00; // Output
	
	//hold = 1 means a car is parked in the space, there are four parking spaces
	while(1)
	{
		// reading
		hold1 = PINA & 0x01;
		hold2 = PINA & 0x02;
		hold3 = PINA & 0x04;
		hold4 = PINA & 0x08;
		cntavail = 0x00;
		
		if (hold1 != 0x00) {
			cntavail = cntavail + 1;
		} 
		if (hold2 != 0x00) {
			cntavail = cntavail + 1;
		}
		if (hold3 != 0x00) {
			cntavail = cntavail + 1;
		}
		if (hold4 != 0x00) {
			cntavail = cntavail + 1;
		}

		if ((hold1 != 0x00) && (hold2 != 0x00) && (hold3 != 0x00) && (hold4 != 0x00)) {
			cntavail = cntavail | 0x80; //full check
		}

			PORTC = 0x00;
			PORTC = cntavail;

	}
	return 0;
}

