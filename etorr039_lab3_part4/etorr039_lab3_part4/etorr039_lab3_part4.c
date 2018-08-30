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

enum locksys {init, wait, hash, open, close} lockState; 

void lockSystem()	// function to call from main for SM
{		
	// state transitions
	switch(lockState) {
		// initialize SM. always go to wait
		case init:
			STATE = 1;
			lockState = wait;
			break;
		// wait for button press
		case wait:
			STATE = 2;
			if (INPUT == 0x04) { 
				lockState = hash;
			}
			else if (INPUT == 0x80) { 
				lockState = close;
			}
			else if (INPUT == 0) { 
				lockState = wait;
			}
			else { 
				lockState = init; 
			}
			break;
		// first correct button press
		case hash:
			if (INPUT == 0x02 && !(STATE == 5)) { 
				lockState = open;
			}
			else if (INPUT == 0x02 && (STATE == 5)) { 
				lockState = close;
			}
			else if (INPUT == 0x00) { 
				lockState = hash;
			}
			else if (STATE == 5) {
				lockState = hash;
			}
			else if (INPUT == 0x04) { 
				lockState = hash;
			}
			else { 
				lockState = init; 
			}
			break;
		// lock door	
		case close:
			STATE = 4;
			if (INPUT == 0x02) {
				lockState = close;
			}
			else {lockState = init;
			}
			break;
		// unlock door
		case open:
			STATE = 5;
			if (INPUT == 0x04) {
				lockState = hash;
			}
			else {
				lockState = open;
			}
			break;
		
		default:
			lockState = init;
			break;
	}
	// state actions
	switch(lockState) {
		case init:
			INPUT = 0; // initialize input value
			break;
		
		case wait:
			break;
		
		case hash: 
			break;
		
		case close:
			OUTPUT = 0; // lock
			break;
		
		case open:
			OUTPUT = 1; // unlock
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
	
	OUTPUT = 0; // initialize output
	lockState = init; // initialize first state call
	
    while (1) 
    {	
		lockSystem(); // call SM
    }
	return 0;
}


