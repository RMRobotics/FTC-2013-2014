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

#define __BACKWARDSTART__
#include "../headers/drivers.h"
#include "../headers/smux_sensor_definitions.h"
#include "../headers/constants.h"
#include "../headers/RobotState.h"
#include "../headers/autonomousFunctions.h"

task main()
{
TFileHandle myFileHandle;          // create a file handle variable 'myFileHandle'
TFileIOResult IOResult;            // create an IO result variable 'IOResult'
string myFileName = "myFile.txt";  // create and initialize a string variable 'myFileName'
int myFileSize = 2000;               // create and initialize an integer variable 'myFileSize'

OpenWrite(myFileHandle, IOResult, myFileName, myFileSize);

int drivespeed = -100;
int x_accel, y_accel, z_accel;
string _tmp, _display;
time1[T1] = 0;

while (nNxtButtonPressed != 3) {
	if (nNxtButtonPressed == 1) {
		WriteString(myFileHandle, IOResult, "mark \n");
	}
	if (time1[T1] < 100) {
		stopMotors();
	} else {
		drive(drivespeed, drivespeed);
	}
	HTACreadAllAxes(HTACCEL, x_accel, y_accel, z_accel);
	StringFormat(_tmp, "%4d %4d", x_accel, y_accel);
  StringFormat(_display, "%s %4d", _tmp, z_accel);
  WriteString(myFileHandle, IOResult, _display);
  WriteString(myFileHandle, IOResult, "\n");
	wait1Msec(20);
}
time1[T1] = 0;
stopMotors();
while (time1[T1] < 100) {
	HTACreadAllAxes(HTACCEL, x_accel, y_accel, z_accel);
	StringFormat(_tmp, "%4d %4d", x_accel, y_accel);
  StringFormat(_display, "%s %4d", _tmp, z_accel);
	WriteString(myFileHandle, IOResult, _display);
	WriteString(myFileHandle, IOResult, "\n");
	wait1Msec(20);
}

Close(myFileHandle, IOResult);

}