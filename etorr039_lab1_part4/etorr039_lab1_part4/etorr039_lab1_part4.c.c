/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 1  Exercise 4 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs,
	DDRC = 0x00; PORTC = 0xFF; // Configure port C's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs,

	while(1)
	{
		unsigned char weight1 = PINA;
		unsigned char weight2 = PINB;
		unsigned char weight3 = PINC;
		unsigned short totweight = 0x00; // initialize total weight to 0
		unsigned char maxweight = 0x8C; // maximum weight is 140
		unsigned char tmp = 0x00; // initialize temporary variable to 0
		
		totweight = weight1 + weight2 + weight3; // add up all the weights
		
		if (totweight > maxweight) { // is total weight greater than 140?
			tmp = 0x01;	// signal 1 to PD0
		}
		if (((weight1 - weight3) > 0x50) | ((weight3 - weight1) > 0x50)) {	// Is weight difference greater than 80?
			tmp = tmp + 0x02;	// signal 1 to PD1
		}	
		// leftmost 6 bits display total weight (000000xx)
		if (totweight < 63) {
			totweight = (totweight << 2);			
		}
		// leftmost 6 bits display total weight according to 000000xxx
		if (totweight > 62 && totweight < 127) {
			totweight = (totweight >> 1);
		}
		// leftmost 6 bits display total weight according to 000000xxxx
		if (totweight > 127 && totweight < 255) {
			totweight = (totweight >> 2);
		}
		// leftmost 6 bits display total weight according to 000000xxxxx
		if (totweight > 255) {
			totweight = (totweight >> 3);
		}
		PORTD = tmp | totweight; // Output
	}
	return 0;
}
