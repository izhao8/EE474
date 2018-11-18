#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "satComs.h"

#define ABS(x)  (((x)<0)?(-(x)):(x))
#define CEILING(x) (((x)>511)?511:(x))
//Used in random function for thruster command
int seed = 1;
//Used in powerManage function
int globalCounter, consumptionState, panelActive, tempCounter, transCounter = 0;
int pulseLength = 250;
unsigned long timeMillis, timeMicros, signalTime = 0;
void powerManage(void *task0, int i);
unsigned int convertBtoD(unsigned int *bits, int length);
unsigned int *convertDtoB(int dec);
int randomInteger(int low, int high);
unsigned int batteryBuffer(int battery);
void consoleKeyPad(void *task);
void solarPanelControl(void *task);
void powerSubsystem(void *task);
void batteryTemperature (void *task);
signed int optfft(signed int x[256], signed int y[256]);
unsigned int tempBuffer(int temp);
void battTempCheck (void *task);


// Solar Panel Control
void solarPanelControl(void *task)
{
  solarPanelControlData *task0 = (solarPanelControlData *)task;
  panelActive = 1;
  int downLength = 500 - pulseLength;

  if(*task0->motorSpeedInc == 1) {
    pulseLength += 25;
    if(pulseLength > 500) {
      pulseLength = 500;
    }
    *task0->motorSpeedInc = 0;
  } else if (*task0->motorSpeedDec == 1) {
    pulseLength -=25;
    if(pulseLength < 0) {
      pulseLength = 0;
    }
    *task0->motorSpeedDec = 0;
  }
  if (*task0->motorDrive < 100) {
    timeMillis = millis();
    while(timeMillis + pulseLength < millis()) {
      analogWrite(A13, 255);
    }
    timeMillis = millis();
    while(timeMillis + downLength < millis()) {
      analogWrite(A13, 0);
    }
    *task0->motorDrive += 10;
  } else {
    if(*task0->deploy) {
      *task0->solarPanelState = 1;
    } else {
      *task0->solarPanelState = 0;
    }
    *task0->motorDrive = 0;
    *task0->deploy = 0;
    *task0->retract = 0;
    pulseLength = 250;
  }
  Serial.println();
  Serial.print("MOTOR DRIVE: \t");
  Serial.println(*task0->motorDrive);
  Serial.print("PWM SIGNAL: \t");
  Serial.print(pulseLength);
  Serial.println("us");
  Serial.println();
}

// Console key pad control 
void consoleKeyPad(void *task)
{
  consoleKeypadData *task0 = (consoleKeypadData *)task;
  int press = 0;
  Serial.println("Press I or D to change PWM");
  timeMillis = millis();
  while (millis() - timeMillis < 2000) {

  }
  Serial.println();
  press = Serial.read();
  if (press == 105){
    *task0->motorSpeedInc = 1;
  } else if (press == 100) {
    *task0->motorSpeedDec = 1;
  } else {
    Serial.print("Error: Invalid Input \n");
  }
}

// The power subsytem keeps track of the battery level and the battery temperature and power consomption
void powerSubsystem(void *task)
{
  powerSubsystemData *task0 = (powerSubsystemData *)task;
  timeMicros = micros();
  while (timeMicros + 600 < micros()) {

  }

// Battery Level set by DC power supply
// PIN 18 TX1: Transmit serial data
// PIN 19 RX1: Receive serial data

 unsigned long buffer = analogRead(A13);
  *task0->batLevel = batteryBuffer(buffer);

  if (*task0->solarPanelState == 1)
  {
    if ((int)*task0->batLevel > 95)
    {
      *task0->retract = 1;
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
      *task0->deploy = 1;
    }
  }
}

