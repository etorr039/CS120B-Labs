/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 8  Exercise 4
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define LED PORTB

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

enum States {start, meter} state;
	
void lightmeter(); 

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs
	
	ADC_init();
	
	while(1) {
		lightmeter();
	}
	return 0;
}

void lightmeter() {
	unsigned char x = ADC;
	
	switch(state) { // transitions
		case start:
		state = meter;
		break;
		
		case meter:
		if (x == 39) {
			LED = 0x00;
		}
		if (x == 44) {
			LED = 0x01;
		}
		if (x == 46) {
			LED = 0x03;
		}
		if (x == 48) {
			LED = 0x07;
		}
		if (x == 49) {
			LED = 0x0F;
		}
		if (x == 51) {
			LED = 0x1F;
		}
		if (x == 56) {
			LED = 0x3F;
		}
		break;
	}
}

