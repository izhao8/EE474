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
};
typedef struct consoleDisplayData consoleDisplayData;

struct warningAlarmData {
	int* fuelLow;
	int* batLow;
	int* solarPanelState;
	unsigned int* batLevel;
	unsigned short* fuelLevel;
};
typedef struct warningAlarmData warningAlarmData;

struct miningCommunications {
	char* command;
	char* response;
};
typedef struct miningCommunications miningCommunications;

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
};
typedef struct solarPanelControlData solarPanelControlData;

#endif