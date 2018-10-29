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
	unsigned short* batLevel;
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
	unsigned short* batLevel;
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
	unsigned short* batLevel;
	unsigned short* fuelLevel;
	unsigned short* pwrCon;
	unsigned short* pwrGen;
};
typedef struct consoleDisplayData consoleDisplayData;

struct warningAlarmData {
	int* fuelLow;
	int* batLow;
	int* solarPanelState;
	unsigned short* batLevel;
	unsigned short* fuelLevel;
	void (*task)(void*);
};
typedef struct warningAlarmData warningAlarmData;


#endif