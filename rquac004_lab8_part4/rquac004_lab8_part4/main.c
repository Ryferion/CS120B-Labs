/*
 * rquac004_lab8_part4.c
 *
 * Created: 5/2/2019 6:46:20 PM
 * Author : mrvec
 */ 
\
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
		DDRA = 0x00; PORTA = 0xFF;
		DDRC = 0xFF; PORTC = 0xFF;
		DDRD = 0xFF; PORTD = 0x00;
		unsigned short love1 = 0;
		unsigned short love2 = 0;
		ADC_init();
		unsigned short MAX = ADC;

    while (1) 
    {
		
		love1 = (char)ADC;
		love2 = (char)(ADC >> 8);
		
		if(ADC > MAX)
		{
			MAX = ADC;
		}
		
		
		if(ADC <= MAX/7)
		{
			PORTC = 0x40;
		}
		else if(ADC <= MAX/6)
		{
			PORTC = 0x20;
		}
		else if(ADC <= MAX/5)
		{
			PORTC = 0x10;
		}
		else if(ADC <= MAX/4)
		{
			PORTC = 0x08;
		}
		else if(ADC <= MAX/3)
		{
			PORTC = 0x04;
		}
		else if(ADC <= MAX/2)
		{
			PORTC = 0x02;
		}
		else
		{
			PORTC = 0x01;
		}
		
	}
}

