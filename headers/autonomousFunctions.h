void initialize(RobotState *state){
	HTSPBsetupIO(HTSPB, 0xFF);
	while (externalBatteryAvg < 0 || HTSMUXreadPowerStatus(SMUX) || HTSMUXreadPowerStatus(SMUX2)){
		HTSPBwriteIO(HTSPB, B3);
		PlaySound(soundBeepBeep);
	}
	HTSPBwriteIO(HTSPB, 0x00);
	memset(state, 0, sizeof(state));
	motor[leftTread] = 0;
	motor[rightTread] = 0;
	servo[wrist] = WRISTIN;
	servo[elbow] = ELBOWIN;
	servo[stopper] = STOPPERIN;

	// Calibrate the gyro, make sure you hold the sensor still
	HTGYROstartCal(HTGYRO);

	setDelayTime(state);
}

void setDelayTime(RobotState *state) {
	TNxtButtons oldButton = nNxtButtonPressed;
	while(nNxtButtonPressed != 3){
		PlaySound(soundBlip);
		short nxtButtonChanged = oldButton ^ nNxtButtonPressed;
		oldButton = nNxtButtonPressed;
		if (nNxtButtonPressed == 1 && nxtButtonChanged) {
			state->delayTime += 1;
		} else if (nNxtButtonPressed == 2 && nxtButtonChanged) {
			state->delayTime -= 1;
		}
		showDiagnostics(state);
	}
}

void getSensors(RobotState *state){
	//************************IR Sensor**************************
	short _dir = 0;
	short dcS1, dcS2, dcS3, dcS4, dcS5 = 0;

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

	//Wait until 20ms has passed
	wait1Msec(20);

	// Read the current rotation speed and acceleration in each of the axis
	rotSpeed = HTGYROreadRot(HTGYRO);
	HTACreadAllAxes(HTACCEL, state->x_accel, state->y_accel, state->z_accel);

	// Crude integration: waited 20ms so degrees turned would be speed * time
	state->degrees += rotSpeed * 0.02;
	//*********************************************************

	//*********************Color Sensors***********************
	switch (HTCS2readColor(color2)) {
  case 4:
  case 2:     state->color2 = BLUE;      break;
  case 9:     state->color2 = RED;       break;
  case 17:    state->color2 = WHITE;     break;
  default:
  case 12:    state->color2 = BLACK;
  }
  switch (HTCS2readColor(color)) {
  case 4:
  case 2:     state->color = BLUE;      break;
  case 9:     state->color = RED;       break;
  case 17:    state->color = WHITE;     break;
  default:
  case 12:    state->color = BLACK;
	}
	//*********************************************************

	//*****************Show Debugging Window*******************
	showDiagnostics(state);
	//*********************************************************
}

void blockScorer(){
	drive(DRIVESPEED, DRIVESPEED);
	wait1Msec(150);
	stopMotors();
	servo[elbow] = ELBOWOUT;
	wait1Msec(500);
	servo[wrist] = WRISTOUT;
	wait1Msec(500);
	servo[wrist] = WRISTIN;
	wait1Msec(100);
	servo[elbow] = ELBOWIN;
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
}

void drive(short leftSpeed, short rightSpeed){
	motor[leftTread] = COMP*leftSpeed;
	motor[rightTread] = rightSpeed;
}

void stopMotors(){
	motor[leftTread] = 0;
	motor[rightTread] = 0;
	motor[harvester] = 0;
}

void showDiagnostics(RobotState *state){
	//create label
	string displayDelayTime = "delay = ";
	string displayState = "state = ";
	string sonarSensor = "sonar = ";
	string irSensor = "IR = ";
	string gyroSensor = "Gyro =";
	string displayColor = "Color = ";
	string displayAccel = "Accel = ";

	//store variable in a string
	string string0 = state->delayTime;
	string string1 = state->currentState;
	string string2 = state->dist;
	string string3 = state->irDir;
	string string4 = state->degrees;
	string string5 = state->color2;
	string string6 = state->x_accel;

	//concat variable with label
	strcat (displayDelayTime, string0);
	strcat(displayState, string1);
	strcat(sonarSensor, string2);
	strcat(irSensor, string3);
	strcat(gyroSensor, string4);
	strcat(displayColor, string5);
	strcat(displayAccel, string6);

	eraseDisplay();

	//display label and value
	nxtDisplayTextLine(1, displayDelayTime);
	nxtDisplayTextLine(2, displayState);
	nxtDisplayTextLine(3, sonarSensor);
	nxtDisplayTextLine(4, irSensor);
	nxtDisplayTextLine(5, gyroSensor);
	nxtDisplayTextLine(6, displayColor);
	nxtDisplayTextLine(7, displayAccel);
}
