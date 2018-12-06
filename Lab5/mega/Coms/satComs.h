#ifndef SATCOMS_H
#define SATCOMS_H

struct TCB {
  void* taskData;
  void (*myTask)(void*);
  struct TCB* next;
  struct TCB* prev;
  int priority;
};
typedef struct TCB TCB;

struct powerSubsystemData {
  int* solarPanelState;
  int* deploy;
  int* retract;
  unsigned int* batLevel;
  unsigned short* pwrCon;
  unsigned short* pwrGen;
  int* batteryOverheating;
  double* battTemp1;
  double* battTemp2;
};
typedef struct powerSubsystemData powerSubsystemData;

struct thrusterSubsystemData {
  unsigned int* thrusterCommand;
  unsigned short* fuelLevel;
};
typedef struct thrusterSubsystemData thrusterSubsystemData;

struct satelliteComsData {
  int* fuelLow;
  int* batLow;
  int* solarPanelState;
  unsigned int* batLevel;
  unsigned short* fuelLevel;
  unsigned short* pwrCon;
  unsigned short* pwrGen;
  unsigned int* thrusterCommand;
  char* errorRecieved; // <---------------------------------- NEW
  char* messageRecieved; // <---------------------------------- NEW
};
typedef struct satelliteComsData satelliteComsData;

struct consoleDisplayData {
  int* fuelLow;
  int* batLow;
  int* solarPanelState;
  unsigned int* batLevel;
  unsigned short* fuelLevel;
  unsigned short* pwrCon;
  unsigned short* pwrGen;
 unsigned short* transportDist;
 double* battTemp;
};
typedef struct consoleDisplayData consoleDisplayData;

struct warningAlarmData {
  int* fuelLow;
  int* batLow;
  int* solarPanelState;
  unsigned int* batLevel;
  unsigned short* fuelLevel;
  int* batteryOverheating;
  int* alienDetected; // <---------------------------------- NEW
};
typedef struct warningAlarmData warningAlarmData;

struct vehicleCommsData {
  char* command;
  char* response;
  unsigned int* imageRaw;
  unsigned int* image;
};
typedef struct vehicleCommsData vehicleCommsData;

struct consoleKeypadData {
  int* motorSpeedInc;
  int* motorSpeedDec;
};
typedef struct consoleKeypadData consoleKeypadData;

struct solarPanelControlData {
  int* solarPanelState;
  int* deploy;
  int* retract;
  int* motorSpeedInc;
  int* motorSpeedDec;
  unsigned short* motorDrive;
};
typedef struct solarPanelControlData solarPanelControlData;

struct imageCaptureData {
  double* imageData;
};
typedef struct imageCaptureData imageCaptureData;

struct transportDistanceData {
  int* transportDist;
};
typedef struct transportDistanceData transportDistanceData;

// NEW STRUCTS FOR LAB 5

struct commandManagementData {
  char* command;
  char* response;

  // What if these were made into ints instead of chars? 
  // Do we need to store the actual letter sent or could we just store the ASCII value?
};
typedef struct commandManagementData commandManagementData;


struct pirateDetectionSubsystemData {
  int* piratesDetected;
};
typedef struct pirateDetectionSubsystemData pirateDetectionSubsystemData;


struct pirateDiscouragementSubsystemData {
  // ADD Pointers to Variables HERE
};
typedef struct pirateDiscouragementSubsystemData pirateDiscouragementSubsystemData;


#endif
