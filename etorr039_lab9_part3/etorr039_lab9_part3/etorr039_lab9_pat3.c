/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: B21
 *	Assignment: Lab 9 Exercise 3
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

const unsigned char tasksNum = 2; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 1; // gcd of all tasks
static unsigned long periodButtonInput = 1; // Task 1 period
const unsigned long periodPlaySong = 1; // task 2 period

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

//enum playSong {Start1, wait1, on, off}; // Task 1
//int TickFct_playSong(int state); // take in state, return state

#define D 293.66
#define F 349.23
#define D1 587.33
#define E1 659.25 
#define F1 698.46
#define C1 523.25
#define A 440.00
#define G 392.00
#define E 329.63

double notes[43] =	{D, F, D1, D, F, D1, E1, F1, E1, 
					F1, E1, C1, A, A, D, F, G, A, A, 
					D, F, G, E, D, F, D1, D, F, D1,	
					E1, F1, E1, F1, E1, C1, A, A, D,
					F, G, A, A, D};

short held[43] =	{100, 100, 500, 100, 100, 500, 500, 
					100, 100, 100, 100, 100, 500, 250, 
					200, 100, 100, 500, 200, 250, 100, 
					100, 500, 100, 100, 600, 100, 100, 
					600, 500, 100, 100, 100, 100, 100,
					500, 250, 200, 100, 100, 700, 200, 
					1000};
	
short pause[43] =	{100, 100, 200, 100, 100, 200, 100,
					80, 80, 80, 80, 80, 200, 100, 100,
					100, 100, 200, 100, 100, 100, 100,
					200, 100, 100, 200, 100, 100, 200,
					100, 80, 80, 80, 80, 80, 200, 100,
					100, 100, 100, 200, 100, 100};

enum buttonStates {playing, waiting};
enum songStates {notPlaying, music, play, chill};

int buttonInput(int state);
int playSong(int state);

unsigned short cnt = 0; 
unsigned long i = 0;
unsigned char SONG_FLAG = 0;

int main(void) {
		
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; // I/O
	
	PWM_off();
	
	unsigned char i = 0;
	tasks[i].state = waiting; // initialize state
	tasks[i].period = periodButtonInput; // set period
	tasks[i].elapsedTime = tasks[i].period; // record time
	tasks[i].TickFct = &buttonInput; // call tick function
	++i;
	tasks[i].state = notPlaying;
	tasks[i].period = periodPlaySong;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &playSong;
	
	TimerSet(tasksPeriodGCD); // system timer
	TimerOn(); // start timer
	
	while (1) {} 
	return 0;
}

int buttonInput(int state)
{
	char buttons = 0x01 & ~PINA;
	
	switch (state) {
		case waiting:
			if(SONG_FLAG == 0 && buttons == 0x01) {
				SONG_FLAG = 1;
				state = playing;
			}
			else {
				PWM_off();
				state = waiting;
			}
			break;
		
		case playing:
			if(SONG_FLAG == 1) { 
				state = playing;
			}
			else if(SONG_FLAG == 0 && buttons == 0x00) {
				state = waiting;
			}
			else {
				state = playing;
			}
			break;
		
		default:
			state = waiting;
			break;
	}
	return state;
}

int playSong(int state)
{
	switch (state) {		
		case notPlaying:
			if(SONG_FLAG == 1) {
				state = music;
				PWM_on();
			} 
			else {
				state = 0;
			}
			break;
		
		case music: 
			set_PWM(notes[i]);
			state = play;
			break;
		
		case play:
			if(cnt <= held[i]) {
				state = play;
				cnt++;
			} 
			else if(cnt > held[i]) {
				state = chill;
				cnt = 0;
				PWM_off();
			} 
			else {
				state = play;
			}
			break;
		
		case chill:
			if(i >= 43-1) {
				state = notPlaying;
				PWM_off();
				SONG_FLAG = 0;
				i = 0;
			}
			else if(cnt <= pause[i]) {
				state = chill;
				cnt++;
			} 
			else if(cnt > pause[i]) {
				state = music;
				cnt = 0;
				i++;
				PWM_on();
			} 
			else {
				state = chill;
			}
			break;
		
		default:
			state = notPlaying;
		break;
	}
	return state;
}