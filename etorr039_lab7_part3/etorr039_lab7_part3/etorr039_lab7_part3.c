/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 7  Exercise 3
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define OUTPUT PORTB
#define BUTTON (~PINA & 0x04)

const unsigned char tasksNum = 4; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 1; // GCD of all tasks
const unsigned long periodThreeLed = 300; // Task 1 period
const unsigned long periodBlinkingLed = 1000; // task 2 period
const unsigned long periodCombineLed = 1; // Task 3 period
const unsigned char periodSpeaker = 1; // Task 4 period

typedef struct task { // new data type (task)
	int state; // state can be represented as an int
	unsigned long period; // time always long
	unsigned long elapsedTime; // time always long
	int(*TickFct)(int); // have each tick function take in an int and return an int (state)
} task;

task tasks[4]; // create array of tasks

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
	for (i = 0; i < tasksNum; i++) { // Heart of the scheduler code
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

enum threeLedStates {Start1, Led1, Led2, Led3}; // Task 1
int TickFct_threeLed(int state); // take in state, return state

enum blinkingLedStates {Start2, On, Off}; // Task 2
int TickFct_blinkingLed(int state); // take in state, return state

enum combineLedStates {Start3, Lights}; // Task 3
int TickFct_combineLed(int state); // take in state, return state

enum speakerStates {Start4, soundOff, soundOn}; // Task 4
int TickFct_speaker(int state);

static unsigned char led1; // variables that will be passed from states
static unsigned char led2; // variable that will be passed from states
static unsigned char speaker;

int main(void) {
		
	DDRA = 0x00; PORTA = 0xFF;	
	DDRB = 0xFF; PORTB = 0x00; // I/O
	OUTPUT = 0; // initialize output
	
	unsigned char i = 0;
	tasks[i].state = Start1; // initialize state
	tasks[i].period = periodThreeLed; // set period
	tasks[i].elapsedTime = tasks[i].period; // record time
	tasks[i].TickFct = &TickFct_threeLed; // call tick function
	i++;
	tasks[i].state = Start2;
	tasks[i].period = periodBlinkingLed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_blinkingLed;
	i++;
	tasks[i].state = Start3;
	tasks[i].period = periodCombineLed;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_combineLed;
	i++;
	tasks[i].state = Start4;
	tasks[i].period = periodSpeaker;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_speaker;
	
	TimerSet(tasksPeriodGCD); // system timer
	TimerOn(); // start timer
	
	while (1) {} // inf loop
		
	return 0;
}

int TickFct_threeLed(int state) {
	switch(state) { // transitions
		case Start1:
		state = Led1;
		break;
		
		case Led1:
		state = Led2;
		break;
		
		case Led2:
		state = Led3;
		break;
		
		case Led3:
		state = Led1;
		break;
	}
	
	switch(state) { // actions
		case Start1:
		led1 = 0x00;
		break;
		
		case Led1:
		led1 = 0x01;
		break;
		
		case Led2:
		led1 = 0x02;
		break;
		
		case Led3:
		led1 = 0x04;
		break;
	}
	return state;
}

int TickFct_blinkingLed(int state) {
	switch(state) { // transitions
		case Start2:
		state = On;
		break;
		
		case On:
		state = Off;
		break;
		
		case Off:
		state = On;
		break;
	}
	
	switch(state) { // actions
		case Start2:
		led2 = 0x00;
		break;
		
		case On:
		led2 = 0x08;
		break;
		
		case Off:
		led2 = 0x00;
		break;
	}
	return state;
}

int TickFct_combineLed(int state) {
	switch(state) { // transitions
		case Start3:
		state = Lights;
		break;
		
		case Lights:
		state = Lights;
		break;
	}
	
	switch(state) { // actions
		case Start3:
		break;
		
		case Lights:
		OUTPUT = led1 + led2 + speaker;
		break;
	}
	return state;
}
int TickFct_speaker(int state) {
	switch(state) { // transitions
		case Start4:
		state = soundOff;
		break;
		
		case soundOff:
		state = soundOn;
		break;
		
		case soundOn:
		state = soundOff;
		break;
	}

	switch(state) { // actions
		case Start4:
		break;
		
		case soundOff:
		speaker = 0x00;
		break;
		
		case soundOn:
		if (BUTTON == 0x04) {
			speaker = 0x10;
		}
		break;
	}
	return state;
}