/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 10 Exercise 1
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 2; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 100; // gcd of all tasks
task tasks[2]; // create array of tasks

#include "timer.h"
#include "keypad.h"

#define LED PORTB

enum inputStates {start1, keyPress};
int TickFct_keypad(int state);

enum outputStates {start2, display};
int TickFct_output(int state);

static unsigned char KEY = 0;

int main(void)
{
	
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	
	unsigned char i = 0;
	tasks[i].state = start1; // initialize state
	tasks[i].period = 100; // set period
	tasks[i].elapsedTime = tasks[i].period; // record time
	tasks[i].TickFct = &TickFct_keypad; // call tick function
	++i;
	tasks[i].state = start2;
	tasks[i].period = 100;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_output;
	
	TimerSet(tasksPeriodGCD); // system timer
	TimerOn(); // start timer
	
	while(1) {}
}

int TickFct_keypad(int state){
		unsigned char x;
		x = GetKeypadKey();
		
		switch(state) {
			case start1:
			state = keyPress;
			break;
			
			case keyPress:
			switch (x) {
				case '\0': KEY = 0x1F; break; // All 5 LEDs on
				case '1': KEY = 0x01; break; // hex equivalent
				case '2': KEY = 0x02; break;
				case '3': KEY = 0x03; break;
				case '4': KEY = 0x04; break;
				case '5': KEY = 0x05; break;
				case '6': KEY = 0x06; break;
				case '7': KEY = 0x07; break;
				case '8': KEY = 0x08; break;
				case '9': KEY = 0x09; break;
				case 'A': KEY = 0x0A; break;
				case 'B': KEY = 0x0B; break;
				case 'C': KEY = 0x0C; break;
				case 'D': KEY = 0x0D; break;
				case '*': KEY = 0x0E; break;
				case '0': KEY = 0x00; break;
				case '#': KEY = 0x0F; break;
				default: KEY = 0x1B; break; // Should never occur. Middle LED off.
			}
			state = keyPress;
			break;
		}
	return state;
}


int TickFct_output(int state){
	switch(state) {
		case start2:
		state = display;
		break;
		
		case display:
		state = display;
		break;
	}
	LED = KEY;	
	return state;
}