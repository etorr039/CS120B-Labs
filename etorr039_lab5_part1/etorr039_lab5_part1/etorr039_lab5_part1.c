/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 5  Exercise 1 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define OUTPUT PORTC

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

unsigned char i = 0;
unsigned char tmp = 1;

void TimerOn() {

	TCCR1B = 0x0B;
	OCR1A = 125;	
	TIMSK1 = 0x02; 
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80; 
}

void TimerOff() {
	TCCR1B = 0x00; 
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States {start, blink} state;
	
void lights() {
	switch(state) { // transitions
		case start:
			state = blink;
			break;
			
		case blink:
			state = blink;
			break;
			
	}
	switch(state) { // actions
		case start:
			break;

		case blink:
			// light up 3 lights consecutively then reset
			if (i < 2) {
				tmp = tmp << 1;
				i++;
			}
			else {
				tmp = 1;
				i = 0;
			}
			break;
	}
}

int main(void) {
	
	DDRC = 0xFF; PORTC = 0x00;
	
	TimerSet(1000); // period of 250 ms
	TimerOn(); //activate timer
	
	state = start; // initialize state
	OUTPUT = 0x01; // initialize output
	
	while (1) {
		lights(); //call function
		while(!TimerFlag) {} // wait for period
		TimerFlag = 0; // lower flag
		OUTPUT = tmp;
	}
	return 0;
}