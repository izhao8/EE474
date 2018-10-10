#include <stdio.h>
#include <stdlib.h>

#include "satComs.h"

//Used in random function for thruster command
int seed = 1;
//Used in powerManage function 
int count, consumptionState = 0;

void powerSubsystem(void* task) 
{
	powerSubsystemData* task0 = (powerSubsystemData*) task;
	count++;

	powerManage(task0, 1); //powerConsumption = 1

	//powerGeneration
	if(*task0->solarPanelState == 1) {
		if(*task0->batLevel > 95) {
			*task0->solarPanelState = 0;
		} else {
			powerManage(task0, 0); //powerGeneration = 1
		}
	} else {
		if(*task0->batLevel <= 10) {
			*task0->solarPanelState = 1;
		}
	}

	//computes battery level
	if(*task0->solarPanelState == 0) {
		*task0->batLevel = (*task0->batLevel) - 3 * (*task0->pwrCon);
	} else {
		*task0->batLevel = (*task0->batLevel) - (*task0->pwrCon) + (*task0->pwrGen);
	}
}

void powerManage(powerSubsystemData* task, int i) {
	switch(i) {
		case 1: //powerConsumption
			switch(consumptionState){
				case 0: //Normal power consumption
					if(count == 0 || count%2 == 0) {
						*task->pwrCon += 2;
					} else {
						*task->pwrCon -= 1;
					}
					if(*task->pwrCon > 10);
						consumptionState = 1;
				case 1: //Reverse power consumption
					if(count == 0 || count%2 == 0) {
						*task->pwrCon -= 2;
					} else {
						*task->pwrCon += 1;
					}
					if(*task->pwrCon < 5);
						consumptionState = 0;
			}
		case 0: //powerGeneration
			if(*task->batLevel < 50) {
				if(count == 0 || count%2 == 0) {
					*task->pwrGen += 2;
				} else {
					*task->pwrGen += 1;
				}
			} else if(*task->batLevel > 50 && *task->batLevel < 95) {
				if(count == 0 || count%2 == 0);
					*task->pwrGen += 2;	
			} 
	}

}

void thrusterSubsystem(void* task) 
{

}

void satelliteComs(void* task) 
{

}

void consoleDisplay(void* task) 
{

}

void WarningAlarm(void* task) 
{
	
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
   		seed = seed*multiplier + addOn;
   		randNum = seed;

		if (randNum <0)
		{
			randNum = randNum + max;
		}

		randNum = randNum/max;

		retVal =  ((int)((high-low+1)*randNum))+low;
	}
	
	return retVal;
}
