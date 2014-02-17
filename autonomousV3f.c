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

#define AUDIO_DEBUG_ENABLED
//#define TIMEOUTS_ENABLED

#define FINDLINE 0
#define LINEFOLLOWER1 1
#define SCOREBLOCK 2
#define LINEFOLLOWER2 3
#define GOTOEND 4
#define PARKSEQUENCE1 5
#define PARKSEQUENCE2 6
#define PARKSEQUENCE3 7
#define PARKSEQUENCE4 8
#define END 9
#define FAIL 10

#define TURNSPEED 75
#define DRIVESPEED 50
#define COMP .99

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

#ifdef AUDIO_DEBUG_ENABLED
#define AUDIO_DEBUG(frequency, duration) PlayTone(frequency, duration)
#else
#define AUDIO_DEBUG(frequency, duration)
#endif

#ifdef TIMEOUTS_ENABLED
#define TIMEOUT(time) time1[T1] > time
#else
#define TIMEOUT(time) time1[T1] > 100000000
#endif

typedef struct{
	int currentState;

	int irDir;
	int dist;
	int time;
	float degrees;
	//float x_distance;
	//float y_distance;
	//float Z_distance;

	int lastSensorCheckTime;
} RobotState;

void initialize(RobotState *state);
void getSensors(RobotState *state);
void LEDController(ubyte LEDBitmask);
void resetGyroAccel(RobotState * state);
void drive(int leftSpeed, int rightSpeed);
void stopMotors();
void showDiagnostics(RobotState *state);

