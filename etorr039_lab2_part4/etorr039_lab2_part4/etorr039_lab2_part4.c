
/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 2  Exercise 4 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure all A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure all B's 8 pins as outputs
	DDRC = 0xFF; PORTC = 0x00; // Configure all C's 8 pins as outputs
	
    while (1) 
    {
		unsigned char input = PINA;
		unsigned char i = 0x00;
		unsigned char tmp1 = 0x00;
		unsigned char tmpC = 0x00;
		unsigned char tmpB = 0x00;
		
		for (i = 0; i < 4; i++) {
			tmp1 = GetBit(input,i);
			
			if (tmp1 == 1) {
				tmpB = tmpB | SetBit (PINB,i,1);
			}
			else {
				tmpB = tmpB |  SetBit(PINB,i,0);
			}
		}
		
		for (i = 4; i < 8; i++) {
			tmp1 = GetBit(input,i);
			
			if (tmp1 == 1) {
				tmpC = tmpC | SetBit (PINC,i,1);
			}
			else {
				tmpC = tmpC | SetBit(PINC,i,0);
			}
		}
		// Output
		PORTB = tmpB;
		PORTC = tmpC;
    }
}

