#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "satComs.h"

//Used in random function for thruster command
int seed = 1;
//Used in powerManage function
int globalCounter, consumptionState = 0;

void schedule(powerSubsystemData* task0,
				thrusterSubsystemData* task1,
				satelliteComsData* task2,
				consoleDisplayData* task3, 
				warningAlarmData* task4,
				TCB* controller) {
	//Task Queue

	//power system
	controller->taskData = (void*) task0;
	powerSubsystem(controller->taskData);
	delay(1);

	//thruster system
	controller->taskData = (void*) task1;
	thrusterSubsystem(controller->taskData);
	delay(1);

	//sattelite coms
	controller->taskData = (void*) task2;
	satelliteComs(controller->taskData);
	delay(1);

	//consoleDisplay
	controller->taskData = (void*) task3;
	consoleDisplay(controller->taskData);
	controller->taskData = (void*) task4;
	WarningAlarm(controller->taskData);
	delay(1);

	globalCounter++;
}

// Time Delay
void delay(int numSeconds)
{
	// Start time
	clock_t start_time = clock();

	// looping while required time is not acheived
	while (clock() < start_time + (numSeconds * CLOCKS_PER_SEC));
}

void powerSubsystem(void *task)
{
	powerSubsystemData *task0 = (powerSubsystemData *)task;

	powerManage((void*) task0, 1); //powerConsumption = 1

	//powerGeneration and solarPanelState
	if (*task0->solarPanelState == 1)
	{
		if ((int)*task0->batLevel > 95)
		{
			*task0->solarPanelState = 0;
			*task0->pwrGen = 0;
		}
		else
		{
			powerManage((void*) task0, 0); //powerGeneration = 0
		}
	}
	else
	{
		if ((int)*task0->batLevel <= 10)
		{
			*task0->solarPanelState = 1;
		}
	}

	//computes battery level
	if (*task0->solarPanelState == 0) {
		if((int)(*task0->batLevel - (3 * (*task0->pwrCon))) < 0) {
			*task0->batLevel = 0;
		} else {
			*task0->batLevel -= (3 * (*task0->pwrCon));
		}
			
	} else {
		if((int)(*task0->batLevel - *task0->pwrCon + *task0->pwrGen) < 0) {
			*task0->batLevel = 0;
		} else {
			*task0->batLevel = *task0->batLevel - *task0->pwrCon + *task0->pwrGen;
		}
		
	}
}

void powerManage(void *task0, int i)
{
	powerSubsystemData *task = (powerSubsystemData*) task0;
	switch (i)
	{
	case 1: //powerConsumption
		switch (consumptionState)
		{
		
		case 1: //Reverse power consumption
			if (globalCounter == 0 || globalCounter % 2 == 0)
			{
				*task->pwrCon = *task->pwrCon - 2;
			}
			else
			{
				*task->pwrCon = *task->pwrCon + 1;
			}
			if (*task->pwrCon < 5) {
				consumptionState = 0;
			}
			break;
		case 0: //Normal power consumption
			if (globalCounter == 0 || globalCounter % 2 == 0)
			{
				*task->pwrCon = *task->pwrCon + 2;
			}
			else
			{
				*task->pwrCon = *task->pwrCon - 1;
			}
			if (*task->pwrCon > 10)
			{
				consumptionState = 1;
			}
			break;
		}
		break;
	case 0: //powerGeneration
		if (*task->batLevel < 50)
		{
			if (globalCounter == 0 || globalCounter % 2 == 0)
			{
				*task->pwrGen += 2;
			}
			else
			{
				*task->pwrGen += 1;
			}
		}
		else if (*task->batLevel > 50 && *task->batLevel < 95)
		{
			if (globalCounter == 0 || globalCounter % 2 == 0);
				*task->pwrGen += 2;
		}
		break;
	}
}

