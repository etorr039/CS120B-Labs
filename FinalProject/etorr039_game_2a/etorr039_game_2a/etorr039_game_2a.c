/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab 0.21
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 1; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 100; // GCD of all tasks
task tasks[1]; // create array of tasks

#include "timer.h"

#define LIGHT PORTA
#define GROUND PORTB
#define BUTTON (~PINC & 0x03)

// === Define Tasks ===
enum rows_States {start, wait, upPress, unPress1, downPress, unPress2};
int rows_Tick(int state);

// === Main Function ===
int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00; 
	DDRC = 0x00; PORTC = 0xFF;
	
	unsigned char i = 0;

	tasks[i].state = start; // initial state
	tasks[i].period = 100; // state period
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &rows_Tick; // function pointer
	
	TimerSet(tasksPeriodGCD); // set system timer
	TimerOn(); // start system timer
	
	while(1) {} 
}

int rows_Tick(int state) {

	// === Local Variables ===
	static unsigned char x;
	static unsigned char y;
	static unsigned char i;
	
	// === Transitions ===
	switch (state) {
		case start:
			state = wait;
			x = 0x01;
			y = 0x00;
			i = 7;
			LIGHT = x;
			GROUND = y;
			break;
		
		case wait:
		if (BUTTON == 0) {
				state = wait;
			}
			else if (BUTTON == 0x01) {
				state = upPress;
			}
			else if (BUTTON == 0x02) {
				state = downPress;
			}
			else {
				state = wait;
			}
			break;
		
		case upPress:
			state = unPress1;
			if (i < 7) {
				x = x >> 1;
				i++;
			}
			break;
		
		case downPress:
			state = unPress2;
			if (i > 0) {
				x = x << 1;
				i--;
			}
			break;
		
		case unPress1:
			if (BUTTON == 0x01) {
				state = unPress1;
			}
			else if (BUTTON == 0x00) {
				state = wait;
			}
			else {
				state = unPress1;
				}
			break;
		
		case unPress2:
			if (BUTTON == 0x02) {
				state = unPress2;
			}
			else if (BUTTON == 0x00) {
				state = wait;
			}
			else {
				state = unPress2;
			}
			break;
	}

	LIGHT = x;
	
	return state;
};