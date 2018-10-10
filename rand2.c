#include <stdio.h>
#include <math.h>
#include <float.h>
#include <limits.h>

//  random number generator 

int randomInteger(int low, int high);

int reseed(void);

double randomReal(void);

int seed = 1;
int multiplier = 2743;
int addOn = 5923;
double max = INT_MAX + 1.0;

//  demonstrate the generators
int main(void)
{
	int i = 0;

	printf("random integer\n");
	for (i = 0; i < 100; i++)
	{
		printf("%d ", (randomInteger(0,10)%7));
	}
	
	printf("\n\n");

	return 0;
}

int randomInteger(int low, int high)
{
	if (low > high)
		return randomInteger(high, low);
	else
		return ((int)((high-low+1)*randomReal()))+low;
}

int reseed(void)
{
	seed = seed*multiplier + addOn;
	return seed;
}

//  generate a real random number
double randomReal(void)
{

	double temp = reseed();

	return temp/max;
}


