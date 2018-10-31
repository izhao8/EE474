#include <stdio.h>
#include <stdlib.h>

#include "tasks.h"
#include "satComs.h"

int main (void) {
    int driveMotorSpeedDec = 0;
    int driveMotorSpeedInc = 0;

    consoleKeypadData* task0 = (consoleKeypadData*)
        malloc(sizeof(consoleKeypadData));

    task0->motorSpeedDec = &driveMotorSpeedDec;
    task0->motorSpeedInc = &driveMotorSpeedInc;


    consoleKeyPad(task0, 1);
}