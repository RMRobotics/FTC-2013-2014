//sensor function definitions

void getColor(RobotState *robot, tMUXSensor colorPort)
{
	if (HTCS2readColor(colorPort) >= 2 && HTCS2readColor(colorPort) <= 5)
	{
		robot->color = BLUE;
	}
	//else if (HTCS2readColor(colorPort) >= 4 && HTCS2readColor(colorPort) <= 5)
	//{
	//	robot->color = GREEN;
	//}
	else if (HTCS2readColor(colorPort) >= 7 && HTCS2readColor(colorPort) <= 9)
	{
		robot->color = RED;
	}
	else if (HTCS2readColor(colorPort) == 17)
	{
		robot->color = WHITE;
	}
	else
	{
		robot->color = BLACK;
  }
}

void getIR(RobotState *robot)
{
	robot->irDir = HTIRS2readACDir(HTIRS);
}

void getSonar(RobotState *robot)
{
	robot->dist = USreadDist(sonar);
}

void getDegrees(RobotState *robot)
{
	float rotSpeed = 0;

	// Wait until 20ms has passed
	wait1Msec(20);

	// Read the current rotation speed
	rotSpeed = HTGYROreadRot(HTGYRO);

	// Crude integration: waited 20ms so degrees turned would be speed * time
	robot->degrees += rotSpeed * 0.02;
}

void getAccel(RobotState *robot)
{
	robot->avgRobotPitch = pitchAvg;
}
