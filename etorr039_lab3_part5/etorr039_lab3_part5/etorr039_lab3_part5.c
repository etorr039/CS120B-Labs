/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 3  Exercise 4 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define INPUT PINA
#define OUTPUT PORTB

enum locksys {init, check1, check2, open, close} lockState;  // type_name {all_states} current_state

void lockSystem()	// function to call from main for SM
{			
	unsigned char i;
	unsigned char k;
	int arr[] = {4,1,2,1};
	// state transitions
	switch(lockState) {
		case init:
			lockState = check1;
			break;
		
		case check1:
			for (i = 0; i < 4; i++) {
				if (INPUT == arr[i]) {
					k++;
				}
				else {
					break;
				}
			}
			if (k == 4) {
				lockState = open;
			}
			else {
				lockState = check1;
			}
			break;
			
		case check2:
			for (i = 0; i < 4; i++) {
				if (INPUT == arr[i]) {
					k++;
				}
				else {
					break;
				}
			}
			if (k == 8) {
				lockState = close;
			}
			else {
				lockState = check2;
			}
			break;
			
		case close:
			lockState = init;
			break;
		
		case open:
			lockState = check2;
			break;
		
		default:
			lockState = init;
			break;
	}
	// state actions
	switch(lockState) {
		case init:
			INPUT = 0; // initial output value
			k = 0;
			break;
		
		case close:
			OUTPUT = 0; // close the door
			break;
		
		case open:
			OUTPUT = 1; // open the door
			break;
			
		case check1:
			k = 0;
			break;
			
		case check2:
			k = 4;
			break;
			
		default:
			break;
	}
}

int main(void)
{	
	OUTPUT = 0;
	lockState = init; // initialize first state call
	
    while (1) 
    {	
		lockSystem(); // call SM
    }
	return 0;
}

