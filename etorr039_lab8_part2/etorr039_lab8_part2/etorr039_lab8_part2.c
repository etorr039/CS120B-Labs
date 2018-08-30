/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 8  Exercise 2
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define ledB PORTB
#define ledD PORTD

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs
	DDRD = 0xFF; PORTD = 0x00;
	
	ADC_init();
	
	while(1) {
		unsigned short x = ADC;
	
		ledB = (char)x;
		ledD = (char)(x >> 8);
	}
	return 0;
}


