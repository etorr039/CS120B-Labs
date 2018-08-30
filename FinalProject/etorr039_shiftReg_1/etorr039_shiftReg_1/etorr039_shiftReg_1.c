/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab shiftReg 0.1
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 1; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 50; // GCD of all tasks
task tasks[1]; // create array of tasks

#include "timer.h"

#define BUTTONS (~PINC & 0x03)

// === Define Tasks ===
enum shift_States {start, wait, upPress, downPress};
int shift_Tick(int state);

void transmit_data(unsigned short data);

// === Main Function ===
int main(void)
{
	DDRC = 0x00; PORTC = 0xFF; // set PORTC as input
	DDRB = 0xFF; PORTB = 0x00; // set PORTB as output
	
	unsigned char i = 0;

	tasks[i].state = start; // initial state
	tasks[i].period = 50; // state period
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &shift_Tick; // function pointer
	
	TimerSet(tasksPeriodGCD); // set system timer
	TimerOn(); // start system timer
	
	while(1) {} 
}

int shift_Tick(int state) {

	// === Local Variables ===
	static unsigned short y = 0x0001;
	
	// === Transitions ===
	switch (state) {
		case start:
			transmit_data(y);
			state = wait;
			break;
			
		case wait:
			if (BUTTONS == 0x01) {
				state = upPress;
			}
			else if (BUTTONS == 0x02) {
				state = downPress;
			}
			else {
				state = wait;
			}
			break;
			
		case upPress:
			state = wait;
			y++;
			transmit_data(y);
			break;
			
		case downPress:
			state = wait;
			y--;
			transmit_data(y);
			break;
			
	}
	return state;
};

void transmit_data(unsigned short data) {
	int i;
	for (i = 0; i < 16 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x08;
		// set SER = next bit of data to be sent.
		PORTB |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}