void initialize(RobotState *state){
	HTSPBsetupIO(HTSPB, 0xFF);
	memset(state, 0, sizeof(state));
	motor[leftTread] = 0;
	motor[rightTread] = 0;
	servo[wrist] = WRISTIN;
	servo[elbow] = ELBOWIN;
	servo[stopper] = STOPPERIN;

	// Calibrate the gyro, make sure you hold the sensor still
	HTGYROstartCal(HTGYRO);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

task main() {
	RobotState state;
	initialize(&state);
	int leftSpeed = 0;
	int rightSpeed = 0;
	int numTimeGreater50 = 0;
	bool special;

	//waitForStart();
	//state.lastSensorCheckTime = time1[T2];

	AUDIO_DEBUG(500, 10);
	time1[T1] = 0;
	while(state.currentState == FINDLINE){
		getSensors(&state);
		drive(DRIVESPEED, DRIVESPEED);
		if(TIMEOUT(3000)){
			state.currentState = FAIL;
			}else if(state.irDir == 5){
			state.currentState = SCOREBLOCK;
			}else if(state.dist <= 50){
			state.currentState = LINEFOLLOWER1;
		}
	}

	AUDIO_DEBUG(500, 10);
	time1[T1] = 0;
	while(state.currentState == LINEFOLLOWER1){
		getSensors(&state);
		drive(leftSpeed, rightSpeed);
		if(time1[T1] > 2000){
			if(state.dist > 50){
				leftSpeed = 25;
				rightSpeed = 25;
				numTimeGreater50++;
				if(numTimeGreater50 > 10) {
					state.currentState = GOTOEND;
				}
			} else {
				numTimeGreater50 = 0;
			}
		}
		if(state.irDir == 5){
			state.currentState = SCOREBLOCK;
			if (time1[T1] >= 3000)
				special = true;
		}else if(state.dist < INNERSONARBOUND){
			leftSpeed  = DRIVESPEED*INLINEFOLLOWRATIO;
			rightSpeed = DRIVESPEED;
		}else if(state.dist > OUTERSONARBOUND){
			leftSpeed = DRIVESPEED;
			rightSpeed = DRIVESPEED*OUTLINEFOLLOWRATIO;
		}else { //in range
			leftSpeed = DRIVESPEED;
			rightSpeed = DRIVESPEED;
		}
	}
	numTimeGreater50 = 0;

	AUDIO_DEBUG(500,10);
	time1[T1] = 0;
	LEDController(B2);
	while(state.currentState == SCOREBLOCK){
		getSensors(&state);
		//if(!special){
		//	wait1Msec(500);
		//}
		stopMotors();
		servo[elbow] = ELBOWOUT;
		wait1Msec(100);
		servo[wrist] = WRISTOUT;
		wait1Msec(500);
		servo[wrist] = WRISTIN;
		wait1Msec(100);
		servo[elbow] = ELBOWIN;
		state.currentState = LINEFOLLOWER2;
	}
	LEDController(0x00);

	AUDIO_DEBUG(500,10);
	time1[T1] = 0;
	while(state.currentState == LINEFOLLOWER2){
		getSensors(&state);
		drive(leftSpeed, rightSpeed);
		if(state.dist >= 50){
			leftSpeed = 25;
			rightSpeed = 25;
			numTimeGreater50++;
			if(numTimeGreater50 > 10){
				state.currentState = GOTOEND;
			}
		} else if(state.dist > OUTERSONARBOUND){
			leftSpeed  = DRIVESPEED;
			rightSpeed = DRIVESPEED*OUTLINEFOLLOWRATIO;
			numTimeGreater50 = 0;
		}else if(state.dist < INNERSONARBOUND){
			leftSpeed = DRIVESPEED*INLINEFOLLOWRATIO;
			rightSpeed = DRIVESPEED;
			numTimeGreater50 = 0;
		}else if(TIMEOUT(1500)){
			state.currentState = GOTOEND;
		}else { //in range
			leftSpeed = DRIVESPEED;
			rightSpeed = DRIVESPEED;
			numTimeGreater50 = 0;
		}
	}

	AUDIO_DEBUG(500,10);
	time1[T1] = 0;
	while(state.currentState == GOTOEND){
		getSensors(&state);
		leftSpeed = -DRIVESPEED;
		rightSpeed = -DRIVESPEED;
		drive(leftSpeed, rightSpeed);
		if(time1[T1] >= 300){
			state.currentState = PARKSEQUENCE1;
		}
	}

	state.currentState = PARKSEQUENCE1;
	AUDIO_DEBUG(500,1);
	time1[T1] = 0;
	resetGyroAccel(&state);
	while(state.currentState == PARKSEQUENCE1){
		getSensors(&state);
		drive(TURNSPEED, -TURNSPEED);
		if(abs(state.degrees) >= 20 || TIMEOUT(500)){
			stopMotors();
			state.currentState = PARKSEQUENCE2;
		}
	}

	AUDIO_DEBUG(500,10);
	time1[T1] = 0;
	motor[harvester] = 100;
	while(state.currentState == PARKSEQUENCE2){
		getSensors(&state);
		drive(DRIVESPEED, DRIVESPEED);
		if(time1[T1] > 6000){
			state.currentState = PARKSEQUENCE3;
		}
	}

	AUDIO_DEBUG(500,10);
	time1[T1] = 0;
	resetGyroAccel(&state);
	while(state.currentState == PARKSEQUENCE3){
		getSensors(&state);
		drive(-TURNSPEED, TURNSPEED);
		if(abs(state.degrees) >= 20 || TIMEOUT(500)){
			stopMotors();
			state.currentState = PARKSEQUENCE4;
		}
	}
	motor[harvester] = 0;

	AUDIO_DEBUG(500,10);
	time1[T1] = 0;
	while(state.currentState == PARKSEQUENCE4){
		getSensors(&state);
		drive(-2*DRIVESPEED, -2*DRIVESPEED);
		if(state.dist < 75  && time1[T1] >= 2500){
			wait1Msec(1500);
			stopMotors();
			state.currentState = END;
		}
	}

	AUDIO_DEBUG(500,10);
	while(state.currentState == FAIL){
		stopMotors();
	}

	AUDIO_DEBUG(500,10);
	while(state.currentState == END){
		stopMotors();
	}
}

void getSensors(RobotState *state){

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
	// exit the app if an error occurs
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
	//float x_axis, y_axis, z_axis = 0;

	//Wait until 20ms has passed
	wait1Msec(20);
	/*state->lastSensorCheckTime += 20;
	int waitTime = state->lastSensorCheckTime-time1[T2];
	if (waitTime >= 0 && waitTime <= 20) {
	wait1Msec(waitTime);
	} else {
	AUDIO_DEBUG(700, 1000);
	eraseDisplay();
	string errorDisplay;
	sprintf(errorDisplay, "missed %d",(int)(time1[T2] - state->lastSensorCheckTime));
	nxtDisplayTextLine(3, errorDisplay);
	stopMotors();
	wait1Msec(100000000000);
	state->lastSensorCheckTime = time1[T2];
	}*/


	// Read the current rotation speed
	rotSpeed = HTGYROreadRot(HTGYRO);
	//HTACreadAllAxes(HTACCEL, x_axis, y_axis, z_axis);

	// Crude integration: waited 20ms so distance moved/degrees turned would be speed * time
	state->degrees += rotSpeed * 0.02;
	//state->x_distance = x_axis * 0.02;
	//state->y_distance = y_axis * 0.02;
	//state->z_distance = z_axis * 0.02;

	//*********************************************************

	//*****************Show Debugging Window*******************

	showDiagnostics(state);

	//*********************************************************
}

void LEDController(ubyte LEDBitmask) {
	//B2 (0x04 or B2): IR detected output address (only use in autonomous)
	//B3 (0x08 or B3): block detected output address
	//B4 (0x0F or B4): robot lifting output address
	//B5 (0x20 or B5): stopper in output address
	HTSPBwriteIO(HTSPB, LEDBitmask);
}

void resetGyroAccel(RobotState *state){
	state->degrees = 0;
	//state->x_distance = 0;
	//state->y_distance = 0;
	//state->z_distance = 0;
}

void drive(int leftSpeed, int rightSpeed){
	motor[leftTread] = COMP*leftSpeed;
	motor[rightTread] = rightSpeed;
	motor[leftTread2] = COMP*leftSpeed;
	motor[rightTread2] = rightSpeed;
}

void stopMotors(){
	motor[leftTread] = 0;
	motor[rightTread] = 0;
	motor[leftTread2] = 0;
	motor[rightTread2] = 0;
	motor[harvester] = 0;
}

void showDiagnostics(RobotState *state){
	//create label
	string displayState = "state = ";
	string sonarSensor = "sonar = ";
	string irSensor = "IR = ";
	string gyroSensor = "Gyro =";
	string timer = "Timer = ";
	string batteryLevel = "power = ";

	//store variable in a string
	string string0 = state->currentState;
	string string1 = state->dist;
	string string2 = state->irDir;
	string string3 = state->degrees;
	string string4 = time1[T1];
	string string5 = externalBatteryAvg;

	//concat variable with label
	strcat(displayState, string0);
	strcat(sonarSensor, string1);
	strcat(irSensor, string2);
	strcat(gyroSensor, string3);
	strcat(timer, string4);
	strcat(batteryLevel, string5);

	eraseDisplay();

	//display label and value
	nxtDisplayTextLine(1, displayState);
	nxtDisplayTextLine(2, sonarSensor);
	nxtDisplayTextLine(3, irSensor);
	nxtDisplayTextLine(4, gyroSensor);
	nxtDisplayTextLine(5, timer);
	nxtDisplayTextLine(6, batteryLevel);
}
