//initialization function declarations

void setDelayTime(RobotState *robot);


//initializaiton function definitions

void initialize(RobotState *robot)
{
	HTSPBsetupIO(HTSPB, 0xFF);

	while (externalBatteryAvg < 0 || HTSMUXreadPowerStatus(SMUX) || HTSMUXreadPowerStatus(SMUX2)){
		HTSPBwriteIO(HTSPB, B3);
		PlaySound(soundBeepBeep);
	}

	HTSPBwriteIO(HTSPB, 0x00);
	memset(robot, 0, sizeof(robot));
	motor[leftTread] = 0;
	motor[rightTread] = 0;
	servo[pawServo] = PAWSERVODOWN;
	servo[wrist] = WRISTOUT;
	servo[elbow] = ELBOWOUT;
	servo[stopper] = STOPPERIN;
	servo[flagServo] = FLAGSERVOIN;

	// Calibrate the gyro, make sure you hold the sensor still
	HTGYROstartCal(HTGYRO);

	// Initialize the accelerometer, it needs at least TRACK_PAST_N_VALUES*20 millisecs to do so
	StartTask(trackAccelerometerReadings);
	wait1Msec(2*TRACK_PAST_N_VALUES*20); // not necessary b/c we're waiting 5 seconds below

	disableDiagnosticsDisplay();

	setDelayTime(robot);

	//wait1Msec(5000);

	servo[wrist] = 255;
	servo[elbow] = 10;
}

void setDelayTime(RobotState *robot)
{
	eraseDisplay();

	string time = robot->delayTime;
	nxtDisplayTextLine(3, "Delay Time:");
	nxtDisplayTextLine(5, time);

	TNxtButtons oldButton = nNxtButtonPressed;
	while(nNxtButtonPressed != 3)
	{
		PlaySound(soundBlip);

		short nxtButtonChanged = oldButton ^ nNxtButtonPressed;
		oldButton = nNxtButtonPressed;

		if (nNxtButtonPressed == 1 && nxtButtonChanged)
		{
			robot->delayTime ++;

			time = robot->delayTime;
			nxtDisplayTextLine(5, time);
		}
		else if (nNxtButtonPressed == 2 && nxtButtonChanged)
		{
			robot->delayTime --;

			time = robot->delayTime;
			nxtDisplayTextLine(5, time);
		}
	}

	eraseDisplay();
}
