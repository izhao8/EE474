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
int globalCounter, consumptionState, panelActive, tempCounter,transCounter, imageCounter = 0;
int pulseLength = 250;
int distance= 0;
unsigned short transportBuffer[8] = {0};
double imageBuffer[16] = {0};
double tempBuffer[16] = {0};
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
  double tempBufferConv(int temp);
  void battTempCheck (void *task);
  void imageCapture(void* task);
  void transportDistance (void * task);
  void bufferCheck();
  void pirateManagment (void *task);
  void pirateDetection (void *task);


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
    analogWrite(45, 255);
    while(timeMillis + pulseLength < millis()) {

    }
    timeMillis = millis();
    analogWrite(45, 0);
    while(timeMillis + downLength < millis()) {

    }
    *task0->motorDrive += 1;
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
  Serial.println("ms");
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
 // *task0->batLevel = batteryBuffer(buffer);
 *task0->batLevel = 50;
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
      Serial.println("Press any key to check temperature: ");
      timeMillis = millis();
      while(millis() < timeMillis + 2000) {

      }
      int press = Serial.read();
      if (press > 0) {
        Serial.println();
        batteryTemperature((void *) task0);
      }
      
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
  timeMicros = micros();
  while(micros() < timeMicros + 500) {
    
  }
  // Raw binary value of voltage
  double sensorOneRaw = analogRead(A14);
  double sensorTwoRaw = analogRead(A15);

  // Voltage value
  double sensorOne = tempBufferConv(sensorOneRaw);
  double sensorTwo = tempBufferConv(sensorTwoRaw);

  // Temperature in celcius
 double sensorOneTemp = 32*sensorOne + 33;
 double sensorTwoTemp = 32*sensorTwo + 33;
 Serial.println(sensorOneTemp);
 Serial.println(sensorTwoTemp);
  *task0->battTemp1 = sensorOneTemp;
  *task0->battTemp2 = sensorTwoTemp; 

  tempBuffer[tempCounter] = sensorOneTemp;
  tempBuffer[tempCounter+1] = sensorTwoTemp;

  battTempCheck((void*) task0);

  tempCounter += 2;

  if (tempCounter >= 16) {
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
/*
void satelliteComs(void *task) {
  satelliteComsData *task2 = (satelliteComsData *)task;
  if(thrust) {
    *task2->thrusterCommand = (unsigned int)randomInteger(-10, 10);
    thrust = 0;
  }
  

}*/

unsigned int *convertDtoB(int dec)
{
  unsigned int *command = malloc(sizeof(unsigned int) * 100);

  for (int i = 15; i >= 0; i--)
  {
    command[i] = (dec >> i) & 1;
  }
  return command;
}

// Lab 5 Additions (CHANGE task0 to whatever task we need the function to be inside of)

/*
The system shall be capable of detecting any obstacles that appear in front of the satellite.
When an alien pirate vehicle, cloaked or uncloaked, appears within 100 meters of the
satellite, a detected flag shall be set signifying that the pirateManagement task should be
scheduled.

Once detected, as long as the alien vehicle remains in range, the detection task shall
provide proximity data to the Pirate Management subsystem.
*/
void pirateDetection (void *task) {
  pirateDetectionSubsystemData *task0 = (pirateDetectionSubsystemData *)task;
  double frequency = analogRead(A13);
  double voltage = frequency * 5/1023;
  double distance = voltage * 20;

  if (distance <= 100) {
    *task0->piratesDetected = 1;
  } else {
    *task0-> piratesDetected = 0;
  }

}


/*
The system shall be capable of managing any alien vehicles that appear in front of the
satellite by first releasing a blast of phasor fire then a barrage photon torpedoes if the
obstacle remains intact.
The management system shall operate as follows.

If the pirateProximity is within 30 meters, the phasor subsystem is enabled and the
communications officer can fire phasors at will.

If the pirateProximity is within 5 meters, the photon subsystem is enabled and the
communications officer can fire photons at will – or any other obstacle that might be
interfering with the satellite’s progress.

The Pirate Management subsystem shall fire one phasor burst or one photon torpedo
at a time in response to each command. 
*/
void pirateManagment (void *task) {
  pirateDiscouragementSubsystemData *task0 = (pirateDiscouragementSubsystemData *)task;

  if(*task0->piratesDetected <= 30) {
    *task0->pirateProximity = 1;
  } else if (*task0->piratesDetected <= 5) {
    *task0->pirateProximity = 2;
  }
  
  if(*task0->pirateProximity == 1) {
    Serial.println("Firing Phasors!");
  } else if (*task0->pirateProximity == 2) {
    Serial.println("Firing Photons!");
  }
}

// USER SPECIFIED TASK
/* 
void blahBlah (void *task) {
  
} 
*/



// Helper Functions 

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

// Convert battery temperature from binary to voltage
double tempBufferConv(int temp) {
  double newTemp = temp * 5 / 1023;
  newTemp = newTemp * 0.65;
  return newTemp;
}

// Check battery temperature and set overheating flag
void battTempCheck (void *task) {
  powerSubsystemData *task0 = (powerSubsystemData *)task;
  if(tempCounter == 0) {
    if(*task0->battTemp1 > tempBuffer[14]*1.2) {
      *task0->batteryOverheating = 1;
    } else if (*task0->battTemp2 > tempBuffer[15]*1.2) {
      *task0->batteryOverheating = 1;
    } else {
      *task0->batteryOverheating = 0;
    }
  } else {
    if(*task0->battTemp1 > (tempBuffer[tempCounter-2]*1.2)) {
      *task0->batteryOverheating = 1;
    } else if (*task0->battTemp2 > (tempBuffer[tempCounter-1]*1.2)) {
      *task0->batteryOverheating = 1;
    } else {
      *task0->batteryOverheating = 0;
    }
  }

  Serial.print("Overheat: \t");
  Serial.println(*task0->batteryOverheating);
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
  } 

  // Lab 5 additions:
   else if(press == 115) {  // s
    return 1;
  } else if(press == 105) { // i
    return 1;
  } else if(press == 119) { // w
    return 1;                     // These two need a response of "A" (97)
  } else if(press == 112) { // p
    return 1;
  } 


  else {
    return 0;
  }
}


