/*
Ivan Zhao, Jason Garcia, Bogdan Tudos

This main function will be used to rapid prototype
a satellite communication system before uploading it
onto Arduinos for communications. Random generator 
given by our lord and savior professor Peckol.
*/

#include <stdio.h>
#include <stdlib.h>

#include "satComs.c"


void main(void)
{
	//comment the line below out if ur not compiling on windows
	setvbuf(stdout, 0, _IONBF, 0); 

	/*
	initialize ALL the variables for the structs as global
	*/
	int solarPanelState = 0; // FALSE = 0, TRUE = 1
	unsigned short batLevel = 100;
	unsigned short pwrCon = 0;
	unsigned short pwrGen = 0;
	unsigned int thusterCommand = 0;
	unsigned short fuelLevel = 100;
	int fuelLow = 0; // FALSE = 0, TRUE = 1
	int batLow = 0; // FALSE = 0, TRUE = 1

	/*
	initialize structs for all subsystems (task order tbd)
	*/
	powerSubsystemData* task0 = (powerSubsystemData*) 
		malloc(sizeof(powerSubsystemData));
	thrusterSubsystemData* task1 = (thrusterSubsystemData*) 
		malloc(sizeof(thrusterSubsystemData));
	satelliteComsData* task2 = (satelliteComsData*) 
		malloc(sizeof(satelliteComsData));
	consoleDisplayData* task3 = (consoleDisplayData*) 
		malloc(sizeof(consoleDisplayData));
	warningAlarmData* task4 = (warningAlarmData*) 
		malloc(sizeof(warningAlarmData));

	/*
	initialize struct for task control board (TCB)
	*/
	TCB* controller = (TCB*) malloc(sizeof(TCB));

	/*
	Assign all the subsystem struct variables
	*/

	//powerSubsystem variables
	task0->solarPanelState = &solarPanelState;
	task0->batLevel = &batLevel;
	task0->pwrCon = &pwrCon;
	task0->pwrGen = &pwrGen;

	//thrusterSubsystemData variables
	task1->thrusterCommand = &thusterCommand;
	task1->fuelLevel = & fuelLevel;

	//satelliteComsData variables
	task2->fuelLow = &fuelLow;
	task2->batLow = &batLow;
	task2->solarPanelState = &solarPanelState;
	task2->batLevel = &batLevel;
	task2->fuelLevel = &fuelLevel;
	task2->pwrCon = &pwrCon;
	task2->pwrGen = &pwrGen;
	task2->thrusterCommand = &thusterCommand;

	//consoleDisplayData variables
	task3->fuelLow = &fuelLow;
	task3->batLow = &batLow;
	task3->solarPanelState = &solarPanelState;
	task3->batLevel = &batLevel;
	task3->fuelLevel = &fuelLevel;
	task3->pwrCon = &pwrCon;
	task3->pwrGen = &pwrGen;

	//warningAlarmData variables
	task4->fuelLow = &fuelLow;
	task4->batLow = &batLow;
	task4->solarPanelState = &solarPanelState;
	task4->batLevel = &batLevel;
	task4->fuelLevel = &fuelLevel;

	controller->myTask = &powerSubsystem;
	controller->taskData = task0;
	controller->myTask(controller->taskData);
}