#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
extern "C" {
  #include "satComs.h"
  #include "tasks.h"
}

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFC00

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
void WarningAlarm(void *task);
void thrusterSubsystem(void *task);
void scheduler();
void vehicleComms(void* task);
void start();
void insertNode(uintptr_t address);
void deleteNode(uintptr_t address);
void bufferCheck();

int startup = 1;
int userInput = 1;
int fuelcount, batcount, tempcount, ackCounter = 0;
int recieved, ack = 1;
  /*
  initialize ALL the variables for the structs as global
  */
  int solarPanelState = 0; // FALSE = 0, TRUE = 1
  unsigned int batLevel = 0;
  unsigned short pwrCon = 0;
  unsigned short pwrGen = 0;
  unsigned int thusterCommand = 0;
  unsigned short fuelLevel = 100;
  int fuelLow = 0; // FALSE = 0, TRUE = 1
  int batLow = 0; // FALSE = 0, TRUE = 1
  char command = NULL;
  char response = NULL;
  int deploy = 0;
  int retract = 0;
  int motorSpeedInc = 0;
  int motorSpeedDec = 0;
  double battTemp1, battTemp2 = 0;
  unsigned short motorDrive = 0;
  int batteryOverheating = 0;
  double imageData = 0;
  int transportDist = 0;
  /*
  initialize structs for all subsystems (task order tbd)
  */
  powerSubsystemData* task0 = (powerSubsystemData*) 
    malloc(sizeof(powerSubsystemData));
  thrusterSubsystemData* task1 = (thrusterSubsystemData*) 
    malloc(sizeof(thrusterSubsystemData));
  satelliteComsData* task2 = (satelliteComsData*) 
    malloc(sizeof(satelliteComsData));
  consoleDisplayData* task3 = (consoleDisplayData*) 
    malloc(sizeof(consoleDisplayData));
  warningAlarmData* task4 = (warningAlarmData*) 
    malloc(sizeof(warningAlarmData));
  vehicleCommsData* task5 = (vehicleCommsData*)
    malloc(sizeof(vehicleCommsData));
  consoleKeypadData* task6 = (consoleKeypadData*)
    malloc(sizeof(vehicleCommsData));
  solarPanelControlData* task7 = (solarPanelControlData*)
    malloc(sizeof(solarPanelControlData));
  imageCaptureData* task8 = (imageCaptureData*)
    malloc(sizeof(imageCaptureData));
  transportDistanceData* task9 = (transportDistanceData*)
    malloc(sizeof(transportDistanceData));

/*
initialize structs for task control board (TCB)
*/
TCB *power = (TCB *)malloc(sizeof(TCB));
TCB *thruster = (TCB *)malloc(sizeof(TCB));
TCB *satellite = (TCB *)malloc(sizeof(TCB));
TCB *console = (TCB *)malloc(sizeof(TCB));
TCB *warning = (TCB *)malloc(sizeof(TCB));
TCB *solarPanel = (TCB *)malloc(sizeof(TCB));
TCB *vehicle = (TCB *)malloc(sizeof(TCB));
TCB *keypad = (TCB *)malloc(sizeof(TCB));
TCB *image = (TCB *)malloc(sizeof(TCB));
TCB *transport = (TCB *)malloc(sizeof(TCB));

