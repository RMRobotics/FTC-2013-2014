//movement function definitions

void driveStraightUntilHitLine(RobotState *robot, short speed)
{
	nxtDisplayTextLine(1, "driveStraightUntilHitLine");

	motor[leftTread] = speed;
	motor[rightTread] = speed;

	do
	{
		getColor(robot, (speed > 0 ? frontColorPort: backColorPort));

		string displayColor = robot->color;
		nxtDisplayTextLine(2, displayColor);
	}
	while(!(robot->color == RED || robot->color == BLUE));

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void turnCCWUntilOffLine(RobotState *robot, short speed)
{
	nxtDisplayTextLine(1, "turnCCWUntilOffLine");

	motor[leftTread] = -speed;
	motor[rightTread] = speed;

	do
	{
		getColor(robot, (speed > 0 ? frontColorPort: backColorPort));
		string displayColor = robot->color;
		nxtDisplayTextLine(2, displayColor);
	}
	while(robot->color == RED || robot->color == BLUE);

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void turnCCWUntilHitLine(RobotState *robot, short speed)
{
	nxtDisplayTextLine(1, "turnCCWUntilHitLine");

	motor[leftTread] = -speed;
	motor[rightTread] = speed;

	do
	{
		getColor(robot, (speed > 0 ? frontColorPort: backColorPort));

		string displayColor = robot->color;
		nxtDisplayTextLine(2, displayColor);
	}
	while(!(robot->color == RED || robot->color == BLUE));

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void followLineUntilOnSonar(RobotState *robot, short speed)
{
	nxtDisplayTextLine(1, "followLineUntilOnSonar");

	do
	{
		getSonar(robot);
		getColor(robot, (speed > 0 ? frontColorPort: backColorPort));

		if (robot->color == RED || robot->color == BLUE)
		{
			motor[leftTread] = speed*LINEFOLLOWRATIO;
			motor[rightTread] = speed;
		}
		else
		{
			motor[leftTread] = speed;
			motor[rightTread] = speed*LINEFOLLOWRATIO;
		}

		string displayColor = robot->color;
		string displayDist = robot->dist;
		nxtDisplayTextLine(2, displayColor);
		nxtDisplayTextLine(3, displayDist);
	}
	while(robot->dist > 75);

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void followLineUntilIRTrigger(RobotState *robot, short speed)
{
	nxtDisplayTextLine(1, "followLineUntilIRTrigger");

	do
	{
		getIR(robot);
		getColor(robot, (speed > 0 ? frontColorPort: backColorPort));
		getSonar(robot);

		if (robot->color == RED || robot->color == BLUE)
		{
			motor[leftTread] = speed*LINEFOLLOWRATIO;
			motor[rightTread] = speed;
		}
		else
		{
			motor[leftTread] = speed;
			motor[rightTread] = speed*LINEFOLLOWRATIO;
		}

		string displayColor = robot->color;
		string displayDist = robot->dist;
		nxtDisplayTextLine(2, displayColor);
		nxtDisplayTextLine(3, displayDist);
	}
	while(robot->irDir != 5 && robot->dist < 75);

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void scoreBlock(RobotState *robot)
{
	nxtDisplayTextLine(1, "scoreBlock");

	LEDController(B2);

	servo[elbow] = ELBOWOUT;
	wait1Msec(500);
	servo[wrist] = WRISTOUT;
	wait1Msec(500);
	servo[wrist] = WRISTIN;
	wait1Msec(100);
	servo[elbow] = ELBOWIN;
}

void followLineUntilSonarTrigger(RobotState *robot, short speed)
{
	nxtDisplayTextLine(1, "followLineUntilSonarTrigger");

	do {
		getSonar(robot);
		getColor(robot, (speed > 0 ? frontColorPort: backColorPort));

		if (robot->color == RED || robot->color == BLUE)
		{
			motor[leftTread] = speed*LINEFOLLOWRATIO;
			motor[rightTread] = speed;
		}
		else
		{
			motor[leftTread] = speed;
			motor[rightTread] = speed*LINEFOLLOWRATIO;
		}

		string displayColor = robot->color;
		string displayDist = robot->dist;
		nxtDisplayTextLine(2, displayColor);
		nxtDisplayTextLine(3, displayDist);
	}
	while(robot->dist < 75);

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void turnDegreesCW(RobotState *robot, short speed, short degrees)
{
	nxtDisplayTextLine(1, "turnDegreesCW");

	motor[leftTread] = speed;
	motor[rightTread] = -speed;

	robot->degrees = 0;
	do
	{
		getDegrees(robot);
		string displayDegrees = robot->degrees;
		nxtDisplayTextLine(4, displayDegrees);
	}
	while(abs(robot->degrees) < degrees);

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

void driveStraightUntilOnRamp(RobotState *robot, short speed)
{
	nxtDisplayTextLine(1, "driveStraightUntilAccel");

	motor[leftTread] = speed;
	motor[rightTread] = speed;

	time1[T1] = 0;
	do
	{
		getAccel(robot);
		string displayPitch = robot->avgRobotPitch;
		nxtDisplayTextLine(5, displayPitch);
	}
	while (robot->avgRobotPitch < 8 || time1[T1] < 500);

	wait1Msec(500); // nudge a bit forward in case we're only partially on the ramp

	motor[leftTread] = 0;
	motor[rightTread] = 0;
}

bool onRamp(RobotState *robot)
{
	nxtDisplayTextLine(1, "checkAccel");

	// wait at least TRACK_PAST_N_VALUES*20 millisecs so that even the oldest value
	// in the running average is from after the function was called
	wait1Msec(2*TRACK_PAST_N_VALUES*20);

	return robot->avgRobotPitch  > 8 ? true: false;
}
