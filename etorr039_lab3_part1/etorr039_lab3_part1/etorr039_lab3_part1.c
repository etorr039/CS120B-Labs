/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 3  Exercise 1 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define BUTTON PINA
#define LEDs PORTB
#define STATE PORTC

enum LED_States {LED_init, LED_wait, LED_1, LED_2, s0, s1} LED_State; // SM States

void lights()	// function to call from main for SM
{		
	// state transitions
	switch(LED_State) {
		// initialize. Always go to wait
		case LED_init:
			LED_State = LED_wait;
			break;
		// wait for button press
		case LED_wait:
			if (BUTTON == 0) {
				LED_State = LED_wait;
			}
			else if (BUTTON == 1 && STATE == 0) {
				LED_State = LED_1;
			}
			else if (BUTTON == 1 && STATE == 1) {
				LED_State = LED_2;
			}
			else {
				LED_State = LED_wait;
			}
			break;
		// turn PB0 off, PB1 on
		case LED_1:
			if (BUTTON == 1 && STATE == 0) {
				LED_State = LED_1;
			}
			else if (BUTTON == 0) {
				LED_State = s1;
			}
			break;
		// wait for button press action complete
		case s1:
			LED_State = LED_wait;
			break;
		// turn PB0 on, PB1 off
		case LED_2:
			if (BUTTON == 1 && STATE == 1) {
				LED_State = LED_2;
			}
			else if (BUTTON == 0) {
				LED_State = s0;
			}
			break;
		// wait for button press action complete	
		case s0:
			LED_State = LED_wait;
			break;
			
		default:
			LED_State = LED_init;
			break;
	}
	// state actions
	switch(LED_State) {
		case LED_init:
			break;
		
		case LED_wait:
			break;
		
		case LED_1:
			LEDs = 0x02; 
			break;
			
		case s1:
			STATE = 1;
			break;
			
		case LED_2:
			LEDs = 0x01;
			break;
			
		case s0:
			STATE = 0;
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
	
	LED_State = LED_init; // initialize first state call
	LEDs = 0x01; // initial output values
	STATE = 0;
	
    while (1) 
    {	
		lights(); // call SM
    }
	return 0;
}

