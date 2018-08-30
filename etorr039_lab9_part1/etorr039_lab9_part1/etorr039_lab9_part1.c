/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 9  Exercise 1
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define BUTTONS (~PINA & 0x07)

// 0.954 hz is lowest frequency possible with this function,
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
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					
		// 31250 is largest frequency that will not result in underflow
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

enum States {start, wait, C4, D4, E4} state;
	
void notes();

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; 
	DDRB = 0xFF; PORTB = 0x00; 
	
	while(1) {
		notes();
	}
	return 0;
}

void notes() {
	switch(state) {
		case start:
			state = wait;
			break;
		
		case wait:
			if (BUTTONS == 0x01) {
				state = C4;
				PWM_on();
				set_PWM(261.63);
			}
			else if (BUTTONS == 0x02) {
				state = D4;
				PWM_on();
				set_PWM(293.66);
			}
			else if (BUTTONS == 0x04) {
				state = E4;
				PWM_on();
				set_PWM(329.63);
			}
			else {
				state = wait;
			}
			break;
		
		case C4:
			if (BUTTONS == 0x01) {
				state = C4;
			}
			else {
				state = wait;
				PWM_off();
			}
			break;
		
		case D4:
			if (BUTTONS == 0x02) {
				state = D4;
			}
			else {
				state = wait;
				PWM_off();
			}
			break;
		
		case E4:
			if (BUTTONS == 0x04) {
				state = E4;
			}
			else {
				state = wait;
				PWM_off();
			}
			break;
	}
}
