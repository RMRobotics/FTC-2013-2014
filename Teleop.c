#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Sensor, S2,     HTIRS,          sensorI2CCustom)
#pragma config(Sensor, S3,     HTGYRO,         sensorI2CCustom)
#pragma config(Sensor, S4,     SMUX,           sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     leftTread,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     rightTread,    tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_1,     harvester,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     leftHang,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     rightHang,        tmotorTetrix, openLoop, reversed)
#pragma config(Servo,  srvo_S1_C3_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-irseeker-v1.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/hitechnic-accelerometer.h"

const tMUXSensor HTACCEL = msensor_S4_1;
const tMUXSensor sonar = msensor_S4_2;
const tMUXSensor color = msensor_S4_3;

typedef struct {
	TJoystick joy;

	int leftSpeed;
	int rightSpeed;
	int harvesterSpeed;
	int leftHangS;
	int rightHangS;
	int stopper;
	int auto;
} joyState;

int speedRef[129];

void initialize(joyState *state);
void updateInput(joyState *state);
void updateRobot(joyState *state);
void stopRobot(joyState *state);
void showDiagnostics(joyState *state);
int joyButton(short bitmask, int button);

task main()
{
	joyState currentState;
	initialize(currentState);

	for(int n = 0; n < 129; n++){
		if(n<=16)
			speedRef[n]=0;
		else if(n<=32)
			speedRef[n]=10;
		else if(n<=64)
			speedRef[n]= 50;
		else if(n<=96)
			speedRef[n] = n * (100.0 / 128.0) + 0.5;
		else
			speedRef[n]=100;
	}

	//waitForStart();

	while(true){
		updateInput(currentState);
		updateRobot(currentState);
	}
}

void initialize(joyState *state) {
	memset(state, 0, sizeof(state));
}

void updateInput(joyState *state) {
	getJoystickSettings(joystick);
	memcpy(state->joy, joystick, sizeof(joystick));

	state->leftSpeed = state->joy.joy1_y2/abs(state->joy.joy1_y2)*speedRef[abs(state->joy.joy1_y2)];
	state->rightSpeed = state->joy.joy1_y1/abs(state->joy.joy1_y1)*speedRef[abs(state->joy.joy1_y1)];

	if (joyButton(state->joy.joy1_Buttons, 1)) {
		state->harvesterSpeed = 100;
	} else if (joyButton(state->joy.joy1_Buttons, 2)) {
		state->harvesterSpeed = 50;
	} else {
		state->harvesterSpeed = 0;
	}

	if (joyButton(state->joy.joy1_Buttons, 7)) {
		state->leftHangS = 100;
	} else if (joyButton(state->joy.joy1_Buttons, 5)) {
		state->leftHangS = -100;
	} else {
		state->leftHangS = 0;
	}

	if (joyButton(state->joy.joy1_Buttons, 8)) {
		state->rightHangS = 100;
	} else if (joyButton(state->joy.joy1_Buttons, 6)) {
		state->rightHangS = -100;
	} else {
		state->rightHangS = 0;
	}

	if (joyButton(state->joy.joy1_Buttons, 4)) {
		state->stopper = 255;
	} else if (joyButton(state->joy.joy1_Buttons, 3)) {
		state->stopper = 120;
	}

	if (state->joy.joy1_TopHat == 0) {
		state->auto = 240;
	} else if (state->joy.joy1_TopHat == 4) {
		state->auto = 0;
	} else if (state->joy.joy1_TopHat == 2) {
		state->auto = 120;
	}

}

int joyButton(short bitmask, int button)
{
	//return 1 or 0 based on whether button in bitmask is pressed or not
	return bitmask & (1 << (button - 1));
}

void updateRobot(joyState *state) {
	motor[rightTread]=state->rightSpeed;
	motor[leftTread]=state->leftSpeed;
	motor[harvester]=state->harvesterSpeed;
	motor[leftHang]=state->leftHangS;
	motor[rightHang]=state->rightHangS;
	servo[servo6]=state->stopper;
	servo[servo5]=state->auto;
}

void stopRobot(joyState *state) {
	motor[rightTread]=0;
	motor[leftTread]=0;
	motor[harvester]=0;
	motor[leftHang]=0;
	motor[rightHang]=0;
	servo[servo6]=0;
	servo[servo5]=0;
}

void showDiagnostics(joyState *state) {
	eraseDisplay();
	nxtDisplayTextLine(1, "TeleOp");
}
