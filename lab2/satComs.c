#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "satComs.h"

//Used in random function for thruster command
int seed = 1;
//Used in powerManage function
int globalCounter, consumptionState = 0;

// Time Delay
void delay(int numSeconds)
{
	// Start time
	clock_t start_time = clock();

	// looping while required time is not acheived
	while (clock() < start_time + (numSeconds * CLOCKS_PER_SEC))
		;
}

void powerSubsystem(void *task)
{
	powerSubsystemData *task0 = (powerSubsystemData *)task;

	powerManage(task0, 1); //powerConsumption = 1

	//powerGeneration and solarPanelState
	if (*task0->solarPanelState == 1)
	{
		if (*task0->batLevel > 95)
		{
			*task0->solarPanelState = 0;
		}
		else
		{
			powerManage(task0, 0); //powerGeneration = 0
		}
	}
	else
	{
		if (*task0->batLevel <= 10)
		{
			*task0->solarPanelState = 1;
		}
	}

	//computes battery level
	if (*task0->solarPanelState == 0)
	{
		*task0->batLevel -= (3 * (*task0->pwrCon));
	}
	else
	{
		*task0->batLevel -= (*task0->pwrCon + *task0->pwrGen);
	}
}

void powerManage(powerSubsystemData *task, int i)
{
	switch (i)
	{
	case 1: //powerConsumption
		switch (consumptionState)
		{
		case 0: //Normal power consumption
			if (globalCounter == 0 || globalCounter % 2 == 0)
			{
				*task->pwrCon += 2;
			}
			else
			{
				*task->pwrCon -= 1;
			}
			if (*task->pwrCon > 10)
				;
			consumptionState = 1;
		case 1: //Reverse power consumption
			if (globalCounter == 0 || globalCounter % 2 == 0)
			{
				*task->pwrCon -= 2;
			}
			else
			{
				*task->pwrCon += 1;
			}
			if (*task->pwrCon < 5)
				;
			consumptionState = 0;
		}
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
			if (globalCounter == 0 || globalCounter % 2 == 0)
				;
			*task->pwrGen += 2;
		}
	}
}

void thrusterSubsystem(void *task)
{
	thrusterSubsystemData *task1 = (thrusterSubsystemData *)task;
	if (task1->thrusterCommand == NULL)
	{
		return;
	}

	unsigned short left = (unsigned short)task1->thrusterCommand[0];
	unsigned short right = (unsigned short)task1->thrusterCommand[1];
	unsigned short up = (unsigned short)task1->thrusterCommand[2];
	unsigned short down = (unsigned short)task1->thrusterCommand[3];

	//questionable if we even need to use magnitude and variable
	unsigned int magnitude[4] = {
		task1->thrusterCommand[4],
		task1->thrusterCommand[5],
		task1->thrusterCommand[6],
		task1->thrusterCommand[7]};

	int magBool;
	if ((magnitude[0] & magnitude[1]) == (magnitude[2] & magnitude[3]))
	{
		if ((magnitude[0] & magnitude[1]) == 1)
		{
			magBool = 100;
		}
		else
		{
			magBool = 0;
		}
	}
	else
	{
		magBool = 50;
	}

	unsigned int duration[8] = {
		task1->thrusterCommand[8],
		task1->thrusterCommand[9],
		task1->thrusterCommand[10],
		task1->thrusterCommand[11],
		task1->thrusterCommand[12],
		task1->thrusterCommand[13],
		task1->thrusterCommand[14],
		task1->thrusterCommand[15]};

	int sum = 0;
	for (int i = 0; i < 8; i++)
	{
		int binary = duration[i];
		sum += ((int)pow(2, i)) * binary;
	}

	printf("%d\n", sum); // Decimal value of the thurster duration

	//Decreases fuel level is thrusters are being used
	int thrustBool = left & right & up & down;
	if (thrustBool)
	{
		task1->fuelLevel -= 5;
		printf("The thrusters are firing at %d\n", magBool);
		//printf("Time until done moving: %d sec\n", duration);
	}
}

void satelliteComs(void *task)
{
	satelliteComsData *task2 = (satelliteComsData *)task;
	unsigned int command[16];
	int rand = randomInteger(-10, 10);

	//converts decimal into binary array
	for (int i = 15; i >= 0; i--)
	{
		command[i] = (rand >> i) & 1;
	}

	task2->thrusterCommand = command;
}

void consoleDisplay(void *task)
{
	//Normal satellite status
	consoleDisplayData *task3 = (consoleDisplayData *)task;
	printf("BATTERY LEVEL: \t%d\n", *task3->batLevel);
	printf("FUEL LEVEL: \t%d\n", *task3->fuelLevel);
	printf("POWER CONSUMPTION: \t%d\n", *task3->pwrCon);
	printf("POWER GENERATION: \t%d\n", *task3->pwrGen);
	printf("CHARGING STATUS: \t");
	if (task3->solarPanelState)
	{
		printf("ON\n");
	}
	else
	{
		printf("OFF\n");
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
	double max = INT_MAX + 1.0;

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
