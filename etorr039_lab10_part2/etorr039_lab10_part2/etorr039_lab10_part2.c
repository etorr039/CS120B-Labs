/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 10 Exercise 2
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 1; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 200; // gcd of all tasks
task tasks[1]; // create array of tasks

#include "timer.h"
#include "io.c"

#define BUTTON (~PINB & 0x20)

enum scrollingStates {start2, scroll, pause};
int TickFct_scroll(int state);

signed char shiftText = 2;
const unsigned char* MESSAGE = "CS120B is Legend... wait for it DARY!";

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0xFF; 
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init(); // initialize LCD
	LCD_ClearScreen();
	
	unsigned char i = 0;

	tasks[i].state = start2;
	tasks[i].period = 600;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_scroll;
	
	TimerSet(tasksPeriodGCD); // system timer
	TimerOn(); // start timer
	
	while(1) {}
}




int TickFct_scroll(int state){
	
	static unsigned char k = 0;
	static unsigned char i = 0;
		
	switch(state) {
		case start2:
			state = scroll;
			break;
			
		case scroll:
			if (i < 8) {
				shiftText--;
				i++;
				state = scroll;
			}
			if (i == 8) {
				state = pause;
			}
			LCD_DisplayString(shiftText, MESSAGE);
			break;
			
		case pause:
			if (k < 1) {
				state = pause;
				k++;
			}
			else {
			shiftText = 2;
			i = 0;
			k = 0;
			state = scroll;
			}
			break;
	}
	return state;
}