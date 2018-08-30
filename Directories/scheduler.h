
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef SCHEDULER_H
#define SCHEDULER_H

////////////////////////////////////////////////////////////////////////////////
//Struct for Tasks represent a running process in our simple real-time operating system
typedef struct _task{
	// Tasks should have members that include: state, period,
	//a measurement of elapsed time, and a function pointer.
	signed char state; 			//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;

#endif //SCHEDULER_H