TCB *head = (TCB *)malloc(sizeof(TCB));
TCB *tail = (TCB *)malloc(sizeof(TCB));

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial.begin(9600);

  //powerSusbsystemData variables
  task0->solarPanelState = &solarPanelState;
  task0->batLevel = &batLevel;
  task0->pwrCon = &pwrCon;
  task0->pwrGen = &pwrGen;
  task0->deploy = &deploy;
  task0->retract = &retract;
  task0->batteryOverheating = &batteryOverheating;
  task0->battTemp1 = &battTemp1;
  task0->battTemp2 = &battTemp2;
  power->taskData = task0;
  power->myTask = &powerSubsystem;
  power->priority = 1;

  //thrusterSubsystemData variables
  task1->thrusterCommand = &thusterCommand;
  task1->fuelLevel = & fuelLevel;
  thruster->taskData = task1;
  thruster->myTask = &thrusterSubsystem;
  thruster->priority = 2;

  //satelliteComsData variables
  task2->fuelLow = &fuelLow;
  task2->batLow = &batLow;
  task2->solarPanelState = &solarPanelState;
  task2->batLevel = &batLevel;
  task2->fuelLevel = &fuelLevel;
  task2->pwrCon = &pwrCon;
  task2->pwrGen = &pwrGen;
  task2->thrusterCommand = &thusterCommand;
  satellite->taskData = task2;
  satellite->myTask = &satelliteComs;
  satellite->priority = 3;

  //consoleDisplayData variables
  task3->fuelLow = &fuelLow;
  task3->batLow = &batLow;
  task3->solarPanelState = &solarPanelState;
  task3->batLevel = &batLevel;
  task3->fuelLevel = &fuelLevel;
  task3->pwrCon = &pwrCon;
  task3->pwrGen = &pwrGen;
  task3->transportDist = transportDist;
  task3->battTemp = &battTemp2;
  console->taskData = task3;
  console->myTask = &consoleDisplay;
  console->priority = 4;

  //warningAlarmData variables
  task4->fuelLow = &fuelLow;
  task4->batLow = &batLow;
  task4->solarPanelState = &solarPanelState;
  task4->batLevel = &batLevel;
  task4->fuelLevel = &fuelLevel;
  task4->batteryOverheating = &batteryOverheating;
  warning->taskData = task4;
  warning->myTask = &WarningAlarm;
  warning->priority = 5;

  task5->command = &command;
  task5->response = &response;
  vehicle->taskData =&task5;
  vehicle->myTask = &vehicleComms;
  warning->priority = 4;

  task6->motorSpeedDec = &motorSpeedDec;
  task6->motorSpeedInc = &motorSpeedInc;
  keypad->taskData = task6;
  keypad->myTask = &consoleKeyPad;
  keypad->priority = 2;

  task7->solarPanelState = &solarPanelState;
  task7->deploy = &deploy;
  task7->retract = &retract;
  task7->motorSpeedInc = &motorSpeedInc;
  task7->motorSpeedDec = &motorSpeedDec;
  task7->motorDrive = &motorDrive;
  solarPanel->taskData = task7;
  solarPanel->myTask = &solarPanelControl;
  solarPanel->priority = 1;

  task8->imageData = &imageData;
  image->taskData = task8;
  image->myTask = &imageCapture;
  image->priority = 3;

  task9->transportDist = &transportDist;
  transport->taskData = task9;
  transport->myTask = transportDistance;
  transport->priority = 3;


 Serial.println(F("TFT LCD test"));


#ifdef USE_Elegoo_SHIELD_PINOUT
 Serial.println(F("Using Elegoo 2.4\" TFT Arduino Shield Pinout"));
#else
 Serial.println(F("Using Elegoo 2.4\" TFT Breakout Board Pinout"));
#endif

 Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

 tft.reset();

  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
   Serial.println(F("Found ILI9325 LCD driver"));
 } else if(identifier == 0x9328) {
   Serial.println(F("Found ILI9328 LCD driver"));
 } else if(identifier == 0x4535) {
   Serial.println(F("Found LGDP4535 LCD driver"));
 }else if(identifier == 0x7575) {
   Serial.println(F("Found HX8347G LCD driver"));
 } else if(identifier == 0x9341) {
   Serial.println(F("Found ILI9341 LCD driver"));
 } else if(identifier == 0x8357) {
   Serial.println(F("Found HX8357D LCD driver"));
 } else if(identifier==0x0101)
 {     
     identifier=0x9341;
      Serial.println(F("Found 0x9341 LCD driver"));
 }
 else if(identifier==0x1111)
 {     
     identifier=0x9328;
      Serial.println(F("Found 0x9328 LCD driver"));
 }
 else {
   Serial.print(F("Unknown LCD driver chip: "));
   Serial.println(identifier, HEX);
   Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
   Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
   Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
   Serial.println(F("If using the breakout board, it should NOT be #defined!"));
   Serial.println(F("Also if using the breakout, double-check that all wiring"));
   Serial.println(F("matches the tutorial."));
   identifier=0x9328;
 
 }
 tft.begin(identifier);
  start();
}

