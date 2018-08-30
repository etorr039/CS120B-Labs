/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 6  Exercise 1 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

#define BUTTON (~PINA & 0x03) // set xxxxxxbb

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
	TimerFlag = 1; // raise flag
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

// start state machine
enum States {init, wait, increment, decrement, reset} state;
	
unsigned char display; // number displayed on LCD
	
void LCD() {
	switch(state) {
		case init:
			state = wait;
			break;
			
		case wait:
			if (BUTTON == 0x01) {
				state = increment;
			}
			else if (BUTTON == 0x02) {
				state = decrement;
			}
			else if (BUTTON == 0x00) {
				state = wait;
			}
			else if (BUTTON == 0x03) {
				state = reset;
			}
			else {
				state = wait;
			}
			break;
			
		case increment:
			if (BUTTON == 0x01) {
				state = increment;
			}
			else if (BUTTON == 0x03) {
				state = reset;
			}
			else if (BUTTON == 0x00) {
				state = wait;
			}
			else if (BUTTON == 0x02) {
				state = decrement;
			}
				break;
				
		case decrement:
			if (BUTTON == 0x02) {
				state = decrement;
			}
			else if (BUTTON == 0x03) {
				state = reset;
			}
			else if (BUTTON == 0x01) {
				state = increment;
			}
			else if (BUTTON == 0x00) {
				state = wait;
			}
			break;
			
		case reset:
			if (BUTTON == 0x00) { //wait for button depress to reset
				state = wait;
				display = 0;
			}
			else {
				state = reset;
			}
			break;
 	}
	switch (state) {
		case init:
			display = 0x00; // set display to 0
			break;
		
		case wait:
			break;
			
		case increment:
			if (display < 9) { // count up to 9
				display++;
			}	
			break;
			
		case decrement:
			if (display > 0) { // count down to 0
				display--;
			}
			break;
			
		case reset:
			break;
		}
}

int main(void) {
	
	DDRA = 0x00; PORTA = 0xFF; // button input
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	state = init; //starting state
	display = 0; // initialize output
	
	LCD_init(); // initialize LCD
	
	TimerSet(1000); // 1 second period
	TimerOn(); // turn timer on
	
	while (1) {
		LCD(); // call tick function
		LCD_Cursor(1); // override previous number on LCD
		LCD_WriteData(display + '0'); // write display number to LCD
		while(!TimerFlag) {} // wait for period
		TimerFlag = 0; // lower flag
	}
	return 0;
}