#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     color2,         sensorCOLORFULL)
#pragma config(Sensor, S3,     HTSPB,          sensorI2CCustom)
#pragma config(Sensor, S4,     SMUX,           sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     leftTread,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     rightTread,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     flag,          tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     harvester,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     rightHang,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C4_2,     leftHang,      tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_3,    flagServo,            tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    wrist,                tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    elbow,                tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    stopper,              tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "../headers/drivers.h"

const tMUXSensor HTIRS = msensor_S4_1;
const tMUXSensor sonar = msensor_S4_2;
const tMUXSensor color = msensor_S4_3;
const tMUXSensor HTGYRO = msensor_S4_4;

#define LEFT 1
#define RIGHT -1
#define INFINITETIME 30000

#define __BACKWARDSTART__
#include "../headers/constants.h"

typedef struct{
	bool mode;
	short irDir;
	short color;
	short dist;
	short time;
	float degrees;
} SensorValues;

void initialize(SensorValues *state);
void getSensors(SensorValues *state);
void drive(SensorValues *state, int time, bool senseColor, bool senseSonar);
void turn (SensorValues *state, int dir, int amount);
void showDiagnostics(SensorValues *state);

void initialize(SensorValues *state){
	HTSPBsetupIO(HTSPB, 0xFF);
	while (externalBatteryAvg < 0){
		HTSPBwriteIO(HTSPB, B3);
		PlaySound(soundBeepBeep);
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
		PlaySound(soundBeepBeep);
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

	drive(&state, INFINITETIME, true, false); //parameters: SensorValues struct, time, use light?, use sonar?
	turn(&state, LEFT, 30); //parameters: SensorValues struct, direction, amount
	drive(&state, INFINITETIME, false, true);
	//*****score block******
	servo[elbow] = ELBOWOUT;
	wait1Msec(100);
	servo[wrist] = WRISTOUT;
	wait1Msec(300);
	servo[wrist] = WRISTIN;
	wait1Msec(100);
	servo[elbow] = ELBOWIN;
	//**********************
	if (state.mode)
		drive(&state, 1000, false, false);
}

void getSensors(SensorValues *state){
	//***********************Sonar*****************************
	state->dist = USreadDist(sonar);
	//*********************************************************

	//*****************Gyro and Accelerometer******************
	float rotSpeed = 0;
	//float x_axis, y_axis, z_axis = 0;

	//Wait until 20ms has passed
	wait1Msec(20);

	// Read the current rotation speed
	rotSpeed = HTGYROreadRot(HTGYRO);
	//HTACreadAllAxes(HTACCEL, x_axis, y_axis, z_axis);

	// Crude integration: waited 20ms so distance moved/degrees turned would be speed * time
	state->degrees += rotSpeed * 0.02;
	//state->x_distance = x_axis * 0.02;
	//state->y_distance = y_axis * 0.02;
	//state->z_distance = z_axis * 0.02;
	//*********************************************************

	//*********************Light Sensors***********************
	switch(SensorValue[color]){
		case REDCOLOR: state->color = RED; break;
		case GREENCOLOR:
		case BLUECOLOR: state->color = BLUE; break;
		case WHITECOLOR: state->color = WHITE; break;
		default:
		case BLACKCOLOR: state->color = BLACK;
	}
	//*********************************************************

	//*****************Show Debugging Window*******************
	showDiagnostics(state);
	//*********************************************************
}

void drive(SensorValues *state, int time, bool senseColor, bool senseSonar) {
	getSensors(state);
	if (senseColor)
		senseColor = (state->color == WHITE);
	if (senseSonar)
		senseSonar = state->dist < 75;

	time1[T1] = 0;
	bool stopCondition = time1[T1] > time || senseColor || senseSonar;

	while(!stopCondition){
		motor[leftTread] = COMP*DRIVESPEED;
		motor[rightTread] = DRIVESPEED;
		stopCondition = time1[T1] > time || senseColor || senseSonar;
	}

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void turn(SensorValues *state, int dir, int amount){
	state->degrees = 0;
	while(abs(state->degrees) < amount){
		getSensors(state);
		motor[leftTread] = dir*TURNSPEED;
		motor[rightTread] = -dir*TURNSPEED;
	}

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void showDiagnostics(SensorValues *state){
	//create label
	string sonarSensor = "sonar = ";
	string gyroSensor = "Gyro =";
	string timer = "Timer = ";
	string batteryLevel = "power = ";

	//store variable in a string
	string string1 = state->dist;
	string string2 = state->degrees;
	string string3 = time1[T1];
	string string4 = externalBatteryAvg;

	//concat variable with label
	strcat(sonarSensor, string1);
	strcat(gyroSensor, string2);
	strcat(timer, string3);
	strcat(batteryLevel, string4);

	eraseDisplay();

	//display label and value
	nxtDisplayTextLine(1, state->mode?"long":"short");
	nxtDisplayTextLine(2, sonarSensor);
	nxtDisplayTextLine(3, gyroSensor);
	nxtDisplayTextLine(4, timer);
	nxtDisplayTextLine(5, batteryLevel);
}
