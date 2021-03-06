/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab hit detection 1 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 5; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 1; // GCD of all tasks
task tasks[5]; // create array of tasks

#include "timer.h"

#define SHOOT (~PINC & 0x01)

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

// === Define Tasks ===
enum move_States {moveStart, wait, up, down, left, right, unPress};
int move_Tick(int state);

enum reGen_States {checkFlag, reGenLeft, reGenRight};
int reGen_Tick(int state);

void shiftReg(unsigned short data);

enum shoot_States {noShot, shot};
int shoot_Tick(int state);

enum inv_States {invStart, invRight, invLeft, invDown};
int inv_Tick(int state);

enum updateMATRIX_States {updateMATRIX};
int updateMATRIX_Tick(int state);

void collision();

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
			
char shotX = 0b00001000;	

char shotY = 0b01111111;
				
char invX[8] = {	0b00000001,
					0b00000010,
					0b00000100,
					0b00001000,
					0b00010000,
					0b00100000,
					0b01000000,
					0b10000000,
				};

char invY[8] = {	0b11111110,
					0b11111101,
					0b11111110,
					0b11111101,
					0b11111110,
					0b11111101,
					0b11111111,
					0b11111111,
				};
	
			
unsigned short MATRIX;
unsigned char reGenLeft_FLAG = 0;
unsigned char reGenRight_FLAG = 0;
unsigned char hit_FLAG = 0;
unsigned char shoot_FLAG = 0;
unsigned char inv_FLAG = 0;
unsigned char position;
unsigned char x = 0;
unsigned char pos = 0;

// === Main Function ===
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; 
	DDRC = 0x00; PORTC = 0xFF;
	
	ADC_init();
	
	unsigned char j = 0;

	tasks[j].state = moveStart; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &move_Tick; // function pointer
	j++;
	tasks[j].state = checkFlag; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &reGen_Tick; // function pointer
	j++;
	tasks[j].state = noShot; // initial state
	tasks[j].period = 20; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &shoot_Tick; // function pointer
	j++;
	tasks[j].state = invStart; // initial state
	tasks[j].period = 800; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &inv_Tick; // function pointer
	j++;
	tasks[j].state = updateMATRIX; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &updateMATRIX_Tick; // function pointer

	TimerSet(tasksPeriodGCD); // set system timer
	TimerOn(); // start system timer
	
	while(1) {} 
}

void collision() {
	
	/*unsigned char i = 0;
	for (i = 0; i < 8; i++) {
		if (shotY == invY[i]) {
			invY[i] = 0xFF;
			shotY = 0xFF;
		}
	}*/
			if (shotY == invY[pos] && pos == 0) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}
			else if (shotY == invY[pos] && pos == 1) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}
			else if (shotY == invY[pos] && pos == 2) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}
			else if (shotY == invY[pos] && pos == 3) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}
			else if (shotY == invY[pos] && pos == 4) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}
			else if (shotY == invY[pos] && pos == 5) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}
			else if (shotY == invY[pos] && pos == 6) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}
			else if (shotY == invY[pos] && pos == 7) {
				invY[pos] = 0xFF;
				shotY = 0xFF;
			}

}

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
		PORTB = 0x08;
		PORTB |= ((data >> i) & 0x01);
		PORTB |= 0x02;
	}
	PORTB |= 0x04;
	PORTB = 0x00;
}

