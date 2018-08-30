/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 3  Exercise 3 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define INPUT PINA
#define OUTPUT PORTB
#define STATE PORTC

enum locksys {init, wait, hash, open, outside} lockState;

void lockSystem()	// function to call from main for SM
{		
	// state transitions
	switch(lockState) {
		// initialize state. always go to wait
		case init:
			STATE = 1;
			lockState = wait;
			break;
		// wait for a button press
		case wait:
			STATE = 2;
			if (INPUT == 0x04) { // if # pressed go to hash
				lockState = hash;
			}
			else if (INPUT == 0) { // if nothing pressed go to wait
				lockState = wait;
			}
			else { // if anything else go to init
				lockState = wait; 
			}
			break;
		// first correct button press
		case hash:
			if (INPUT == 0x02 && !(STATE == 5)) { // if Y pressed go to open
				lockState = open;
			}
			else if (INPUT == 0x00) { // if nothing pressed stay in hash
				lockState = hash;
			}
			else if (STATE == 5) {
				lockState = hash;
			}
			else if (INPUT == 0x04) { // if # pressed go to hash
				lockState = hash;
			}
			else { // anything else go to  init
				lockState = init; 
			}
			break;
		// second correct button press ==> unlock
		case open:
			STATE = 5;
			if (INPUT == 0x04) {
				lockState = hash;
			}
			else if (INPUT == 0x80) {
				lockState = outside;
			}
			else {
				lockState = open;
			}
			break;
		// lock door if inside
		case outside:
			STATE = 6;
			lockState = init;
		
		default:
			lockState = init;
			break;
	}
	// state actions
	switch(lockState) {
		case init:
			INPUT = 0; // initial input value
			break;
		
		case wait:
			break;
		
		case hash: 
			break;
		
		case open:
			OUTPUT = 1; // unlock
			break;
			
		case outside:
			OUTPUT = 0; // lock
			break;
		
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	OUTPUT = 0; // initialize output start
	lockState = init; // initialize first state call
	
    while (1) 
    {	
		lockSystem(); // call SM
    }
	return 0;
}

