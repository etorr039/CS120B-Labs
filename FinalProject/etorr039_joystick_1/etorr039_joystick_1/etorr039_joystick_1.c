/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab joystick
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 3; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 1; // GCD of all tasks
task tasks[3]; // create array of tasks

#include "timer.h"

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

// === Define Tasks ===
enum ship_States {makeShip};
int square_Tick(int state);

enum move_States {moveStart, wait, up, down, left, right, unPress};
int move_Tick(int state);

enum reGen_States {checkFlag, reGenLeft, reGenRight};
int reGen_Tick(int state);

void shiftReg(unsigned short data);

// === Global Variables ===
char shipX[8] = {	0b00000100,
					0b00001000,
					0b00010000, 
					0b00000000, 
					0b00000000, 
					0b00000000,
					0b00000000,
					0b00000000,
				};
			
char shipY[8] = {	0b01111111,
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
unsigned char position;

// === Main Function ===
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; 
	
	ADC_init();
	
	unsigned char j = 0;

	tasks[j].state = makeShip; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &square_Tick; // function pointer
	j++;
	tasks[j].state = moveStart; // initial state
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
				MATRIX = shipX[i] << 8;
				MATRIX = MATRIX + shipY[i];
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
	static unsigned char i;
	unsigned short joyStick = ADC;
	
	// === Transitions ===
	switch(state) {
		case moveStart:
			state = wait;
			i = 0;
			position = 3;
			break;
			
		case wait:
			if (joyStick > 250 && joyStick < 750) {
				state = wait;
			}
			else if (joyStick <= 250) {
				state = left;
			}
			else if (joyStick >= 750) {
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
			if (position >= 1) {
				for (i = 0; i < 8; i++) {
					shipX[i] = shipX[i] >> 1;
				}
				position--;
			}
			break;
			
		case right:
			state = unPress;
			if (position <= 6) {
				for (i = 0; i < 8; i++) {
					shipX[i] = shipX[i] << 1;
				}
				position++;
			}
			break;
			
		case unPress:
			if (position == 1) {
				reGenLeft_FLAG = 1;
			}
			if (position == 6) {
				reGenRight_FLAG = 1;
			}
			if (joyStick > 250 && joyStick < 750) {
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
			shipX[0] = 0b00000001;
			shipX[1] = 0b00000010;
			shipX[2] = 0b00000100;
			reGenLeft_FLAG = 0;
			state = checkFlag;
			break;
			
		case reGenRight:
			shipX[0] = 0b00100000;
			shipX[1] = 0b01000000;
			shipX[2] = 0b10000000;
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

