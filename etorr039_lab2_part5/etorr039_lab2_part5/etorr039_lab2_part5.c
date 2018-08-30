/* Name: Edwin Torres
 * Partner(s) Name & Email: Cesar Monarrez cmona001@ucr.edu
 * Lab Section: A21
 * Assignment: Lab 2 Exercise 5
 * I Acknowledge all content contained herein, excluding template and example
 * code, is my own original work.
 */ 

#include <avr/io.h>

int main(void)
{
	DDRB = 0xFE; PORTB = 0x01; // configure PORT B's pin as such: 11111110
	DDRD = 0x00; PORTD = 0xFF; // configure PORT D's 8 bits as input
	
    while (1) 
    {
		unsigned char weight1 = PIND; // weight1 will encompass all PIND 8 bits
		unsigned short weight2 = 256; // weight2 be used to add the 9th bit
		unsigned short weighttot = 0x00; // initialize weightot to 0
		unsigned char tmp = 0x00; // initialize tmp to 0
		
		// if 1 is detected in "9th bit" place, add 256
		if ((PINB & 0x01) == 0x01) {
			weighttot = weight1 + weight2;
		}
		// if 0 detected in "9th bit" place, do not add 256
		else {
			weighttot = weight1;
		}
		// if weight exceeds 70 lbs, light up PB1
		if (weighttot >= 70) {
			tmp = 0x02;
		}
		// if weight in between 5 and 50, light up PB2
		if (weighttot > 5 && weighttot <70) {
			tmp = 0x04;
		}
		// if weight less than 5, do not light anything up
		if (weighttot < 5) {
			tmp = 0x00;
		}
	//output
	PORTB = tmp;
	
	}
	return 0;
}


