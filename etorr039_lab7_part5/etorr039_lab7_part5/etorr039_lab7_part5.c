/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 7  Exercise 5 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED PORTB
#define BUTTON (~PINA & 0x03)

const unsigned char tasksNum = 2; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 200; // gcd of all tasks
static unsigned long periodLed = 1000; // Task 1 period
const unsigned long periodFaster = 1000; // task 2 period
const unsigned long periodHeld = 400; // period after holding button for 3 seconds

typedef struct task { // new data type (task)
	int state; // state can be represented as an int
	unsigned long period; // time always long
	unsigned long elapsedTime; // time always long
	int(*TickFct)(int); // have each tick function take in an int and return an int (state)
} task;

task tasks[2]; // create array of tasks

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
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
		if (tasks[i].elapsedTime >= tasks[i].period ) { // Ready
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
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

enum ledStates {Start1, wait1, add1, sub1, reset}; // Task 1
int TickFct_led(int state); // take in state, return state

enum fasterLedStates {Start2, wait2, periodSwap}; // Task 2
int TickFct_fasterLed(int state); // take in state, return state

unsigned char k;

int main(void) {
		
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; // I/O
	LED = 0; // initialize output
	
	unsigned char i = 0;
	tasks[i].state = Start1; // initialize state
	tasks[i].period = periodLed; // set period
	tasks[i].elapsedTime = tasks[i].period; // record time
	tasks[i].TickFct = &TickFct_led; // call tick function
	++i;
	tasks[i].state = Start2;
	tasks[i].period = periodFaster;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_fasterLed;
	
	TimerSet(tasksPeriodGCD); // system timer
	TimerOn(); // start timer
	
	while (1) {} // inf loop
		
	return 0;
}

int TickFct_led(int state) {
	switch(state) { // transitions
		case Start1:
		state = wait1;
		break;
		
		case wait1:
		if (BUTTON == 0x00) {
			state = wait1;
		}
		else if (BUTTON == 0x01) {
			state = add1;
		}
		else if (BUTTON == 0x02) {
			state = sub1;
		}
		else if (BUTTON == 0x03) {
			state = reset;
		}
		break;
		
		case add1:
		if (BUTTON == 0x00) {
			state = wait1;
		}
		else if (BUTTON == 0x01) {
			state = add1;
		}
		else if (BUTTON == 0x03) {
			state = reset;
		}
		else {
			state = wait1;
		}
		break;
		
		case sub1:
		if (BUTTON == 0x00) {
			state = wait1;
		}
		else if (BUTTON == 0x02) {
			state = sub1;
		}
		else if (BUTTON == 0x03) {
			state = reset;
		}
		else {
			state = wait1;
		}
		break;
		
		case reset:
		if (BUTTON == 0x03) {
			state = reset;
		}
		else if (BUTTON == 0x00) {
			state = wait1;
			LED = 0;
		}
		else {
			state = reset;
		}
		break;
	}
	
	switch(state) { // actions
		case Start1:
		k = 0;
		break;
		
		case wait1:
		k = 0;
		break;
		
		case add1:
		if (LED < 9) {
			LED++;
		}
		k++;
		break;
		
		case sub1:
		if (LED > 0) {
			LED--;
		}
		k++;
		break;
		
		case reset:
		break;
	}
	return state;
}

int TickFct_fasterLed(int state) {
	switch(state) { // transitions
		case Start2:
		state = wait2;
		break;
		
		case wait2:
		if (k < 3) {
			state = wait2;
		}
		else if (k == 3) {
			state = periodSwap;
		}
		break;
		
		case periodSwap:
		if (k >= 3) {
			state = periodSwap;
		}
		else if (k < 3) {
			state = wait2;
		}
		break;
	}
	
	switch(state) { // actions
		case Start2:
		break;
		
		case wait2:
		tasks[0].period = periodLed; // original period
		break;
		
		case periodSwap:
		tasks[0].period = periodHeld; // faster period
		break;
	}
	return state;
}