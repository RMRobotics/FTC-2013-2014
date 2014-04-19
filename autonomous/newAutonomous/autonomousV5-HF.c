#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     SMUX,           sensorI2CCustom)
#pragma config(Sensor, S3,     HTSPB,          sensorI2CCustom)
#pragma config(Sensor, S4,     SMUX2,          sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     leftTread,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     rightTread,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     flag,          tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     harvester,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     rightHang,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C4_2,     leftHang,      tmotorTetrix, openLoop, reversed)
#pragma config(Servo,  srvo_S1_C2_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_2,    pawServo,             tServoNone)
#pragma config(Servo,  srvo_S1_C2_3,    flagServo,            tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    wrist,                tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    elbow,                tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    stopper,              tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Copyright (c) 2014 Jon Xia, RM Robotics

#include "../../headers/newHeaders/drivers.h"
#include "../../headers/newHeaders/sensor_mux_definitions.h"
#include "../../headers/newHeaders/constants.h"
#include "../../headers/newHeaders/RobotState.h"
#include "../../headers/newHeaders/trackAccelerometerReadingsTask.h"
#include "../../headers/newHeaders/initializationFunctions.h"
#include "../../headers/newHeaders/sensorFunctions.h"
#include "../../headers/newHeaders/lightIndicatorFunction.h"
#include "../../headers/newHeaders/movementFunctions.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

task main() {
	RobotState robot;
	initialize(&robot);

	/*
	.......................................................
	|`.         ,-       A                  `.         ,' |
	|         ,'                              `.     ,'   |
	|        /                                  `._,'     |
	|      ,'                                    ,'`.     |
	|    ,'              B.                    ,'    `.   |
	|  ,'              ,'  =                 ,'        `. |
	| /              ,'   /  ,.            D'            '|
	|'             ,'   ,' ,'  `.        ,'               |
	|            ,'   ,'  /      `._   ,'                 |
	|           /   ,'  ,'          `,'                   |
	|         C'  ,'  ,'           ,' `.                  |
	|          `.,   /           /'     `.                |
	|              ,'          E'         `._             |
	|              `.         /             / _           |
	|                `.     ,'            ,',- `.         |
	|                  `. ,'            ,' /   ,'         |
	|                   ,'._           / ,'  ,'           |
	|                 ,'    `.       ,','  ,'             |
	|_              ,'        `.   ,','   /               |
	| \           ,'            `>' /   ,'              , |
	|  `.       ,'                 '  ,'              ,'  |
	|    `.   ,'                    ''               /    |
	|      `,'                                     ,'     |
	|     ,' `.                                  ,'       |
	|   ,'     `.                               /         |
	| ,'         `.                           ,'       `. |
	''-------------`-------------------------'-----------`'
	*/

	// Start at A and go to B
	driveStraightUntilHitLine(&robot, SLOWSPEED);
	// Turn until parallel to BC
	turnCCWUntilOffLine(&robot, TURNSPEED);
	// Drive on BC until next to wall
	followLineUntilOnSonar(&robot, DRIVESPEED);
	// Drive to IR beacon on BC and score
	followLineUntilIRTrigger(&robot, DRIVESPEED);
	scoreBlock(&robot);
	// Drive on CB until back on wall
	followLineUntilOnSonar(&robot, -DRIVESPEED);
	// DRIVE back to B
	followLineUntilSonarTrigger(&robot, -DRIVESPEED);
	// Turn until parallel to BD
	turnDegreesCW(&robot, -TURNSPEED, 45);
	// Drive to D
	driveStraightUntilHitLine(&robot, -DRIVESPEED);
	// Turn until parallel to DE
	turnDegreesCW(&robot, -TURNSPEED, 80);
	// Drive to E
	driveStraightUntilAccel(&robot, -DRIVESPEED);
}
