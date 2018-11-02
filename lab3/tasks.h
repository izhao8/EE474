#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "satComs.h"

//Used in random function for thruster command
int seed = 1;
//Used in powerManage function
int globalCounter, consumptionState, panelActive = 0;
unsigned int timeMilli, timeMicro = 0;

TCB *head = (TCB *)malloc(sizeof(TCB));
TCB *tail = (TCB *)malloc(sizeof(TCB));

void powerManage(void *task0, int i);
unsigned int convertBtoD(unsigned int *bits, int length);
unsigned int *convertDtoB(int dec);
int randomInteger(int low, int high);
void insert(TCB *node);
double batteryBuffer(int battery);
void consoleKeyPad(void *task);
void solarPanelControl(void *task);
void powerSubsystem(void *task);
void deleteNode(TCB *node);

/*
initialize structs for task control board (TCB)
// */
TCB *power = (TCB *)malloc(sizeof(TCB));
TCB *thruster = (TCB *)malloc(sizeof(TCB));
TCB *satellite = (TCB *)malloc(sizeof(TCB));
TCB *console = (TCB *)malloc(sizeof(TCB));
TCB *warning = (TCB *)malloc(sizeof(TCB));
TCB *solarPanel = (TCB *)malloc(sizeof(TCB));
TCB *vehicle = (TCB *)malloc(sizeof(TCB));
TCB *keypad = (TCB *)malloc(sizeof(TCB));

void start() 
{
/*	head->next = power;
	power->next = thruster;
	thruster->next = satellite;
	satellite->next = console;
	console->next = warning;
	warning->next = solarPanel;
	solarPanel->next = vehicle;
	vehicle->next = keypad;

	power->prev = head;
	thruster->prev = power;
	satellite->prev = thruster;
	console->prev = satellite;
	warning->prev = console;
	solarPanel->prev = warning;
	vehicle->prev = solarPanel;
	keypad->prev = vehicle;
*/
	insert(power);
	insert(thruster);
	insert(satellite);
	insert(console);
	insert(warning);
	timeMicro = micros();
	timeMilli = millis();
}

void solarPanelControl(void *task)
{
	solarPanelControlData *task0 = (solarPanelControlData *)task;

	if (task0->motorSpeedInc)
	{
		if ((unsigned int)*task0->motorDrive == 255)
		{
			*task0->motorDrive = 0;
			*task0->solarPanelState = 1; // fully deployed interupt signal
		}
		*task0->deploy = 1;
		*task0->retract = 0;
		*task0->motorDrive += 12.75; // 12.75 is 5% of 255
		analogWrite(*task0->motorDrive, 13); // pin 13 is PWM
	}
	else if (*task0->motorSpeedDec)
	{
		if ((unsigned int)*task0->motorDrive == 0)
		{
			*task0->solarPanelState = 0; // fully retracted interupt signal 
		}
		*task0->deploy = 0;
		*task0->retract = 1;
		*task0->motorDrive -= 12.75;
		analogWrite(*task0->motorDrive, 13);
	}
}

void consoleKeyPad(void *task)
{
	consoleKeypadData *task0 = (consoleKeypadData *)task;
	int motorSpeed = 50;
	char press = 0;

	// Need to figure out a good way of toggleing this block on and off while also keeping track of the
	// motorSpeed value...
	while (panelActive)
	{
		scanf(" %c", &press);

		// letter "I" is pressed
		if (press == 105)
		{
			if ((0 <= motorSpeed) && (motorSpeed < 100))
			{
				*task0->motorSpeedInc = 1;
				motorSpeed += 5;
			}
			else if (motorSpeed >= 100)
			{
				*task0->motorSpeedInc = 0;
				motorSpeed = 100;
			}
		}
		// letter "D" is pressed
		else if (press == 100)
		{
			if ((0 < motorSpeed) && (motorSpeed <= 100))
			{
				*task0->motorSpeedDec = 1;
				motorSpeed -= 5;
			}
			else if (motorSpeed <= 0)
			{
				*task0->motorSpeedDec = 0;
				motorSpeed = 0;
			}
		}
	}
}