void thrusterSubsystem(void *task)
{
	thrusterSubsystemData *task1 = (thrusterSubsystemData *)task;
	unsigned int* command = convertDtoB(*task1->thrusterCommand);

	unsigned short left = (unsigned short) command[0];
	unsigned short right = (unsigned short) command[1];
	unsigned short up = (unsigned short) command[2];
	unsigned short down = (unsigned short) command[3];
	int direction = left | right | up | down;
	//questionable if we even need to use magnitude and variable
	unsigned int magnitude[4] = {
		command[4],
		command[5],
		command[6],
		command[7]};

	unsigned int magBool = convertBtoD(magnitude, 4);
	unsigned int mag = 0;

	unsigned int duration[8] = {
		command[8],
		command[9],
		command[10],
		command[11],
		command[12],
		command[13],
		command[14],
		command[15]};
	unsigned int sum = convertBtoD(duration, 8);

	if(direction && sum > 0 && (int) *task1->fuelLevel > 0) {
			if(magBool == 14) {
				mag = 100;
				if((int)*task1->fuelLevel - 10 < 0) {
					*task1->fuelLevel = 0;
				} else {
					*task1->fuelLevel -= 10;
				}
			} else if(magBool == 0) {
				mag = 0;
			} else {
				mag = 50;
				if((int)*task1->fuelLevel - 5 < 0) {
					*task1->fuelLevel = 0;
				} else {
					*task1->fuelLevel -= 5;
				}
			}
		} else {
			mag = 0;
		}
	
	printf("DURATION: \t%u ms\n", sum);
	printf("THRUSTERS: \t%d\n", mag);
}

void satelliteComs(void *task)
{
	satelliteComsData *task2 = (satelliteComsData *)task;
	*task2->thrusterCommand = (unsigned int) randomInteger(-10, 10);
}

unsigned int* convertDtoB(int dec) {
	unsigned int* command = malloc(sizeof(int) * 100);
	int i;
	for (i = 15; i >= 0; i--)
		{
			command[i] = (dec >> i) & 1;
		}
	return command;
}

unsigned int convertBtoD(unsigned int* bits, int length) {
	unsigned int sum;
	int i;
	for (i = 0; i < length; i++){
			int binary = bits[i];
			sum += ((int)pow(2, i)) * binary;
	}

	return sum;
}

void consoleDisplay(void *task)
{
	//Normal satellite status
	consoleDisplayData *task3 = (consoleDisplayData *)task;
	printf("BATTERY LEVEL: \t");
	if(*task3->batLevel > 100 || *task3->batLevel < 0) {
		printf("0\n");
	} else {
		printf("%d\n", *task3->batLevel);
	}
	printf("FUEL LEVEL: \t%d\n", *task3->fuelLevel);
	printf("POWER CONSUMPTION: \t%d\n", *task3->pwrCon);
	printf("POWER GENERATION: \t%d\n", *task3->pwrGen);
	printf("CHARGING STATUS: \t");
	if (*task3->solarPanelState)
	{
		printf("ON\n\n");
	}
	else
	{
		printf("OFF\n\n");
	}

	//Annunciation mode
	if (*task3->batLevel < 50)
	{
		*task3->batLow = 1;
	}
	else
	{
		*task3->batLow = 0;
	}
	if (*task3->fuelLevel < 50)
	{
		*task3->fuelLow = 1;
	}
	else
	{
		*task3->fuelLow = 0;
	}
}

void WarningAlarm(void *task)
{
	warningAlarmData *task4 = (warningAlarmData *)task;
}

int randomInteger(int low, int high)
{
	double randNum = 0.0;
	int multiplier = 2743;
	int addOn = 5923;
	double max = 214783647 + 1.0;

	int retVal = 0;

	if (low > high)
		retVal = randomInteger(high, low);
	else
	{
		seed = seed * multiplier + addOn;
		randNum = seed;

		if (randNum < 0)
		{
			randNum = randNum + max;
		}

		randNum = randNum / max;

		retVal = ((int)((high - low + 1) * randNum)) + low;
	}

	return retVal;
}
