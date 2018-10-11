#ifndef SATCOMS_H
#define SATCOMS_H

struct TCB {
	void* taskData;
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
};
typedef struct warningAlarmData warningAlarmData;

/* Power function that tracks power consumption
and deploys solar panels if battery levels are low */
void powerSubsystem();

/* Thruster function that manages duration and magnitude
of movement to desired location */
void thrusterSubsystem();

/* Coms function that transmits satellite systems
and are limited to thruster commands for this lab */
void satelliteComs();

/* Announces warnings and statuses of the systems by
three major categories: status, annunciation, and alarm.
Displayed on the console */
void consoleDisplay();

/* Displays low fuel and battery */
void WarningAlarm();

/* Generates random numbers for thrusterSubsystem */
int randomInteger(int low, int high);

/* Helper function used in powerSubsystem to calculate
the amount of power generated and consumed every time the
powerSubsystem function is called*/
void powerManage(powerSubsystemData* task, int i);

/* Determines which subsystem and function should be 
running and manages TCB pointer*/
void schedule();

#endif
