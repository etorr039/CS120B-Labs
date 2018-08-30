/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 1  Exercise 3 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0x00; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs,
	// initialize to 0s

	while(1)
	{
		unsigned char tmpD = PINA & 0x08; // Temporary variable to hold the value of D
		unsigned char tmpC = PINA & 0x04; // Temporary variable to hold the value of C
		unsigned char tmpB = PINA & 0x02; // Temporary variable to hold the value of B
		unsigned char tmpA = PINA & 0x01; // Temporary variable to hold the value of A
		unsigned char cntavail = 0x00; // Available parking spaces
		
		if (!(tmpA == 0x01)) {
			cntavail++;
		}
		if (!(tmpB == 0x02)) {
			cntavail++;
		}
		if (!(tmpC == 0x04)) {
			cntavail++;
		}
		if (!(tmpD == 0x08)) {
			cntavail++;
		}
		if (cntavail == 0x00) {
			cntavail = 0x80;
		}
		//output
		PORTC = cntavail & 0x8F; // set xbbbxxxx
	}
	return 0;
}