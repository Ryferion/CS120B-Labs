/*
 * rquac004_lab9_part1.c
 *
 * Created: 5/2/2019 7:04:25 PM
 * Author : mrvec
 */ 

#include <avr/io.h>
// 0.954 Hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = (int)0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64                    // 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
	unsigned char check = 0;
	
void tick(unsigned char count){
	if ( (check == 0) && !(~PINA & 0x07) ){
	if (count == 1){
		set_PWM(261.63);
	}
	if (count == 2){
		set_PWM(293.66);
	}
	if (count == 3){
		set_PWM(329.63);
	}
	if (count == 4){
		set_PWM(349.23);
	}
	if (count == 5){
		set_PWM(392.00);
	}
	if (count == 6){
		set_PWM(440.00);
	}
	if (count == 7){
		set_PWM(493.88);
	}
	if (count == 8){
		set_PWM(523.25);
	}
	}
}

int main(void)
{
	unsigned char counter = 0;
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	
    /* Replace with your application code */
		PWM_on();
		
    while (1) 
    {
		
		if(GetBit(~PINA, 0) && !GetBit(~PINA, 1) && !GetBit(~PINA, 2))
		{	
			if (check == 0){
				check = 1;
			} else {
				check = 0;
			}
		}
			if (check == 0){
				PWM_on();
				//check = 1;
			}
			if (check  == 1){
				PWM_off();
				//check = 0;
			}
			
		
		if (GetBit(~PINA, 1)  && !GetBit(~PINA, 0) && !GetBit(~PINA, 2))
		{	
			if (counter < 8){
				while(~PINA & 0x01){
					
				}
				counter = counter + 1;
				tick(counter);
			}
		}

		else if (GetBit(~PINA, 2) && !GetBit(~PINA, 1) && !GetBit(~PINA, 0))
		{
			if (counter > 1){
				while(~PINA & 0x02){
					
				}
				counter = counter - 1;
				tick(counter);
			}
		}
		
		//else if(!GetBit(~PINA, 0) && !GetBit(~PINA, 1) && !GetBit(~PINA, 2)){
		
		//}
		
		//PWM_off();
    }
}

