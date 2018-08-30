
/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 2  Exercise 1 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,

	while(1)
	{
		unsigned char num1tot = 0x00;	// Initialize total 1's to 0
		unsigned char num1A = 0x00;		// Initialize total PINA 1's to 0
		unsigned char num1B = 0x00;		// Initialize total PINB 1's to 0
		unsigned char i;				// define counter variable
		
		for(i = 0; i < 8; i++) { // Count number of 1's in PINA
			num1A = num1A + GetBit(PINA, i);
		}		
		for(i = 0; i < 8; i++) { // Count number of 1's in PINB
			num1B = num1B + GetBit(PINB, i);
		}
				
		num1tot = num1A + num1B; // Sum number of 1's
		
		//output number of 1's
		PORTC = num1tot;
	}
	return 0;
}