void loop() { 
  if(deploy || retract) {
    head = solarPanel;
    tail = keypad;
  } else {
    head = power;
    tail = vehicle;
  }
  scheduler();
  //solarPanel->myTask(solarPanel->taskData);
/*  transport->myTask(transport->taskData);
  bufferCheck();
  for(int i = 0; i< 8; i++) {
    Serial.print(transportBuffer[i]);
    Serial.print(", ");
  }
  Serial.println();
  delay(3000);*/

/*  analogWrite(45,255);
  delay(250);
  analogWrite(45, 0);
  delay(1000);
  */
}

void scheduler() {
  unsigned long time;
  TCB* current = head;
  while (current != NULL) {
    current->myTask(current->taskData);
    current = current->next;
    WarningAlarm((void*)task4);
    time = millis();
    while(millis() - time < 1000){

    }
  }

  Serial.print("Image Capture?");
  time = millis();
  while(millis() - time < 1000){

  }
  int press = Serial.read();
  if (press > 0){
    image->myTask(image->taskData);
  }
  globalCounter++;
}

void consoleDisplay(void *task)
{
  //Normal satellite status
  consoleDisplayData *task3 = (consoleDisplayData *)task;
  Serial.print("BATTERY LEVEL: \t\t");
  if(*task3->batLevel > 100 || *task3->batLevel < 0) {
    Serial.println("0\n");
  } else {
    Serial.println(*task3->batLevel);
  }
  Serial.print("FUEL LEVEL: \t\t");
  Serial.println(*task3->fuelLevel);
  Serial.print("POWER CONSUMPTION: \t");
  Serial.println(*task3->pwrCon);
  Serial.print("POWER GENERATION: \t");
  Serial.println(*task3->pwrGen);
  Serial.print("CHARGING STATUS: \t");
  if (*task3->solarPanelState)
  {
    Serial.println("ON");
    Serial.println();
  }
  else
  {
   Serial.println("OFF");
   Serial.println();
  }
  if(deploy) {
    Serial.println("DEPLOY");
  }
  if(retract) {
    Serial.println("RETRACT");
  }
  //Annunciation mode
  if (*task3->batLevel < 50)
  {
    *task3->batLow = 1;
    Serial.println("BATTERY LOW");
  }
  else
  {
    *task3->batLow = 0;
  }
  if (*task3->fuelLevel < 50)
  {
    *task3->fuelLow = 1;
    Serial.println("Fuel LOW");
  }
  else
  {
    *task3->fuelLow = 0;
  }
  Serial.print("TRANSPORT DISTANCE: \t");
  Serial.println(*task9->transportDist);
  Serial.print("TEMPERATUR: \t");
  Serial.println(*task3->battTemp);

}

