#ifndef SATCOMS_H
#define SATCOMS_H

struct powerSubsystemData {
	int* solarPanelState;
	unsigned short* batLevel;
	unsigned short* pwrCon;
	unsigned short* pwrGen;
	void (*task)(void*);
};
typedef struct powerSubsystemData powerSubsystemData;

struct thrusterSubsystemData {
	unsigned int* thrusterCommand;
	unsigned short* fuelLevel;
	void (*task)(void*);
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
	void (*task)(void*);
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
	void (*task)(void*);
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
