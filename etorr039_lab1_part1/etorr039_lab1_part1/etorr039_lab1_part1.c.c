/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 1  Exercise 1
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpB = 0x00; 	// Temporary variable to hold the value of B 
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	while(1)
	{
		// set bbbbbbxx
		tmpA = PINA & 0x03; 
		// if light sensor 0 and garage door 1, turn on LED
		if (tmpA == 0x01) {
			tmpB = 0x01; 
		}
		// otherwise, LED is off
		else {
			tmpB = 0x00;
		}
		//output
		PORTB = tmpB;
	}
	return 0;
}


