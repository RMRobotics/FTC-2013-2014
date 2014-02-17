#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Hubs,  S2, HTServo,  none,     none,     none)
#pragma config(Sensor, S3,     HTSPB,          sensorI2CCustom)
#pragma config(Sensor, S4,     HTSMUX,         sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     leftTread,            tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     leftTread2,            tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_1,     rightTread,           tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     rightTread2,           tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     flag,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     harvester,                 tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     rightHang,             tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C4_2,     leftHang,            tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S2_C1_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_3,    flagServo,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_4,    wrist,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_5,    elbow,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_6,    stopper,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.
#include "drivers/hitechnic-sensormux.h"
#include "drivers/lego-ultrasound.h"
#include "drivers/hitechnic-irseeker-v1.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/hitechnic-accelerometer.h"
#include "drivers/hitechnic-superpro.h"

const tMUXSensor HTIRS = msensor_S4_1;
const tMUXSensor sonar = msensor_S4_2;
const tMUXSensor color = msensor_S4_3;
const tMUXSensor HTGYRO = msensor_S4_4;

//LEDBitmask addres definitions
#define B2 0x04 //IR Detected output address
#define B3 0x08 //Block Detected output address
#define B4 0x10 //Robot Lifting output address
#define B5 0x20 //Stopper In output address

#define STOPPERIN 255
#define STOPPEROUT 0

typedef struct {
	TNxtButtons oldNxtButton;
	short nxtButtonChanged;

	bool mode;
	ubyte LEDBitmask;

	int harvesterSpeed;
	int leftHangSpeed;
	int rightHangSpeed;
	int stopperPos;
} nxtState;

void initialize(nxtState *state);
void updateInput(nxtState *state);
void updateRobot(nxtState *state);
void showDiagnostics(nxtState *state);

task main()
{
	nxtState currentState;
	initialize(currentState);

	while(true){
		updateInput(currentState);
		updateRobot(currentState);
		showDiagnostics(currentState);
	}
}

void initialize(nxtState *state) {
	memset(state, 0, sizeof(state));
	HTSPBsetupIO(HTSPB, 0xFF);
	updateRobot(state);
}

void updateInput(nxtState *state) {
	state->nxtButtonChanged = state->oldNxtButton ^ nNxtButtonPressed;
	state->oldNxtButton = nNxtButtonPressed;

	if (nNxtButtonPressed == 3 && state->nxtButtonChanged)
		state->mode = !state->mode;

	if (state->mode) {
		state->stopperPos = STOPPERIN;
		state->LEDBitmask = B5;
	} else {
		state->stopperPos = STOPPEROUT;
		state->LEDBitmask = 0x00;
	}

	if (nNxtButtonPressed == 1){
		state->harvesterSpeed = 100;
	}else if (nNxtButtonPressed == 2){
		state->harvesterSpeed = -100;
	}else {
		state->harvesterSpeed = 0;
		state->harvesterSpeed = 0;
	}

	state->leftHangSpeed = 0;
	state->rightHangSpeed = 0;
}

void updateRobot(nxtState *state) {
	motor[harvester]=state->harvesterSpeed;
	motor[leftHang]=state->leftHangSpeed;
	motor[rightHang]=state->rightHangSpeed;
	servo[stopper] = state->stopperPos;
	HTSPBwriteIO(HTSPB, state->LEDBitmask);
}

void showDiagnostics(nxtState *state) {
	//create label
	string batteryLevel = "power = ";

	//store variable in a string
	string string5 = externalBatteryAvg;

	//concat variable with label
	strcat(batteryLevel, string5);

	eraseDisplay();

	//display label and value
	nxtDisplayTextLine(1, "Harvester");
	nxtDisplayTextLine(2, "LftBtn:dn,RtBtn:up");
	nxtDisplayTextLine(3, batteryLevel);
}
