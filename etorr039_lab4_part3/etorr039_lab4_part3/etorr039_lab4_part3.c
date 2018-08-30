/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 4  Exercise 3 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define button (PINA & 0x01)
#define OUTPUT PORTC
#define q PORTB
#define k PORTD

enum States {start, wait, wait2, s1, s2, s3, s4, s5, s6, s7, s8,
			 s9, s10, s11, s12, s13, s14} state;
	
unsigned char i;
	
void lights() {
	switch(state) { // transitions
		case start:
			state = wait;
			break;

		case wait:
			if (button == 0 && k == 1) {
				state = wait2;
			}
			else if (button == 0 && k == 0) {
				state = q;
			}
			else {
				state = wait;
			}
			break;
			
		case wait2:
			if (button == 0 && k == 1) {
				state = wait2;
			}
			else if (button == 1 && k == 0) {
				state = wait;
				
			}
			break;

		case s1:
			state = wait;
			break;

		case s2:
			state = wait;
			break;

		case s3:
			state = wait;
			break;

		case s4:
			state = wait;
			break;

		case s5:
			state = wait;
			break;
			
		case s6:
			state = wait;
			break;

		case s7:
		state = wait;
			break;
			
		case s8:
		state = wait;
			break;

		case s9:
			state = wait;
			break;

		case s10:
			state = wait;
			break;

		case s11:
			state = wait;
			break;

		case s12:
			state = wait;
			break;
		
		case s13:
			state = wait;
			break;

		case s14:
			state = wait;
			break;
	}
	
	switch(state) { // actions
		case start:
			k = 0;
			q = 3;
			break;

		case wait:
			break;
			
		case wait2:
			k = 0;
			break;

		case s1:
			k = 1;
			q = 4;
			OUTPUT = 0x81;
			break;

		case s2:
			k = 1;
			q++;
			OUTPUT = 0x42;
			break;

		case s3:
			k = 1;
			q++;
			OUTPUT = 0x24;
			break;

		case s4:
			k = 1;
			q++;
			OUTPUT = 0x18;
			break;

		case s5:
			k = 1;
			q++;
			OUTPUT = 0x3C;
			break;

		case s6:
			k = 1;
			q++;
			OUTPUT = 0x7E;
			break;
			
		case s7:
			k = 1;
			q++;
			OUTPUT = 0xFF;
			break;

		case s8:
			k = 1;
			q++;
			OUTPUT = 0x7E;
			break;

		case s9:
			k = 1;
			q++;
			OUTPUT = 0x3C;
			break;

		case s10:
			k = 1;
			q++;
			OUTPUT = 0x18;
			break;

		case s11:
			k = 1;
			q++;
			OUTPUT = 0x00;
			break;

		case s12:
			k = 1;
			q++;
			OUTPUT = 0x18;
			break;
		
		case s13:
			k = 1;
			q++;
			OUTPUT = 0x24;
			break;

		case s14:
			k = 1;
			q = 3;
			OUTPUT = 0x42;
			break;
	}
}

int main(void) {
	
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	state = start; // initialize state
	
	while (1) {
		lights(); //call function
	}
	return 0;
}