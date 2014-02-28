#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Hubs,  S2, HTServo,  none,     none,     none)
#pragma config(Sensor, S3,     HTSPB,          sensorI2CCustom)
#pragma config(Sensor, S4,     SMUX,         sensorI2CCustom)
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
#include "drivers/hitechnic-irseeker-v2.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/hitechnic-accelerometer.h"
#include "drivers/hitechnic-colour-v2.h"
#include "drivers/hitechnic-superpro.h"

const tMUXSensor HTIRS = msensor_S4_1;
const tMUXSensor sonar = msensor_S4_2;
const tMUXSensor color = msensor_S4_3;
const tMUXSensor HTGYRO = msensor_S4_4;

#define LEFT 1
#define RIGHT -1
#define INFINITETIME 30000

#define TURNSPEED -75
#define DRIVESPEED -50
#define COMP .95

#define INLINEFOLLOWRATIO .5
#define OUTLINEFOLLOWRATIO .5
#define INNERSONARBOUND 25
#define OUTERSONARBOUND 30

#define FLAGSERVOOUT 255
#define FLAGSERVOIN 0
#define STOPPERIN 255
#define STOPPEROUT 0
#define WRISTIN 255
#define WRISTOUT 50
#define ELBOWIN 0
#define ELBOWOUT 115

//LEDBitmask addres definitions
#define B2 0x04 //IR detected output address
#define B3 0x08 //block detected output address
#define B4 0x0F //robot lifting output address
#define B5 0x20 //stopper in output address

typedef struct{
	bool mode;
	int irDir;
	int dist;
	int time;
	float degrees;
} SensorValues;

void initialize(SensorValues *state);
void getSensors(SensorValues *state);
void drive(SensorValues *state, int time, bool senseIR, bool senseSonar);
void turn (SensorValues *state, int dir, int amount);
void showDiagnostics(SensorValues *state);

void initialize(SensorValues *state){
	HTSPBsetupIO(HTSPB, 0xFF);
	while (externalBatteryAvg < 0){
		HTSPBwriteIO(HTSPB, B3);
		PlayTone(700, 1);
	}
	HTSPBwriteIO(HTSPB, 0x00);
	memset(state, 0, sizeof(state));
	servo[wrist] = WRISTIN;
	servo[elbow] = ELBOWIN;
	servo[stopper] = STOPPERIN;

	// Calibrate the gyro, make sure you hold the sensor still
	HTGYROstartCal(HTGYRO);

	TNxtButtons oldButton = nNxtButtonPressed;
	short nxtButtonChanged;
	while (nNxtButtonPressed != 3) {
		PlayTone(500, 1);
		showDiagnostics(state);
		nxtButtonChanged = oldButton ^ nNxtButtonPressed;
		oldButton = nNxtButtonPressed;
		if ((nNxtButtonPressed == 1 || nNxtButtonPressed == 2) && nxtButtonChanged)
			state->mode = !state->mode;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
//                      Drive **BACKWARD** onto ramp, then score auto block
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

task main()
{
	SensorValues state;
	initialize(&state);
	waitForStart();

	drive(&state, 100, false, false); //parameters: SensorValues struct, time, use ir?, use sonar?
	turn(&state, LEFT, 30); //parameters: SensorValues struct, direction, amount
	drive(&state, 3000, false, false);
	//*****score block******
	servo[elbow] = ELBOWOUT;
	wait1Msec(100);
	servo[wrist] = WRISTOUT;
	wait1Msec(500);
	servo[wrist] = WRISTIN;
	wait1Msec(100);
	servo[elbow] = ELBOWIN;
	//**********************
	if (state.mode)
		drive(&state, 1000, false, false);
}

void getSensors(SensorValues *state){
	//************************IR Sensor**************************
	int _dir = 0;
	int dcS1, dcS2, dcS3, dcS4, dcS5 = 0;

	// read all of the sensors' values at once,
	// exit the app if an error occurs
	if (! HTIRS2readAllACStrength(HTIRS, dcS1, dcS2, dcS3, dcS4, dcS5)) {
		nxtDisplayTextLine(4, "ERROR!!");
		wait1Msec(2000);
		StopAllTasks();
	}

	// read the direction from which the signal is coming,
	// exit the subroutine if an error occurs
	_dir = HTIRS2readACDir(HTIRS);
	if (_dir < 0) {
		nxtDisplayTextLine(4, "ERROR!!");
		wait1Msec(2000);
		StopAllTasks();
	}

	state->irDir = _dir;
	//*********************************************************

	//***********************Sonar*****************************
	state->dist = USreadDist(sonar);
	//*********************************************************

	//*****************Gyro and Accelerometer******************
	float rotSpeed = 0;

	//Wait until 20ms has passed
	wait1Msec(20);

	// Read the current rotation speed
	rotSpeed = HTGYROreadRot(HTGYRO);

	// Crude integration: waited 20ms so degrees turned would be speed * time
	state->degrees += rotSpeed * 0.02;
	//*********************************************************

	//*****************Show Debugging Window*******************
	showDiagnostics(state);
	//*********************************************************
}

void drive(SensorValues *state, int time, bool senseIR, bool senseSonar) {
	getSensors(state);
	if (senseIR)
		senseIR = state->irDir == 5;
	if (senseSonar)
		senseSonar = state->dist < 75;

	time1[T1] = 0;
	bool stopCondition = time1[T1] > time || senseIR || senseSonar;

	while(!stopCondition){
		motor[leftTread] = COMP*DRIVESPEED;
		motor[rightTread] = DRIVESPEED;
		motor[leftTread2] = COMP*DRIVESPEED;
		motor[rightTread2] = DRIVESPEED;
		stopCondition = time1[T1] > time || senseIR || senseSonar;
	}

	motor[leftTread] = 0;
	motor[rightTread] = 0;
	motor[leftTread2] = 0;
	motor[rightTread2] = 0;
}

void turn(SensorValues *state, int dir, int amount){
	state->degrees = 0;
	while(abs(state->degrees) < amount){
		getSensors(state);
		motor[leftTread] = dir*TURNSPEED;
		motor[leftTread2] = dir*TURNSPEED;
		motor[rightTread] = -dir*TURNSPEED;
		motor[rightTread] = -dir*TURNSPEED;
	}

	motor[leftTread] = 0;
	motor[rightTread] = 0;
	motor[leftTread2] = 0;
	motor[rightTread2] = 0;
}

void showDiagnostics(SensorValues *state){
	//create label
	string sonarSensor = "sonar = ";
	string irSensor = "IR = ";
	string gyroSensor = "Gyro =";
	string timer = "Timer = ";
	string batteryLevel = "power = ";

	//store variable in a string
	string string1 = state->dist;
	string string2 = state->irDir;
	string string3 = state->degrees;
	string string4 = time1[T1];
	string string5 = externalBatteryAvg;

	//concat variable with label
	strcat(sonarSensor, string1);
	strcat(irSensor, string2);
	strcat(gyroSensor, string3);
	strcat(timer, string4);
	strcat(batteryLevel, string5);

	eraseDisplay();

	//display label and value
	nxtDisplayTextLine(1, state->mode?"long":"short");
	nxtDisplayTextLine(2, sonarSensor);
	nxtDisplayTextLine(3, irSensor);
	nxtDisplayTextLine(4, gyroSensor);
	nxtDisplayTextLine(5, timer);
	nxtDisplayTextLine(6, batteryLevel);
}
