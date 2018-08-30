/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 5  Exercise 2 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED PORTC
#define BUTTON (~PINA & 0x01)

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

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

enum States {start, led1, led2, led3, stop, unpress, reset} state;
	
void lightGame() {
	switch(state) { // transitions
		case start:
			state = led1;
			break;
			
		case led1: 
			if (BUTTON == 1) {
				state = stop;
			}
			else {
				state = led2;
			}
			break;
			
		case led2:
			if (BUTTON == 1) {
				state = stop;
			}
			else {
				state = led3;
			}
			break;
			
		case led3:
			if (BUTTON == 1) {
				state = stop;
			}
			else {
				state = led1;
			}
			break;
			
		case stop:
			if (BUTTON == 0) {
				state = unpress;
			}
			else {
				state = stop;
			}
			break;
			
		case unpress:
			if (BUTTON == 1) {
				state = reset;
			}
			else {
				state = unpress;
			}
			break;
			
		case reset:
			if (BUTTON == 0) {
				state = led1;
			}
			else {
				state = reset;
			}
			break;
	}
	switch(state) { // actions
		case start:
			state = led1;
			break;

		case led1: // light up first LED
			LED = 0x01;
			break;
		
		case led2: // light up second LED
			LED = 0x02;
			break;
		
		case led3: // light up third LED
			LED = 0x04;
			break;
		
		case stop:
			break;
		
		case unpress:
			break;
		
		case reset:
			break;
	}
}

int main(void) {
	
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	TimerSet(200); // period of 300 ms
	TimerOn(); //activate timer
	
	state = start; // initialize state
	LED = 0x01; // initialize output
	
	while (1) {
		lightGame(); //call function
		while(!TimerFlag) {} // wait for period
		TimerFlag = 0; // lower flag
	}
	return 0;
}