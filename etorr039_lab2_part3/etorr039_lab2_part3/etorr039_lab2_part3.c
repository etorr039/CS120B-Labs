

/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 1  Exercise 3 
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
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,

	while(1)
	{
		unsigned char gaslvl = PINA & 0x0F;	// set bbbbxxxx
		unsigned char seatLight1 = PINA & 0x7F;	// set bxxxxxxx
		unsigned char indicator = 0x00;		// initialize indicator
		unsigned char keyIn = 0x00;			// initialize key in ignition
		unsigned char seated = 0x00;		// initialize person seated
		unsigned char seatbeltOn = 0x00;	// initialize seatbelt fastened
		unsigned char seatLight = 0x00;		// initialize seatbelt light
		
		// if gas level 1 or 2, set indicator to light up PC5
		if (gaslvl == 1 || gaslvl == 2) {
			indicator = 0x20;
		}
		// if gas level 3 or 4, set indicator to light up PC5 and PC4
		if (gaslvl == 3 || gaslvl == 4) {
			indicator = 0x30;
		}
		// if gas level 5 or 6, set indicator to light up PC5 and PC4 and PC3
		if (gaslvl == 5 || gaslvl == 6) {
			indicator = 0x38;
		}
		// if gas level 7 or 8 or 9, set indicator to light up PC5 and PC4 and PC3 and PC2
		if (gaslvl == 7 || gaslvl == 8 || gaslvl == 9) {
			indicator = 0x3C;
		}
		// if gas level 10 or 11 or 12, set indicator to light up PC5 through PC1
		if (gaslvl == 10 || gaslvl == 11 || gaslvl == 12) {
			indicator = 0x3E;
		}
		// if gas level 13 or 14 or 15, set indicator to light up PC5 through PC0
		if (gaslvl == 13 || gaslvl == 14 || gaslvl == 15) {
			indicator = 0x3F;
		}
		// if gas level 0 or 1 or 2 or 4, set low fuel light up to PC6
		if (gaslvl == 0 || gaslvl == 1 || gaslvl == 2 || gaslvl == 3 || gaslvl == 4) {
			indicator = indicator + 0x40;
		}
		// retrieve bit 5. if 1, set keyIn to 1xxxx
		if (GetBit(seatLight1, 4) == 0x01) {
			keyIn = 0x10;
		}
		// retrieve bit 6. if 1, set seated to 10xxxx
		if (GetBit(seatLight1, 5) == 0x01) {
			seated = 0x20;
		}
		// retrieve bit 6. if 1, set seatbeltOn to 100xxxx
		if (GetBit(seatLight1, 6) == 0x01) {
			seatbeltOn = 0x40;
		}
		// sum all seatLight related items				
		seatLight = keyIn + seated + seatbeltOn;
		// if seatLight = 11xxxx, turn on seatbelt light			
		if (seatLight == 0x30) {
			indicator = indicator + 0x80;
		}
		//output light indicators
		PORTC = indicator;
	}
	return 0;
}

