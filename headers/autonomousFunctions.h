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
	servo[pawServo] = PAWSERVODOWN;
	servo[wrist] = WRISTOUT;
	servo[elbow] = ELBOWOUT;
	servo[stopper] = STOPPERIN;

	// Calibrate the gyro, make sure you hold the sensor still
	HTGYROstartCal(HTGYRO);

	disableDiagnosticsDisplay();

	setDelayTime(state);

	wait1Msec(5000);
	servo[wrist] = 255;
	servo[elbow] = 10;
}

void setDelayTime(RobotState *state) {
	TNxtButtons oldButton = nNxtButtonPressed;
	while(nNxtButtonPressed != 3){
		PlaySound(soundBlip);
		short nxtButtonChanged = oldButton ^ nNxtButtonPressed;
		oldButton = nNxtButtonPressed;
		if (nNxtButtonPressed == 1 && nxtButtonChanged) {
			state->delayTime ++;
		} else if (nNxtButtonPressed == 2 && nxtButtonChanged) {
			state->delayTime --;
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
	if (HTCS2readColor(color2) >= 2 && HTCS2readColor(color2) <= 3) {
		state->color2 = BLUE;
	} else if (HTCS2readColor(color2) >= 4 && HTCS2readColor(color2) <= 5) {
		state->color2 = GREEN;
	} else if (HTCS2readColor(color2) >= 7 && HTCS2readColor(color2) <= 9) {
		state->color2 = RED;
	} else if (HTCS2readColor(color2) == 17) {
		state->color2 = WHITE;
	} else {
		state->color2 = BLACK;
  	}
	if (HTCS2readColor(color) >= 2 && HTCS2readColor(color) <= 3) {
		state->color = BLUE;
	} else if (HTCS2readColor(color) >= 4 && HTCS2readColor(color) <= 5) {
		state->color = GREEN;
	} else if (HTCS2readColor(color) >= 7 && HTCS2readColor(color) <= 9) {
		state->color = RED;
	} else if (HTCS2readColor(color) == 17) {
		state->color = WHITE;
	} else {
		state->color = BLACK;
  	}
	//*********************************************************

	//*****************Show Debugging Window*******************
	showDiagnostics(state);
	//*********************************************************
}

void blockScorer(){
	drive(DRIVESPEED, DRIVESPEED);
	WAITSPECIAL();
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
