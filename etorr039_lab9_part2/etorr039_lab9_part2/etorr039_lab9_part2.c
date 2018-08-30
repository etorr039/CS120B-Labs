/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 9 Exercise 2
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

const unsigned char tasksNum = 2; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 50; // gcd of all tasks
static unsigned long periodOnOff = 100; // Task 1 period
const unsigned long periodIncDec = 100; // task 2 period

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

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum onOffStates {Start1, wait1, on, off}; // Task 1
int TickFct_onOff(int state); // take in state, return state

enum incDecStates {Start2, wait2, inc, dec}; // Task 2
int TickFct_incDec(int state); // take in state, return state

unsigned char i;
unsigned char k;
double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

int main(void) {
		
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; // I/O
	
	PWM_off();
	
	unsigned char i = 0;
	tasks[i].state = Start1; // initialize state
	tasks[i].period = periodOnOff; // set period
	tasks[i].elapsedTime = tasks[i].period; // record time
	tasks[i].TickFct = &TickFct_onOff; // call tick function
	++i;
	tasks[i].state = Start2;
	tasks[i].period = periodIncDec;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_incDec;
	
	TimerSet(tasksPeriodGCD); // system timer
	TimerOn(); // start timer
	
	while (1) {} 
	return 0;
}

int TickFct_onOff(int state) {	
	
	unsigned char BUTTONS = (~PINA & 0x01);
	
	switch(state) { 
		case Start1:
			state = wait1;
			k = 0;
			break;
			
		case wait1:
			if (BUTTONS == 0x01 && k == 0) {
				state = on;
				PWM_on();
				set_PWM(notes[i]);
				k = 1;
			}
			else if (BUTTONS == 0x01 && k == 1) {
				state = off;
				PWM_off();
				k = 0;
			}
			else {
				state = wait1;
			}
			break;
			
		case on:
			if (BUTTONS == 0x01) {
			state = on;
			}
			else if (BUTTONS == 0x00) {
				state = wait1;
			}
			else {
				state = inc;
			}
			break;
			
		case off:
			if (BUTTONS == 0x01) {
				state = off;
			}
			else if (BUTTONS == 0x00) {
				state = wait1;
			}
			else {
				state = inc;
			}
			break;
			
		default:
			state = wait1;
			break;
	}	
	return state;
}

int TickFct_incDec(int state) {
	
	unsigned char BUTTONS = (~PINA & 0x06);
	
	switch(state) { 
		case Start2:
			state = wait2;
			i = 0;
			break;
			
		case wait2:
			if (BUTTONS == 0x02 && i < 7) {
				state = inc;
				i++;
				set_PWM(notes[i]);
			}
			else if (BUTTONS == 0x04 && i > 0) {
				state = dec;
				i--;
				set_PWM(notes[i]);
			}
			else {
				state = wait2;
			}
			break;
			
		case inc:
			if (BUTTONS == 0x02) {
				state = inc;
			}
			else if (BUTTONS == 0x00) {
				state = wait2;
			}
			else {
				state = inc;
			}
			break;
			
		case dec:
			if (BUTTONS == 0x04) {
				state = dec;
			}
			else if (BUTTONS == 0x00) {
				state = wait2;
			}
			else {
				state = dec;
			}
			break;
			
		default:
			state = wait2;
			break;
	}
	return state;
}