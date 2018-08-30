/*	Name: Edwin Torres etorr039@ucr.edu
 *	Lab Section: B21
 *	Assignment: Custom Lab Final Game
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "scheduler.h"

const unsigned char tasksNum = 8; // number of tasks (# of state machines)
const unsigned long tasksPeriodGCD = 1; // GCD of all tasks
task tasks[8]; // create array of tasks

#include "timer.h"
#include "io.c"

#define SHOOT (~PINA & 0x20)

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

// === Define Tasks ===
enum move_States {moveStart, wait, up, down, left, right, unPress};
int move_Tick(int state);

enum reGen_States {checkFlag, reGenLeft, reGenRight};
int reGen_Tick(int state);

enum shoot_States {noShot, shot};
int shoot_Tick(int state);

enum inv_States {invStart, invRight, invLeft, invDown};
int inv_Tick(int state);

enum updateMATRIX_States {updateMATRIX};
int updateMATRIX_Tick(int state);

enum win_States {playing1, win, nextLVL};
int win_Tick(int state);

enum lose_States {playing2, lose, gameOver};
int lose_Tick(int state);

enum score_States {update};
int score_Tick(int state);

// === Define Functions ===

void shiftReg(unsigned short data);
void collision();
void winMESG();
void loseMESG();

// === Global Variables ===

// Player ship art
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
			
// Player shot art
char shotX = 0b00001000;	

char shotY = 0b01111111;

// Enemies art	
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
					0b11111110,
					0b11111110,
					0b11111110,
					0b11111110,
					0b11111110,
					0b11111111,
					0b11111111,
				};
			
unsigned short MATRIX;
unsigned char reGenLeft_FLAG = 0;
unsigned char reGenRight_FLAG = 0;
unsigned char START_FLAG = 0;
unsigned char position;
unsigned char x = 0;
unsigned char pos = 0;
unsigned char LEVEL = 0;
unsigned char SCORE = 0;

// === Main Function ===
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Input from joystick and button
	DDRB = 0xFF; PORTB = 0x00; // Output to shift registers and LED matrix
	DDRC = 0xFF; PORTC = 0x00; // Output to LCD screen
	DDRD = 0xFF; PORTD = 0x00; // Output to LCD screen
	
	ADC_init(); // initialize Analog to Digital Conversion
	LCD_init(); // initialize LCD screen 
	
	// "SCORE" is displayed on LCD
	LCD_ClearScreen();
	LCD_DisplayString(1,"SCORE:");
	LCD_Cursor(17);
	
	unsigned char j = 0;

	tasks[j].state = moveStart; // initial state
	tasks[j].period = 1; // state period
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &move_Tick; // function pointer
	j++;
	tasks[j].state = checkFlag;
	tasks[j].period = 1; 
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &reGen_Tick; 
	j++;
	tasks[j].state = noShot; 
	tasks[j].period = 20; 
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &shoot_Tick; 
	j++;
	tasks[j].state = invStart; 
	tasks[j].period = 200; 
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &inv_Tick; 
	j++;
	tasks[j].state = updateMATRIX;
	tasks[j].period = 1; 
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &updateMATRIX_Tick; 
	j++;
	tasks[j].state = playing1; 
	tasks[j].period = 100;
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &win_Tick; 
	j++;
	tasks[j].state = playing2; 
	tasks[j].period = 100; 
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &lose_Tick; 
	j++;
	tasks[j].state = update; 
	tasks[j].period = 250; 
	tasks[j].elapsedTime = tasks[j].period;
	tasks[j].TickFct = &score_Tick;

	TimerSet(tasksPeriodGCD); // Set system timer
	TimerOn(); // Start system timer
	
	while(1) {} // Infinite loop
}

// === Task Definitions ===

int lose_Tick(int state) {
	switch(state) {
		case playing2:
			if (	(invY[0] == 0b01111111 || invY[1] == 0b01111111 || invY[2] == 0b01111111 ||
					invY[3]	== 0b01111111 || invY[4] == 0b01111111 || invY[5] == 0b01111111 ||
					invY[6]	== 0b01111111 || invY[7] == 0b01111111) && START_FLAG == 0) {
					state = lose;
			}
			break;
		
		case lose:
			loseMESG();
			tasks[3].period = 99999;
			state = gameOver;
			break;
			
		case gameOver:
			
			state = playing2;
			break;
	}
	return state;
}

int win_Tick(int state) {
	
	switch(state) {
		
		case playing1:
			if (invY[0] == 0xFF && invY[1] == 0xFF && invY[2] == 0xFF && 
				invY[3] == 0xFF && invY[4] == 0xFF && invY[5] == 0xFF && 
				invY[6] == 0xFF && invY[7] == 0xFF ) {
				state = win;
				LEVEL++;
			}
			else {
				state = playing1;
			}
			break;
			
		case win:
			winMESG();
			tasks[3].period = 1000;
			tasks[5].period = 1000;
			START_FLAG = 1;	
			state = nextLVL;
			break;
			
		case nextLVL:
			tasks[3].period = 200;
			tasks[5].period = 100;
			if(LEVEL == 0) {
				invY[0] = 0b11111110;
				invY[1]	= 0b11111110;
				invY[2]	= 0b11111110;
				invY[3]	= 0b11111110;
				invY[4]	= 0b11111110;
				invY[5]	= 0b11111110;
				invY[6]	= 0b11111111;
				invY[7]	= 0b11111111;
			}
			else if(LEVEL == 1) {
				invY[0] = 0b11111110;
				invY[1]	= 0b11111011;
				invY[2]	= 0b11111110;
				invY[3]	= 0b11111011;
				invY[4]	= 0b11111110;
				invY[5]	= 0b11111011;
				invY[6]	= 0b11111111;
				invY[7]	= 0b11111111;
			}
			else if(LEVEL == 2) {
				invY[0] = 0b11111110;
				invY[1]	= 0b11111101;
				invY[2]	= 0b11111011;
				invY[3]	= 0b11111011;
				invY[4]	= 0b11111101;
				invY[5]	= 0b11111110;
				invY[6]	= 0b11111111;
				invY[7]	= 0b11111111;
				LEVEL = -1;
			}
			START_FLAG = 1;
			state = playing1;
			break;
	}
	return state;
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

int shoot_Tick(int state) {
	
	switch(state) {
		case noShot:
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
			i = 0;
			j = 0;
			k = 0;
			invLeft_FLAG = 0;
			invRight_FLAG = 0;
			START_FLAG = 0;
			break;
			
		case invRight:
			if (START_FLAG == 1) {
				state = invStart;
			}
			else if (j < 1) {
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
			if (START_FLAG == 1) {
				state = invStart;
			}
			else if (k < 1) {
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
			if (START_FLAG == 1) {
				state = invStart;
			}
			for (i = 0; i < 8; i++) {
				invY[i] = ~invY[i];
				invY[i] = invY[i] << 1;
				invY[i] = ~invY[i];
			}	
			if (START_FLAG == 1) {
				state = invStart;
			}
			else if (invRight_FLAG == 1) {
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

int score_Tick(int state) {
	
	static unsigned char i = 0; 
	
	switch(state) {
		case update:
			if (SCORE <= 20 && i == 0) {
				LCD_Cursor(17);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(1 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 1) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(2 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 2) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(3 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 3) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(4 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 4) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(5 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 5) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(6 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 6) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(7 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 7) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(8 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}
			else if (SCORE <= 20 && i == 8) {
				LCD_Cursor(18);
				LCD_WriteData(SCORE + '0');
				LCD_Cursor(21);
				if (SCORE >= 10) {
					LCD_Cursor(17);
					LCD_WriteData(9 + '0');
					LCD_Cursor(21);
					SCORE = 0;
					i++;
				}
			}

			state = update;
			break;
	}
	return state;
}

void collision() {
	
	if (shotY == invY[pos] && pos == 0) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
	else if (shotY == invY[pos] && pos == 1) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
	else if (shotY == invY[pos] && pos == 2) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
	else if (shotY == invY[pos] && pos == 3) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
	else if (shotY == invY[pos] && pos == 4) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
	else if (shotY == invY[pos] && pos == 5) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
	else if (shotY == invY[pos] && pos == 6) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
	else if (shotY == invY[pos] && pos == 7) {
		invY[pos] = 0xFF;
		shotY = 0xFF;
		SCORE += 1;
	}
}

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

void winMESG() {
	invY[0] = 0b11111000;
	invY[1]	= 0b11111001;
	invY[2]	= 0b11111000;
	invY[3]	= 0b11100011;
	invY[4]	= 0b00001111;
	invY[5]	= 0b11011111;
	invY[6]	= 0b10111111;
	invY[7]	= 0b00001111;
}

void loseMESG() {
	invY[0] = 0b00000000;
	invY[1]	= 0b00000000;
	invY[2]	= 0b00000000;
	invY[3]	= 0b00111100;
	invY[4]	= 0b00100000;
	invY[5]	= 0b00000000;
	invY[6]	= 0b00000000;
	invY[7]	= 0b00000000;
}