void powerSubsystem(void *task)
{
	powerSubsystemData *task0 = (powerSubsystemData *)task;

	powerManage((void *)task0, 1); //powerConsumption = 1

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
			powerManage((void *)task0, 0); //powerGeneration = 0
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
	if (*task0->solarPanelState == 0)
	{
		if ((int)(*task0->batLevel - (3 * (*task0->pwrCon))) < 0)
		{
			*task0->batLevel = 0;
		}
		else
		{
			*task0->batLevel -= (3 * (*task0->pwrCon));
		}
	}
	else
	{
		if ((int)(*task0->batLevel - *task0->pwrCon + *task0->pwrGen) < 0)
		{
			*task0->batLevel = 0;
		}
		else
		{
			*task0->batLevel = *task0->batLevel - *task0->pwrCon + *task0->pwrGen;
		}
	}
}

void powerManage(void *task0, int i)
{
	powerSubsystemData *task = (powerSubsystemData *)task0;
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
			if (*task->pwrCon < 5)
			{
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
			if (globalCounter == 0 || globalCounter % 2 == 0)
				;
			*task->pwrGen += 2;
		}
		break;
	}
}

void thrusterSubsystem(void *task)
{
	thrusterSubsystemData *task1 = (thrusterSubsystemData *)task;
	unsigned int *command = convertDtoB(*task1->thrusterCommand);

	unsigned short left = (unsigned short)command[0];
	unsigned short right = (unsigned short)command[1];
	unsigned short up = (unsigned short)command[2];
	unsigned short down = (unsigned short)command[3];
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

	if (direction && sum > 0 && (int)*task1->fuelLevel > 0)
	{
		if (magBool == 14)
		{
			mag = 100;
			if ((int)*task1->fuelLevel - 5 < 0)
			{
				*task1->fuelLevel = 0;
			}
			else
			{
				*task1->fuelLevel -= 5;
			}
		}
		else if (magBool == 0)
		{
			mag = 0;
		}
		else
		{
			mag = 50;
			if ((int)*task1->fuelLevel - 5 < 0)
			{
				*task1->fuelLevel = 0;
			}
			else
			{
				*task1->fuelLevel -= 5;
			}
		}
	}
	else
	{
		mag = 0;
	}
}

void satelliteComs(void *task)
{
	satelliteComsData *task2 = (satelliteComsData *)task;
	*task2->thrusterCommand = (unsigned int)randomInteger(-10, 10);
}

unsigned int *convertDtoB(int dec)
{
	unsigned int *command = malloc(sizeof(unsigned int) * 100);

	for (int i = 15; i >= 0; i--)
	{
		command[i] = (dec >> i) & 1;
	}
	return command;
}

unsigned int convertBtoD(unsigned int *bits, int length)
{
	unsigned int sum = 0;
	for (int i = 0; i < length; i++)
	{
		int binary = bits[i];
		sum += ((int)pow(2, i)) * binary;
	}

	return sum;
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

void insert(TCB *node)
{
	if (node == NULL)
	{
		head = node;
		tail = node;
	}
	else
	{
		tail->next = node;
		node->prev = tail;
		tail = node;
	}
	return;
}

void deleteNode(TCB *node)
{
	if (node == NULL || head == NULL)
	{
		return;
	}
	TCB *current = head;
	while (current->taskData != node->taskData && current != NULL)
	{
		current = current->next;
	}
	if (current == NULL)
	{
		return;
	}

	if (current == head)
	{
		head = head->next;
		head->prev = NULL;
	}
	else if (current == tail)
	{
		tail = current->prev;
		tail->next = NULL;
	}
	else if (head == tail)
	{
		head = NULL;
		tail = NULL;
	}
	else
	{
		current->prev->next = current->next;
		current->next->prev = current->prev;
	}
	//formally deletes the node from memory
	free(current);
}

double batteryBuffer(int battery)
{
	double newBat = battery * 5;
	newBat = floor(newBat * 7.2);
	return newBat;
}