int shoot_Tick(int state) {
	
	switch(state) {
		case noShot:
			shoot_FLAG = 0;
			if (!SHOOT) {
				state = noShot;
				shotY = 0b11111111;
			}
			else if (SHOOT) {
				state = shot;
				pos = position;
				shotY = shotY - 0b10000000;
			}
			break;
			
		case shot:
			if (pos == 0) {
				shotX = 0b00000001;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					x++;
					state = shot;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			else if (pos == 1) {
				shotX = 0b00000010;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					x++;
					state = shot;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			else if (pos == 2) {
				shotX = 0b00000100;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					x++;
					state = shot;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			else if (pos == 3) {
				shotX = 0b00001000;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					state = shot;
					x++;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			else if (pos == 4) {
				shotX = 0b00010000;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					x++;
					state = shot;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			else if (pos == 5) {
				shotX = 0b00100000;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					x++;
					state = shot;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			else if (pos == 6) {
				shotX = 0b01000000;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					x++;
					state = shot;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			else if (pos == 7) {
				shotX = 0b10000000;
				shoot_FLAG = 1;
				if (x <= 7) {
					collision();
					shotY = shotY >> 1;
					shotY = shotY + 0x80;
					x++;
					state = shot;
				}
				else if (x == 8) {
					if (!SHOOT) {
						x = 0;
						state = noShot;
					}
				}
			}
			break;
	}
	return state;
}

int inv_Tick(int state) {
	
	static unsigned char i = 0;
	static unsigned char j = 0;
	static unsigned char k = 0;
	static unsigned char invLeft_FLAG = 0;
	static unsigned char invRight_FLAG = 0;
	
	switch(state) {
		case invStart:
			state = invRight;
			break;
			
		case invRight:
			if (j < 1) {
				state = invRight;
				j++;
				//for (i = 0; i < 8; i++) {
				//	invX[i] = invX[i] << 1;
				//}
				invY[6] = invY[5];
				invY[5] = invY[4];
				invY[4] = invY[3];
				invY[3] = invY[2];
				invY[2] = invY[1];
				invY[1] = invY[0];
				invY[0] = 0xFF;
				pos = pos + 1;
			}
			else if (j < 2) {
				state = invRight;
				j++;
				invY[7] = invY[6];
				invY[6] = invY[5];
				invY[5] = invY[4];
				invY[4] = invY[3];
				invY[3] = invY[2];
				invY[2] = invY[1];
				invY[1] = 0xFF;
				pos = pos + 1;
			}
			else if (j == 2) {
				state = invDown;
				invLeft_FLAG = 1;
				j = 0;
			}
			break;
			
		case invLeft:	
			if (k < 1) {
				state = invLeft;
				k++;
				//for (i = 0; i < 8; i++) {
				//	invX[i] = invX[i] >> 1;
				//}
				invY[1] = invY[2];
				invY[2] = invY[3];
				invY[3] = invY[4];
				invY[4] = invY[5];
				invY[5] = invY[6];
				invY[6] = invY[7];
				invY[7] = 0xFF;
			}
			else if (k < 2) {
				state = invLeft;
				k++;
				invY[0] = invY[1];
				invY[1] = invY[2];
				invY[2] = invY[3];
				invY[3] = invY[4];
				invY[4] = invY[5];
				invY[5] = invY[6];
				invY[6] = 0xFF;
			}
			else if (k == 2) {
				state = invDown;
				invRight_FLAG = 1;
				k = 0;
			}
			break;
			
		case invDown:
			for (i = 0; i < 8; i++) {
				invY[i] = ~invY[i];
				invY[i] = invY[i] << 1;
				invY[i] = ~invY[i];
			}	
			if (invRight_FLAG == 1) {
				state = invRight;
				invRight_FLAG = 0;
			}
			else if (invLeft_FLAG == 1) {
				state = invLeft;
				invRight_FLAG = 0;
			}
			break;	
	}
	return state;
}

int updateMATRIX_Tick(int state) {
	
	unsigned char i;
	
	switch(state){
		case updateMATRIX:
			for (i = 0; i < 8; i++) {
				MATRIX = shipX[i] << 8;
				MATRIX = MATRIX + shipY[i];
				shiftReg(MATRIX);
				if (i > 7) {
					i = 0;
				}
			}
			
			for (i = 0; i < 8; i++) {
				MATRIX = invX[i] << 8;
				MATRIX = MATRIX + invY[i];
				shiftReg(MATRIX);
			}
			
			MATRIX = shotX << 8;
			MATRIX = MATRIX + shotY;
			shiftReg(MATRIX);
			
			state = updateMATRIX;
			break;
	}
	return state;
}