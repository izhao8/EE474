#ifndef SATCOMS_H
#define SATCOMS_H

struct TCB {
  void* taskData;
  void (*myTask)(void*);
  struct TCB* next;
  struct TCB* prev;
};
typedef struct TCB TCB;

struct powerSubsystemData {
  int* solarPanelState;
  int* deploy;
  int* retract;
  unsigned int* batLevel;
  unsigned short* pwrCon;
  unsigned short* pwrGen;
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
 int* batteryOverheating;
};
typedef struct consoleDisplayData consoleDisplayData;

struct warningAlarmData {
  int* fuelLow;
  int* batLow;
  int* solarPanelState;
  unsigned int* batLevel;
  unsigned short* fuelLevel;
  int* batteryOverheating;
};
typedef struct warningAlarmData warningAlarmData;

struct vehicleCommsData {
  char* command;
  char* response;
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
  unsigned int* imageDataRaw;
  unsigned int* imageData;
};
typedef struct imageCaptureData imageCaptureData;

struct batteryTempData {
  int** battTemp;
  int* batteryOverheating;
};
typedef struct batteryTempData batterTempData;

/*
struct transportDistanceData {
  
};
typedef struct transportDistanceData transportDistanceData;

struct transportCommsData {
  
};
typedef struct transportCommsData transportCommsData;
*/

#endif
