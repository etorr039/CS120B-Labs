/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 10 Exercise 3
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
#include "io.c"

unsigned char KEY = 0;
unsigned char noInput = 1;
unsigned char FLAG = 0;

enum keypad_States {press, standby};
int keypad(int state);

enum output_States {write, wait};
int output (int state);

void taskScheduler();

int main(void) {
	
	DDRC = 0xF0; PORTC = 0x0F;  //Keypad is on PORTC
	DDRA = 0xFF; PORTA = 0x00;  //LCD is on PORTA
	DDRD = 0xFF; PORTD = 0x00;  //LCD is also on PORTD
	
	taskScheduler();
	
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	LCD_init();

	while(1) {}
	
	return 0;
}

void taskScheduler() {
	
	unsigned short i = 0; // Scheduler for-loop iterator
	
	tasks[i].state = standby;
	tasks[i].period = 100;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &keypad;
	i++;
	tasks[i].state = write;
	tasks[i].period = 100;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &output;
}

int keypad(int state) {
	unsigned char x = GetKeypadKey();
	
	switch(state) {
		case standby:
		if(x != '\0') {
			state = press;
			KEY = x;
			FLAG = 1;
			} else {
			state = standby;
		}
		break;
		
		case press:
		if(x == '\0') {
			state = standby;
			FLAG = 0;
			} else {
			state = press;
		}
		break;
		
		default:
		state = standby;
	}
	return state;
}

int output (int state) {
	switch(state) {
		case write:
		if(FLAG == 0) {
			if(KEY != '\0') {
				if(noInput != KEY) {
					noInput = KEY;
					LCD_Cursor(1);
					LCD_WriteData(KEY);
				}
				state = write;
			}
			else {
				state = wait;
			}
			break;
			
			case wait:
			if(FLAG == 0) {
				state = write;
				} else {
				state = wait;
			}
			break;
			
			default:
			state = write;
			break;
		}
	}	
	return state;
}