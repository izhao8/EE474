output: satComs.o satelliteComs.o
	gcc -Wall -std=c11 -g satelliteComs.o satComs.o

satelliteComs.o: satComs.h satComs.c satelliteComs.c
	gcc -Wall -std=c11 -g -c satelliteComs.c 

satComs.o: satComs.c satComs.h
	gcc -Wall -std=c11 -g -c satComs.c satComs.h 

clean: 
	rm *.o output;