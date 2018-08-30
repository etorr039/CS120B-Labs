/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab 0.1
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 1; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 100; // GCD of all tasks
task tasks[1]; // create array of tasks

#include "timer.h"

// === Define Tasks ===
enum SM1_States {sm1_display};
int SM1_Tick(int state);

// === Main Function ===
int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00; 
	
	unsigned char i = 0;

	tasks[i].state = sm1_display; // initial state
	tasks[i].period = 100; // state period
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &SM1_Tick; // function pointer
	
	TimerSet(tasksPeriodGCD); // set system timer
	TimerOn(); // start system timer
	
	while(1) {} 
}

int SM1_Tick(int state) {

	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	
	// === Transitions ===
	switch (state) {
		case sm1_display:	
			break;
			
		default:   	        
			state = sm1_display;
			break;
	}
	
	// === Actions ===
	switch (state) {
		case sm1_display:   // If illuminated LED in bottom right corner
		if (column_sel == 0xFE && column_val == 0x80) {
			column_sel = 0x7F; // display far left column
			column_val = 0x01; // pattern illuminates top row
		}
		// else if far right column was last to display (grounded)
		else if (column_sel == 0xFE) {
			column_sel = 0x7F; // resets display column to far left column
			column_val = column_val << 1; // shift down illuminated LED one row
		}
		// else Shift displayed column one to the right
		else {
			column_sel = (column_sel >> 1) | 0x80;
		}
		break;
		default:   	        break;
	}
	
	PORTA = column_val; // PORTA displays column pattern
	PORTB = column_sel; // PORTB selects column to display pattern

	return state;
};
