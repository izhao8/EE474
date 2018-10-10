#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int randomInteger(int low, int high);

int seed = 1;

void main(void)
{
	int i = 0;
	int n = 0;

	// Print hex digits one after another
	for(i=0; i<20; i++)
	{
      	n = randomInteger(-10,10)%3;
		printf("the number is: %d\n", n);
	}
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