void imageCapture(void* task) {
  imageCaptureData *task1 = (imageCaptureData *)task;
  int real[256], imag[256] = {0};
  for(int i = 0; i < 256;i++) {
    real[i] = analogRead(A12)*0.01*10 - 10;
    imag[i] = 0;
    delayMicroseconds(100);
  }

  signed int index = optfft(real, imag);
  *task1->imageData = 10000 * index / 200;
  *task1->imageData = abs(*task1->imageData);  
  imageBuffer[imageCounter] = *task1->imageData;
  Serial.println(*task1->imageData);
  for(int i = 0; i< 16; i++) {
    Serial.print(imageBuffer[i]);
    Serial.print(", ");
  }
  Serial.println();
  //Serial.println(imageCounter);
  if(imageCounter >= 16) {
    imageCounter = 0;
  } else {
    imageCounter++;
  }
}


// Fast Fourier Transform Function
signed int optfft(signed int real[256], signed int imag[256]) {

signed int i, i1, j, l, l1, l2, t1, t2, u;

  #include "tables.h"


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

void transportDistance (void * task) {
  transportDistanceData *task0 = (transportDistanceData *)task;
  int duration = floor(pulseIn(A12, HIGH)); //Reads a pulse and returns it in microseconds

  // At 35Hz, duration = 2857 us (micro)
  // At 3.75kHz, duration = 27 us

  // Then convert that to meters
 distance = floor(0.671378 * duration); 
 if(distance > 2000) {
  distance = 2000;
 } else if (distance < 100){
  distance = 100;
 }
 *task0->transportDist = distance;
 bufferCheck();
 Serial.println(distance);
}


void bufferCheck() {
  int difference = 0;
  if(transCounter == 0) {
    difference = transportBuffer[7]-distance;
  } else {
    difference = transportBuffer[transCounter - 1]-distance;
  }
  difference = abs(difference);
  if(transCounter == 0) {
    if (difference > (transportBuffer[7] * 0.1)){
      transportBuffer[transCounter] = distance;

      if(transCounter >= 7) {
        transCounter = 0;
      } else {
        transCounter++;
      }

    } 
  }else {
      if (difference > (transportBuffer[transCounter-1] * 0.1)){
        transportBuffer[transCounter] = distance;

        if(transCounter >= 7) {
          transCounter = 0;
        } else {
          transCounter++;
        }

      }
    }
  }


