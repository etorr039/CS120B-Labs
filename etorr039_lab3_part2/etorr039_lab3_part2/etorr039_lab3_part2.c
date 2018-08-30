/*	Name: Edwin Torres etorr039@ucr.edu
 *	Partner(s) Name & E-mail: Cesar Monarrez cmona001@ucr.edu
 *	Lab Section: A21
 *	Assignment: Lab 3  Exercise 2 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

enum States {Start, Wait, Reset, Increment, Decrement} state;
unsigned char tmpA, tmpC;

void Tick()
{
	switch(state) { // mealy
		case Start : state = Wait; tmpC = 7;
		break; 
		
		case Wait : 
			if (tmpA  == 0x03){ 
				state = Reset; 
				tmpC = 0;
			}
			else if (tmpA == 0x02){ 
				state = Decrement; 
				if (tmpC > 0) tmpC--;
			}
			else if (tmpA  == 0x01){ 
				state = Increment;
				if (tmpC < 9) tmpC++;
			}
			else state = Wait; 
		break;
		
		case Reset : 
			if (tmpA == 0x00) state = Wait;
			else state = Reset;
		break;
		
		case Increment : 
			if (tmpA == 0x03){
				 state = Reset;
				 tmpC = 0;
			}
			else if (tmpA == 0x01) state = Increment;
			else state = Wait;
		break;
			
		case Decrement : 
			if (tmpA == 0x03){
				 state = Reset;
				 tmpC = 0;
			}
			else if (tmpA == 0x02) state = Decrement;
			else state = Wait;
		break;
		
		default : state = Start;
		break;
		
	}
	
	switch(state){
		case Start:
			break;
			
		case Wait:
			break;
		
		case Reset:
			break;
		
		case Increment:
			break;
			
		case Decrement:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	state = Start;
	
	while(1){
        tmpA = PINA & 0x03;
		Tick();
		PORTC = tmpC;
    }
	return 0;
}