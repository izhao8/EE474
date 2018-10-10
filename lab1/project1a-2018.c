// preprocessor directive to support printing to the display

#include <stdio.h>

// the main program
int main(void)
{
	//  declare, efine, and inbitializa some local variables
	setvbuf(stdout, 0, _IONBF, 0); 
	int x =  9;
	int y = 10;
	int z =  0;
	float a = 0.0;

	//  preform a simple calculation
	z = x+y;
	
	//  print the results of the calculation to the display
	printf ("the sum of x and y is %d\n", z);
	
	//  ask the user for some data
	printf("please enter a value\n");
	
	//  get the data from the user
	//    the data will be a floating point number %f
	//    stored in the variable 'a'
	//    the & operator takes the address of the variable 'a'

	scanf ("%f", &a);
	
	//  remove the newline from input buffer
	getchar();
	
	//  print the user data to the display
	//    the format will be xx.yy
	
	printf ("the data is %2.2f\n", a);

	return 0;
}
