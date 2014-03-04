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

#include "../headers/constants.h"
#include "../headers/autonomousStruct.h"
#include "../headers/functionPrototypes.h"
#include "../headers/autonomousFunctions.h"

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
	short leftSpeed, rightSpeed;
	short prevState = 999;
	bool distLess50;
	bool irDetected = false;

	//waitForStart();
	wait1Msec(state.delayTime*1000);

	while(true){
		//if state changes: stop motors, play tone, reset timers, gyro and lights
		if (prevState != state.currentState){
			stopMotors();
			AUDIO_DEBUG(500, 10);
			time1[T1] = 0;
			resetGyroAccel(&state);
			LEDController(0x00);
			leftSpeed = 0;
			distLess50 = false;
		}

		getSensors(&state);
		prevState = state.currentState;

		if(state.currentState == FINDLINE_TURN){
			drive(0, TURNSPEED);
			if(abs(state.degrees) >= 15){
				state.currentState = FINDLINE_DRIVE;
			}
		} else if (state.currentState == FINDLINE_DRIVE) {
			drive(DRIVESPEED, DRIVESPEED);
			if(state.color == RED || state.color == BLUE){
				state.currentState = LINEFOLLOW;
			}
		} else if (state.currentState == LINEFOLLOW) {
			if (state.dist < 50) {
				distLess50 = true;
			}
			if (state.irDir == 5 && irDetected == false) {
				state.currentState = SCOREBLOCK;
			} else if (state.dist > 50 && distLess50) {
				state.currentState = PARK_TURN1;
			} else {
				if (state.color == RED || state.color == BLUE) {
					leftSpeed = DRIVESPEED*OUTLINEFOLLOWRATIO;
					rightSpeed = DRIVESPEED;
				} else {
					leftSpeed = DRIVESPEED;
					rightSpeed = DRIVESPEED*INLINEFOLLOWRATIO;
				}
			}
			drive(leftSpeed, rightSpeed);
		} else if (state.currentState == SCOREBLOCK) {
			irDetected = true;
			LEDController(B2);
			blockScorer();
			state.currentState = LINEFOLLOW;
		} else if (state.currentState == PARK_TURN1) {
			drive(TURNSPEED, -TURNSPEED);
			if(abs(state.degrees) >= 20){
				state.currentState = PARK_DRIVE1;
			}
		} else if (state.currentState == PARK_DRIVE1) {
			motor[harvester] = 100;
			drive(DRIVESPEED, DRIVESPEED);
			if(state.color == RED || state.color == BLUE){
				state.currentState = PARK_TURN2;
			}
		} else if (state.currentState == PARK_TURN2) {
			motor[harvester] = 100;
			drive(-TURNSPEED, TURNSPEED);
			if(abs(state.degrees) >= 20){
				state.currentState = PARK_DRIVE2;
			}
		} else if (state.currentState == PARK_DRIVE2) {
			drive(-2*DRIVESPEED, -2*DRIVESPEED);
			if(state.dist < 75  && time1[T1] >= 2500){
				wait1Msec(1500);
				state.currentState = END;
			}
		} else if (state.currentState == END){
			break;
		} else { //if error occured
			break;
		}
	}
}
