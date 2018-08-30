/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab 0.3
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 1; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 1; // GCD of all tasks
task tasks[1]; // create array of tasks

#include "timer.h"

#define LIGHT PORTA
#define GROUND PORTB
#define BUTTON (~PINC & 0x03)

// === Define Tasks ===
enum square_States {start};
int square_Tick(int state);

// === Main Function ===
int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00; 
	DDRC = 0x00; PORTC = 0xFF;
	
	unsigned char i = 0;

	tasks[i].state = start; // initial state
	tasks[i].period = 1; // state period
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &square_Tick; // function pointer
	
	TimerSet(tasksPeriodGCD); // set system timer
	TimerOn(); // start system timer
	
	while(1) {} 
}

int square_Tick(int state) {

	// === Local Variables ===
	char x[4] = {0b00100000, 0b00010000, 0b00001000, 0b00000100};
	char y[4] = {0b11000011, 0b11011011, 0b11011011, 0b11000011};
	static unsigned char i = 0;
	
	// === Transitions ===
	switch (state) {
		case start:
			state = start;
				LIGHT = x[i];
				GROUND = y[i];
				i++;
				if (i > 3) {
					i = 0;
				}
			break;
	}
	return state;
};