void batteryTemperature (void *task) {
  powerSubsystemData *task0 = (powerSubsystemData *)task;

  // Raw binary value of voltage
  unsigned long sensorOneRaw = analogRead(A14);
  unsigned long sensorTwoRaw = analogRead(A15);

  // Voltage value
  int sensorOne = tempBuffer(sensorOneRaw);
  int sensorTwo = tempBuffer(sensorTwoRaw);

  // Temperature in celcius
 int sensorOneTemp = 32*sensorOne*1000 + 33;
 int sensorTwoTemp = 32*sensorTwo*1000 + 33;

  task0->battTemp[tempCounter] = sensorOne;
  task0->battTemp[tempCounter+1] = sensorTwo; 

  battTempCheck((void*) task0);

  tempCounter += 2;

  if (tempCounter >= 14) {
    tempCounter = 0;
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
}// Test val 1023 == batteryLevel: 100
// unsigned long buffer = 1023;

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


unsigned int batteryBuffer(int battery)
{
  double newBat = battery * 5 / 1023;
  newBat = floor(newBat * 7.2);
  newBat = floor(newBat *2.78);
  return (unsigned int) newBat;
}

// Convert batter temperature from binary to voltage
unsigned int tempBuffer(int temp) {
  double newTemp = temp * 3.25 / 1023;
  return newTemp;
}

// Check battery temperature and set overheating flag
void battTempCheck (void *task) {
  powerSubsystemData *task0 = (powerSubsystemData *)task;
  timeMicros = micros();
  while(micros() < timeMicros + 500) {
  	
  }
  for (int i = 0; i < 15; i++) {
    if (task0->battTemp[tempCounter] > (task0->battTemp[i] * 1.2)) {
      *task0-> batteryOverheating = 1;
    } else if (task0->battTemp[tempCounter+1] > (task0->battTemp[i] * 1.2)) {
      *task0-> batteryOverheating = 1;
    } else {
      *task0->batteryOverheating = 0;
    } 
  }
}

// Check if any of the follownig keys were pressed
int inputCheck(int press) {
  if(press == 102) {
    return 1;
  } else if(press == 98) {
    return 1;
  } else if(press == 108) {
    return 1;
  } else if(press == 114) {
    return 1;
  } else if(press == 100) {
    return 1;
  } else if(press == 104) {
    return 1;
  } else {
    return 0;
  }
}


void imageCapture(*void) {
    imageCaptureData *task1 = (imageCaptureData *)task;
    
    // Raw binary
    *task->imageDataRaw = analogRead(A12);

    // int frequency = ?;

    // FFT preformed on bindary to give us an int
    *task->imageData = optfft(*task->imageDataRaw, frequency);

}


// Fast Fourier Transform Function
signed int optfft(signed int real[256], signed int imag[256]) {

signed int i, i1, j, l, l1, l2, t1, t2, u;

#include "tables.c"

  /* Bit reversal. */
  /*Do the bit reversal */
  l2 = 128;
  i=0;
  for(l=0;l<255;l++) {
    if(l < i) {
      j=real[l];real[l]=real[i];real[i]=j;
    }
    l1 = l2;
    while (l1 <= i){
      i -= l1;
      l1 >>= 1;
    }
    i += l1;
  }

  /* Compute the FFT */
  u = 0;
  l2 = 1;
  for(l=0;l<8;l++){
    l1 = l2;
    l2 <<= 1;
    for(j=0;j<l1;j++){
      for(i=j;i<256;i+=l2){
        i1 = i + l1;
        t1 = (u1[u]*real[i1] - u2[u]*imag[i1])/32; 
        t2 = (u1[u]*imag[i1] + u2[u]*real[i1])/32;
        real[i1] = real[i]-t1;
        imag[i1] = imag[i]-t2;
        real[i] += t1;
        imag[i] += t2;
      }
      u++;
    }
  }

  /* Find the highest amplitude value */
  /* start at index 1 because 0 can hold high values */
  j=1;
  l=0;      
  for ( i=1; i<(128); i++ ) {
    l1 = square[CEILING(ABS(real[i]))]+square[CEILING(ABS(imag[i]))];
    if (l1 > l) {
      j = i;
      l = l1;
    }
  }
  return (j);
}

void transportDistance (*void) {
  transportDistancData *task0 = (transportDistancData *)task;
  int duration = floor(pulseIn(A12, HIGH)); //Reads a pulse and returns it in microseconds

  // At 35Hz, duration = 2857 us (micro)
  // At 3.75kHz, duration = 27 us

  // Then convert that to meters
 short distance = floor(0.671378 * duration); 

if(transCounter == 0) {
  if(task0->transportDist[7]*1.1 < distance) {
    task0->transportDist[transCounter] = distance;
  }
} else{
  if(task0->transportDist[transCounter-1]*1.1 < distance) {
    task0->transportDist[transCounter] = distance;
  }
}

  if (transCounter >= 7) {
    transCounter = 0;
  } else {
    transCounter++;
  }

}

