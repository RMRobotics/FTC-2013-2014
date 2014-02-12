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
#include "drivers/hitechnic-colour-v2.h"
#include "drivers/hitechnic-superpro.h"

const tMUXSensor HTIRS = msensor_S4_1;
const tMUXSensor sonar = msensor_S4_2;
const tMUXSensor color = msensor_S4_3;
const tMUXSensor HTGYRO = msensor_S4_4;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
//                                  Drive forward 3.5s onto ramp
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


task main()
{
	waitForStart();
motor[leftTread] = -100;
motor[rightTread] = -100;
motor[leftTread2] = -100;
motor[rightTread2] = -100;
wait1Msec(3500);
motor[leftTread] = 0;
motor[rightTread] = 0;
motor[leftTread2] = -100;
motor[rightTread2] = -100;

}
