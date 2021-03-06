#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     SMUX,           sensorI2CCustom)
#pragma config(Sensor, S3,     HTSPB,          sensorI2CCustom)
#pragma config(Sensor, S4,     SMUX2,          sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     leftTread,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     rightTread,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     flag,          tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     harvester,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     rightHang,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     leftHang,      tmotorTetrix, openLoop, reversed)
#pragma config(Servo,  srvo_S1_C2_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_3,    flagServo,            tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    wrist,                tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    elbow,                tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    stopper,              tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Copyright (c) 2014 Jon Xia, RM Robotics

#include "../headers/drivers.h"
#include "../headers/smux_sensor_definitions.h"
#include "../headers/constants.h"
#define FLAGSERVOSPEED 3
#define COLORWAITTIME 500 //in milliseconds

typedef struct {
	TJoystick joy;

	short LEDindicatorArray[9];

	int x_accel;
	int y_accel;
	int z_accel;

	short leftSpeed;
	short rightSpeed;
	short harvesterSpeed;
	short leftHangSpeed;
	short rightHangSpeed;
	short flagSpeed;
	float flagServoPos;
	short stopperPos;
	short wristPos;
	short elbowPos;

	bool robotWave; //make the robot wave :)
} teleopState;

int joyPosToSpeedTable[129];

//function prototypes
void initialize(teleopState *state);
void updateJoystickInput(teleopState *state);
void updateSensorInput(teleopState *state);
void updateRobot(teleopState *state);
void stopRobot();
void showDiagnostics(teleopState *state);
int joyButton(short bitmask, int button);
void LEDController(teleopState *state);

void initialize(teleopState *state) {
	HTSPBsetupIO(HTSPB, 0x01);
	memset(state, 0, sizeof(state));
	state->wristPos = WRISTIN;
	state->elbowPos = ELBOWIN;
	state->stopperPos = STOPPEROUT;
	state->robotWave = false;
	updateRobot(state);
	stopRobot();
	time1[T1] = 0;
	//time1[T2] = 0;
	disableDiagnosticsDisplay();
}