void WarningAlarm(void *task) {
  warningAlarmData *task4 = (warningAlarmData *)task;
 tft.setCursor(0, 0);
 tft.setTextSize(4);
 
 
 if(*task4->batLevel <= 10) {
   if(batcount >= 2){
     tft.setTextColor(BLACK);
     batcount = 0;
   } else {
     batcount++;
     tft.setTextColor(RED); 
   }
 } else if (*task4->batLevel <= 50) {
   if(batcount == 1){
     tft.setTextColor(BLACK);
     batcount = 0;
   } else {
     tft.setTextColor(ORANGE);
     batcount++;
   }
 } else {
   tft.setTextColor(GREEN);
 }

 tft.print("BATTERY");
 tft.println();
 
 if(*task4->fuelLevel <= 10) {
   if(fuelcount >= 2) {
     tft.setTextColor(BLACK);
     fuelcount = 0;
   } else {
      tft.setTextColor(RED);
      fuelcount++;
   }
 } else if (*task4->fuelLevel <= 50) {
   if(fuelcount == 1) {
     tft.setTextColor(BLACK);
     fuelcount = 0;
   } else {
      tft.setTextColor(ORANGE);
      fuelcount++;
   }
 } else {
   tft.setTextColor(GREEN);
 }
 
 tft.print("FUEL");
 tft.println();


 if(batteryOverheating && !recieved) {
    ack = 0;
    tft.setCursor(0,65);
    tft.setTextColor(RED);
    tft.print("TEMP");
    ackCounter++;
    //if(!recieved) {
    int press = 0;

      press = Serial.read();
      if(press > 0) {
        recieved = 1;
      } else {
        recieved = 0;
      }
    //}
  if(!recieved && ackCounter > 15) {
    int count = 0;
    if(tempcount < 5) {
      while (count < 20) {
        tft.setCursor(0, 65);
        if(count%2 == 0) {
          tft.setTextColor(BLACK);
          tft.print("TEMP");
          tft.println();
        } else {
          tft.setCursor(0, 65);
          tft.setTextColor(RED);
          tft.print("TEMP");
          tft.println();
        }
        delay(50);
        count++;
      }
    } else {
      tft.setCursor(0, 65);
      tft.setTextColor(RED);
      tft.print("TEMP");
    }

    if (tempcount >= 10) {
      tempcount = 0;
    } else {
      tempcount++;
    }
  }
  } else {
    tft.setCursor(0, 65);
    tft.setTextColor(BLACK);
    tft.print("TEMP");
    ackCounter = 0;
    recieved = 0;
  }

   
}

void vehicleComms(void* task) {
  vehicleCommsData* task5 = (vehicleCommsData*) task;
  int press = 0;
  timeMillis = millis();
  while (millis() - timeMillis < 2000) {

  }
  Serial.println();
  press = Serial.read();

  if(inputCheck(press)) {
    Serial.println(press);
    Serial.println("Command Response Sent");
  } else {
    Serial.print("Error: Invalid Input \n");
  }

  timeMillis = millis();
  while (millis() - timeMillis < 2000) {

  }
  press = Serial.read();
  if(press == 116 || press == 100)
    if(press == 116) {
      Serial.println("k");
    } else {
      Serial.println("c");
    }
}

void start() 
{
  head = power;
  power->next = thruster;
  thruster->next = satellite;
  satellite->next = console;
  console->next = vehicle;

  tail = vehicle;
  tail->next = NULL;
  vehicle->prev = console;
  console->prev = satellite;
  satellite->prev = thruster;
  thruster->prev = power;
  head->prev = NULL;

  solarPanel->next = keypad;
  solarPanel->prev = NULL;
  keypad->next = NULL;
  keypad->prev = solarPanel;

  pinMode(45, OUTPUT);
}

void insertNode(uintptr_t address)
{
   TCB* node = (TCB*) address;
   if(NULL == head) // If the head pointer is pointing to nothing
   {
   head = node; // set the head and tail pointers to point to this node
   tail = node;
   } else // otherwise, head is not NULL, add the node to the end of the list
  {
   tail -> next = node;
   node -> prev = tail; // note that the tail pointer is still pointing
   // to the prior last node at this point
   tail = node; // update the tail pointer
  }
   return;
} 

void deleteNode(uintptr_t address)
{
  TCB* node = (TCB*) address;
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

/*void bufferCheck() {
  int difference = 0;
  if(transCounter == 0) {
    difference = transportBuffer[7]-transportDist;
  } else {
    difference = transportBuffer[transCounter - 1]-transportDist;
  }
  difference = abs(difference);
  if(transCounter == 0) {
    if (difference > (transportBuffer[7] * 0.1)){
      Serial.println("first");
      transportBuffer[transCounter] = transportDist;

      if(transCounter >= 7) {
        transCounter = 0;
      } else {
        transCounter++;
      }

  	} 
  }else {
      if (difference > (transportBuffer[transCounter-1] * 0.1)){
        Serial.println("got here");
        transportBuffer[transCounter] = transportDist;

        if(transCounter >= 7) {
          transCounter = 0;
        } else {
          transCounter++;
        }

      }
    }
  }*/
