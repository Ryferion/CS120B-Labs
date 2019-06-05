#include <avr/io.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int main(void)
{
		DDRA = 0x00; PORTA = 0xFF; // input for A
		DDRC = 0xFF; PORTC = 0xFF; // output for C
		DDRD = 0xFF; PORTD = 0xFF;//output for D
		unsigned short hold = 0;
		unsigned short hold1 = 0;
		unsigned short MAX = PORTA;
		ADC_init();

    while (1) 
    {
			
		hold = (char)ADC;
		hold1 = (char)(ADC >> 8);
		
		if(ADC > MAX)
		MAX = ADC;
		PORTC = hold;
		PORTD = hold1;
		/*
		if(ADC >= MAX/2)
		{
			PORTC = 0xFF;
			PORTD = 0xFF;
		}
		else
		{
			PORTC = 0x00;
			PORTD = 0x00;
		}
		*/
		
	}
}