void LEDController(teleopState *state) {
	int batteryLevel = (externalBatteryAvg - 1200)/10;
	int nxtBatteryLevel = nAvgBatteryLevel/100;
	if (externalBatteryAvg < 0) {
		batteryLevel = 0;
	} else if (batteryLevel <= 1) {
		batteryLevel = 1;
	} else if (batteryLevel >= 20) {
		batteryLevel = 20;
	}
	if (HTSMUXreadPowerStatus(SMUX)) {
		state->LEDindicatorArray[2] = 1;
	} else {
		state->LEDindicatorArray[2] = 0;
	}
	if (HTSMUXreadPowerStatus(SMUX2)) {
		state->LEDindicatorArray[3] = 1;
	} else {
		state->LEDindicatorArray[3] = 0;
	}
	state->LEDindicatorArray[0] = batteryLevel;
	state->LEDindicatorArray[1] = nxtBatteryLevel;
	HTSPBwriteIO(HTSPB, 0x01);
	for (int messageID = 0; messageID <= 8; messageID ++) {
		HTSPBwriteAnalog(HTSPB, HTSPB_DACO0, DAC_MODE_DCOUT, 1, messageID*50);
		HTSPBwriteAnalog(HTSPB, HTSPB_DACO1, DAC_MODE_DCOUT, 1, state->LEDindicatorArray[messageID]*50);
	}
	HTSPBwriteIO(HTSPB, 0x00);

	//*************************************************************

	//string display = "id = ";
	//string display2 = "val = ";
	//string string3;
	//string string4;

	//for (int i = 0; i <= 8; i++){
	//	for (int j = 0; j <=20; j++){
	//		HTSPBwriteIO(HTSPB, 0x01);
	//		HTSPBwriteAnalog(HTSPB, HTSPB_DACO0, DAC_MODE_DCOUT, 1, i*50);
	//		HTSPBwriteAnalog(HTSPB, HTSPB_DACO1, DAC_MODE_DCOUT, 1, j*50);
	//		string3 = i;
	//		string4 = j;
	//		strcat(display, string3);
	//		strcat(display2, string4);
	//		nxtDisplayTextLine(3, display);
	//		nxtDisplayTextLine(4, display2);
	//		wait1Msec(50);
	//		HTSPBwriteIO(HTSPB, 0x00);
	//		display = "id = ";
	//		display2 = "val = ":
	//	}
	//}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
// This is the main loop for the Tele-op program.
//
// The current controls to the robot are the following:
//
//               _==7==_                               _==8==_
//              / __5__ \                             / __6__ \
//            +.-'_____'-.---------------------------.-'_____'-.+
//           /   |  0  |  '.                       .'  |     |   \
//          / ___| /|\ |___ \                     / ___|(Y/4)|___ \
//         / |   7  |  1   | ;   __        __    ; |             | ;
//         | |6<---   --->2| |  |__|      |__|   | |(X/1)   (B/3)| |
//         | |___5  |  3___| ; BACK/9   START/10 ; |___       ___| ;
//         |\    | \|/ |    /  _     ___      _   \    |(A/2)|    /|
//         | \   |__4__|  .','" "', |___|  ,'" "', '.  |_____|  .' |
//         |  '-.______.-' /       \ MODE /       \  '-._____.-'   |
//         |               |       |------|       |                |
//         |              /\       /      \       /\               |
//         |             /  '.___.'        '.___.'  \              |
//         |            /                            \             |
//          \          /                              \           /
//           \________/                                \_________/
//
// Controls
//   Controller 1
//     Left Joystick:................Left tread speed (analog control)
//     Right Joystick:...............Right tread speed (analog control)
//     X/1:..........................Flag servo in
//     A/2:..........................
//     B/3:..........................Give joystick 2 drive control
//     Y/4:..........................Flag servo out
//     Left Bumper/5:................Harvester fast
//     Left Trigger/7:..,............Harvester slow
//     Right Bumper/6:...............Harvester reverse
//     Right Trigger/8:..............Flag turner
//     Tophat/D-pad
//       Up/0:.......................
//       Down/4:.....................Robot waving
//       Left/6:.....................Stopper out
//       Right/2:....................Stopper in
//   Controller 2
//     Left Joystick:................Left tread speed (analog control) (Controller 1, btn 3 must be active)
//     Right Joystick:...............Right tread speed (analog control) (Controller 1, btn 3 must be active)
//     X/1:..........................Wrist servo in
//     A/2:..........................Wrist sero out
//     B/3:..........................Elbow servo in
//     Y/4:..........................Elvow servo out
//     Left Bumper/5:................Left Hanger Up
//     Left Trigger/7:..,............Left Hanger Down
//     Right Bumper/6:...............Right Hanger Up
//     Right Trigger/8:..............Right Hanger Down
//     Tophat/D-pad
//       Up/0:.......................
//       Down/4:.....................
//       Left/6:.....................
//       Right/2:....................
//
// NOTE: ASCII Art adapted from: http://chris.com/ascii/index.php?art=video%20games/other
/////////////////////////////////////////////////////////////////////////////////////////////////////

task main()
{
	teleopState currentState;
	initialize(currentState);

	for(int n = 0; n < 129; n++){
		if(n<=16)
			joyPosToSpeedTable[n]=0;
		else if(n<=32)
			joyPosToSpeedTable[n]=10;
		else if(n<=64)
			joyPosToSpeedTable[n]= 50;
		else if(n<=96)
			joyPosToSpeedTable[n] = n * (100.0 / 128.0) + 0.5;
		else
			joyPosToSpeedTable[n]=100;
	}

	waitForStart();

	while(true){
		HTSPBsetupIO(HTSPB, 0x01);
		updateJoystickInput(currentState);
		updateSensorInput(currentState);
		updateRobot(currentState);
		showDiagnostics(currentState);
		LEDController(currentState);
	}
}

void updateJoystickInput(teleopState *state) {
	hogCPU();
	getJoystickSettings(state->joy);
	releaseCPU();

	if (joyButton(state->joy.joy1_Buttons, 3)){
		if(state->joy.joy2_y1 >= 0){
			state->leftSpeed = joyPosToSpeedTable[state->joy.joy2_y1];
		}else{
			state->leftSpeed = -joyPosToSpeedTable[-state->joy.joy2_y1];
		}
		if(state->joy.joy2_y2 >= 0){
			state->rightSpeed = joyPosToSpeedTable[state->joy.joy2_y2];
		}else{
			state->rightSpeed = -joyPosToSpeedTable[-state->joy.joy2_y2];
		}
	}else{
		if(state->joy.joy1_y1 >= 0){
			state->leftSpeed = joyPosToSpeedTable[state->joy.joy1_y1];
		}else{
			state->leftSpeed = -joyPosToSpeedTable[-state->joy.joy1_y1];
		}
		if(state->joy.joy1_y2 >= 0){
			state->rightSpeed = joyPosToSpeedTable[state->joy.joy1_y2];
		}else{
			state->rightSpeed = -joyPosToSpeedTable[-state->joy.joy1_y2];
		}
	}

	if (joyButton(state->joy.joy1_Buttons, 5)) {
		state->harvesterSpeed = 100;
		state->LEDindicatorArray[6] = 1;
	} else if (joyButton(state->joy.joy1_Buttons, 7)) {
		state->harvesterSpeed = 50;
		state->LEDindicatorArray[6] = 2;
	} else if (joyButton(state->joy.joy1_Buttons, 6)) {
		state->harvesterSpeed = -100;
		state->LEDindicatorArray[6] = 3;
	//}	else if (time1[T2] > 100 && time1[T2] < 700) { //turn the harvester a bit when stopper is drawn out
	//	state->harvesterSpeed = 100;
	} else {
		state->harvesterSpeed = 0;
		state->LEDindicatorArray[6] = 0;
	}

	if (joyButton(state->joy.joy1_Buttons, 8)){
		state->flagSpeed = 100;
	} else {
		state->flagSpeed = 0;
	}

	if (joyButton(state->joy.joy2_Buttons, 7)) {
		state->rightHangSpeed = 100;
		state->LEDindicatorArray[4] = 1;
	} else if (joyButton(state->joy.joy2_Buttons, 5)) {
		state->rightHangSpeed = -100;
		state->LEDindicatorArray[4] = 0;
	} else {
		state->rightHangSpeed = 0;
		state->LEDindicatorArray[4] = 0;
	}

	if (joyButton(state->joy.joy2_Buttons, 8)) {
		state->leftHangSpeed = 100;
		state->LEDindicatorArray[4] = 1;
	} else if (joyButton(state->joy.joy2_Buttons, 6)) {
		state->leftHangSpeed = -100;
		state->LEDindicatorArray[4] = 0;
	} else {
		state->leftHangSpeed = 0;
		state->LEDindicatorArray[4] = 0;
	}

	//flag servo
	if (joyButton(state->joy.joy1_Buttons, 4) && state->flagServoPos < FLAGSERVOOUT) {
		state->flagServoPos += FLAGSERVOSPEED;
	} else if (joyButton(state->joy.joy1_Buttons, 1 && state->flagServoPos > FLAGSERVOIN)) {
		state->flagServoPos -= FLAGSERVOSPEED;
	}

	//stopper servo
	if (state->joy.joy1_TopHat == 2) {
		state->stopperPos = STOPPERIN;
	} else if (state->joy.joy1_TopHat == 6) {
		state->stopperPos = STOPPEROUT;
		//time1[T2] = 0;
	}
	if (state->stopperPos == STOPPERIN){
		state->LEDindicatorArray[5] = 1;
	} else {
		state->LEDindicatorArray[5] = 0;
	}

	//wrist servo
	if (joyButton(state->joy.joy2_Buttons, 1)) {
		state->wristPos = WRISTIN;
		state->robotWave = false;
	} else if (joyButton(state->joy.joy2_Buttons, 2)) {
		state->wristPos = WRISTOUT;
		state->robotWave = false;
	}

	//elbow servo
	if (joyButton(state->joy.joy2_Buttons, 3)) {
		state->elbowPos = ELBOWIN;
	} else if (joyButton(state->joy.joy2_Buttons, 4)) {
		state->elbowPos = ELBOWOUT;
	}

	//robot waving
	if (state->joy.joy1_TopHat == 0) {
		state->robotWave = true;
		time1[T3] = 0;
	} else if (state->joy.joy1_TopHat == 4) {
		state->robotWave = false;
		state->wristPos = WRISTIN;
	}
	if (state->robotWave) {
		if (time1[T3] < 250) {
			state->wristPos = WRISTOUT;
		} else if (time1[T3] < 500) {
			state->wristPos = 150;
		} else {
			time1[T3] = 0;
		}
		state->LEDindicatorArray[7] = 1;
	} else {
		state->LEDindicatorArray[7] = 0;
	}

	//robot horn
	if (joyButton(state->joy.joy1_Buttons, 2)) {
		PlayTone(700, 8);
	}
}

void updateSensorInput(teleopState *state) {
	int lightValue = LSvalNorm(light);
	if (lightValue > 50) {
		if (time1[T1] > COLORWAITTIME) {
			state->LEDindicatorArray[8] = 1;
		}
	} else {
		time1[T1] = 0;
		state->LEDindicatorArray[8] = 0;
	}
	HTACreadAllAxes(HTACCEL, state->x_accel, state->y_accel, state->z_accel); //
}

int joyButton(short bitmask, int button)
{
	//return 1 or 0 based on whether button in bitmask is pressed or not
	return bitmask & (1 << (button - 1));
}

void updateRobot(teleopState *state) {
	motor[leftTread]=state->leftSpeed;
	motor[rightTread]=state->rightSpeed;
	motor[harvester]=state->harvesterSpeed;
	motor[leftHang]=state->leftHangSpeed;
	motor[rightHang]=state->rightHangSpeed;
	motor[flag]=state->flagSpeed;
	servo[flagServo] = state->flagServoPos;
	servo[wrist]=state->wristPos;
	servo[elbow]=state->elbowPos;
	servo[stopper]=state->stopperPos;
	//LEDController(state->LEDBitmask);
}

void stopRobot() {
	motor[leftTread]=0;
	motor[rightTread]=0;
	motor[harvester]=0;
	motor[leftHang]=0;
	motor[rightHang]=0;
	motor[flag]=0;
}

void showDiagnostics(teleopState *state) {
	//create label
	string batteryLevel = "power = ";
	string nxtBatteryLevel = "nxtPower = ";
	string displayXAccel = "x-accel = ";
	string displayYAccel = "y-accel = ";
	string displayZAccel = "z-accel = ";

	//store variable in a string
	string string1 = externalBatteryAvg;
	string string2 = nAvgBatteryLevel;
	string string3 = state->x_accel;
	string string4 = state->y_accel;
	string string5 = state->z_accel;

	//concat variable with label
	strcat(batteryLevel, string1);
	strcat(nxtBatteryLevel, string2);
	strcat(displayXAccel, string3);
	strcat(displayYAccel, string4);
	strcat(displayZAccel, string5);

	eraseDisplay();

	//display label and value
	nxtDisplayTextLine(1, "Teleop");
	nxtDisplayTextLine(2, batteryLevel);
	nxtDisplayTextLine(3, nxtBatteryLevel);
	nxtDisplayTextLine(4, displayXAccel);
	nxtDisplayTextLine(5, displayYAccel);
	nxtDisplayTextLine(6, displayZAccel);
}
