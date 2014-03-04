#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     light1,         sensorLightActive)
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
#define FLAGSERVOSPEED 3
#define COLORWAITTIME 500 //in milliseconds

typedef struct {
	TJoystick joy;

	ubyte LEDBitmask;

	int leftSpeed;
	int rightSpeed;
	int harvesterSpeed;
	int leftHangSpeed;
	int rightHangSpeed;
	int flagSpeed;
	float flagServoPos;
	int stopperPos;
	int wristPos;
	int elbowPos;
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
void LEDController(ubyte LEDBitmask);

void initialize(teleopState *state) {
	HTSPBsetupIO(HTSPB, 0xFF);
	memset(state, 0, sizeof(state));
	state->wristPos = WRISTIN;
	state->elbowPos = ELBOWIN;
	state->stopperPos = STOPPEROUT;
	updateRobot(state);
	stopRobot();
	time1[T1] = 0;
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
//     A/2:..........................Flag servo out
//     B/3:..........................Give joystick 2 drive control
//     Y/4:..........................
//     Left Bumper/5:................Harvester fast
//     Left Trigger/7:..,............Harvester slow
//     Right Bumper/6:...............Harvester reverse
//     Right Trigger/8:..............Flag turner
//     Tophat/D-pad
//       Up/0:.......................
//       Down/4:.....................
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
		HTSPBsetupIO(HTSPB, 0xFF);
		currentState.LEDBitmask = 0x00; //clear LEDBitmask
		updateJoystickInput(currentState);
		updateSensorInput(currentState);
		updateRobot(currentState);
		showDiagnostics(currentState);
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
	} else if (joyButton(state->joy.joy1_Buttons, 7)) {
		state->harvesterSpeed = 50;
	} else if (joyButton(state->joy.joy1_Buttons, 6)) {
		state->harvesterSpeed = -100;
	}	else {
		state->harvesterSpeed = 0;
	}

	if (joyButton(state->joy.joy1_Buttons, 8)){
		state->flagSpeed = 100;
	} else {
		state->flagSpeed = 0;
	}

	if (joyButton(state->joy.joy2_Buttons, 7)) {
		state->rightHangSpeed = 100;
		state->LEDBitmask = state->LEDBitmask | B4;
	} else if (joyButton(state->joy.joy2_Buttons, 5)) {
		state->rightHangSpeed = -100;
	} else {
		state->rightHangSpeed = 0;
	}

	if (joyButton(state->joy.joy2_Buttons, 8)) {
		state->leftHangSpeed = 100;
		state->LEDBitmask = state->LEDBitmask | B4;
	} else if (joyButton(state->joy.joy2_Buttons, 6)) {
		state->leftHangSpeed = -100;
	} else {
		state->leftHangSpeed = 0;
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
	}
	if (state->stopperPos == STOPPERIN){
		state->LEDBitmask = state->LEDBitmask | B5;
	}

	//wrist servo
	if (joyButton(state->joy.joy2_Buttons, 1)) {
		state->wristPos = WRISTIN;
	} else if (joyButton(state->joy.joy2_Buttons, 2)) {
		state->wristPos = WRISTOUT;
	}

	//elbow servo
	if (joyButton(state->joy.joy2_Buttons, 3)) {
		state->elbowPos = ELBOWIN;
	} else if (joyButton(state->joy.joy2_Buttons, 4)) {
		state->elbowPos = ELBOWOUT;
	}
}

void updateSensorInput(teleopState *state) {
	int red, green, blue;
	if (!HTCS2readRGB(color, red, green, blue)) {
		//do nothing
	}
	if (red - blue > 10 && green - blue > 10) {
		if (time1[T1] > COLORWAITTIME)
			state->LEDBitmask = state->LEDBitmask | B3;
	} else {
		time1[T1] = 0;
	}
}

int joyButton(short bitmask, int button)
{
	//return 1 or 0 based on whether button in bitmask is pressed or not
	return bitmask & (1 << (button - 1));
}

void LEDController(ubyte LEDBitmask) {
	//B2 (0x04): IR detected output address (only use in autonomous)
	//B3 (0x08): Block Detected output address
	//B4 (0x0F): Robot Lifting output address
	//B5 (0x20): Stopper In output address
	HTSPBwriteIO(HTSPB, LEDBitmask);
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
	LEDController(state->LEDBitmask);
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

	//store variable in a string
	string string5 = externalBatteryAvg;

	//concat variable with label
	strcat(batteryLevel, string5);

	eraseDisplay();

	//display label and value
	nxtDisplayTextLine(3, "Teleop");
	nxtDisplayTextLine(5, batteryLevel);
}