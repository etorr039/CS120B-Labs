/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 8  Exercise 3
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define LED PORTB

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

enum States {start, ledOn, ledOff} state;
	
void led(); 

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs
	
	ADC_init();
	
	while(1) {
		led();
	}
	return 0;
}

void led() {
	unsigned short x = ADC;
	unsigned short MAX = 56;
	unsigned short MIN = 39;
	unsigned short MAXhalf = (MAX + MIN)/2;
	
	switch(state) { // transitions
		case start:
		state = ledOn;
		break;
		
		case ledOn:
		if (x >= MAXhalf) {
			state = ledOn;
			LED = 1;
		}
		else {
			state = ledOff;
		}
		break;
		
		case ledOff:
		if (x < MAXhalf) {
			state = ledOff;
			LED = 0;
		}
		else {
			state = ledOn;
		}
		break;
	}
}


