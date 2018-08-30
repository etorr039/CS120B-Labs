/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab 0.4
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 3; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 1; // GCD of all tasks
task tasks[3]; // create array of tasks

#include "timer.h"

#define BUTTON (~PINC & 0x0F)

// === Define Tasks ===
enum ship_States {makeShip};
int square_Tick(int state);

enum move_States {start, wait, up, down, left, right, unPress};
int move_Tick(int state);

enum reGen_States {checkFlag, reGenLeft, reGenRight};
int reGen_Tick(int state);

void shiftReg(unsigned short data);

// === Global Variables ===
char x[8] = {	0b00000100,
				0b00001000,
				0b00010000, 
				0b00000000, 
				0b00000000, 
				0b00000000,
				0b00000000,
				0b00000000,
			};
			
char y[8] = {	0b01111111,
				0b00111111,
				0b01111111,
				0b11111111, 
				0b11111111, 
				0b11111111,
				0b11111111,
				0b11111111,
			};
			
unsigned short MATRIX;
unsigned char reGenLeft_FLAG = 0;
unsigned char reGenRight_FLAG = 0;

// === Main Function ===
int main(void)
{
	DDRB = 0xFF; PORTB = 0x00; 
	DDRC = 0x00; PORTC = 0xFF;
	
	unsigned char j = 0;

	tasks[j].state = makeShip; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &square_Tick; // function pointer
	j++;
	tasks[j].state = start; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &move_Tick; // function pointer
	j++;
	tasks[j].state = checkFlag; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &reGen_Tick; // function pointer
	
	TimerSet(tasksPeriodGCD); // set system timer
	TimerOn(); // start system timer
	
	while(1) {} 
}

int square_Tick(int state) {

	// === Local Variables ===
	static unsigned char i = 0;
	
	// === Transitions ===
	switch (state) {
		case makeShip:
			state = makeShip;
			for (i = 0; i < 8; i++) {
				MATRIX = x[i] << 8;
				MATRIX = MATRIX + y[i];
				shiftReg(MATRIX);
				if (i > 7) {
					i = 0;
				}
			}
			break;
	}
	return state;
};

int move_Tick(int state){

	// === Local Variables ===
	static unsigned char i, j;
	
	// === Transitions ===
	switch(state) {
		case start:
			state = wait;
			i = 0;
			j = 3;
			break;
			
		case wait:
			if (BUTTON == 0) {
				state = wait;
			}
			else if (BUTTON == 0x01) {
				state = left;
			}
			else if (BUTTON == 0x02) {
				state = right;
			}
	/*		else if (BUTTON == 0x04) {
				state = up;
			}
			else if (BUTTON == 0x08) {
				state = down;
			}
	*/
			else {
				state = wait;
			}
			break;
			
	/*	case up:
			state = unPress;
			for (i = 0; i < 8; i++) {
				y[i] = ~y[i];
				y[i] = y[i] >> 1;
				y[i] = ~y[i];
			}
			break;
			
		case down:
			state = unPress;
			for (i = 0; i < 8; i++) {
				y[i] = ~y[i];
				y[i] = y[i] << 1;
				y[i] = ~y[i];
			}
			break;
	*/
			
		case left:
			state = unPress;
			if (j >= 1) {
				for (i = 0; i < 8; i++) {
					x[i] = x[i] >> 1;
				}
				j--;
			}
			break;
			
		case right:
			state = unPress;
			if (j <= 6) {
				for (i = 0; i < 8; i++) {
					x[i] = x[i] << 1;
				}
				j++;
			}
			break;
			
		case unPress:
			if (j == 1) {
				reGenLeft_FLAG = 1;
			}
			if (j == 6) {
				reGenRight_FLAG = 1;
			}
			if (BUTTON == 0x00) {
				state = wait;
			}
			else {
				state = unPress;
			}
			break;
	}
	return state;
};

int reGen_Tick(int state) {
	
	switch(state) {
		case checkFlag:
			if (reGenLeft_FLAG == 1) {
				state = reGenLeft;
			}
			else if (reGenRight_FLAG == 1) {
				state = reGenRight;
			}
			else {
				state = checkFlag;
			}
			break;
		
		case reGenLeft:
			x[0] = 0b00000001;
			x[1] = 0b00000010;
			x[2] = 0b00000100;
			reGenLeft_FLAG = 0;
			state = checkFlag;
			break;
			
		case reGenRight:
			x[0] = 0b00100000;
			x[1] = 0b01000000;
			x[2] = 0b10000000;
			reGenRight_FLAG = 0;
			state = checkFlag;
			break;
	}
	
	return state;
};

void shiftReg(unsigned short data) {
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
	// set RCLK = 1. Rising edge copies data from ?Shift? register to ?Storage? register
	PORTB |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}