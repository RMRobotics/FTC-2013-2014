typedef struct{
	short delayTime;
	short currentState;

	short lightVal1;
	short lightVal2;
	short irDir;
	short dist;
	short time;
	float degrees;
	//float x_distance;
	//float y_distance;
	//float Z_distance;

	short lastSensorCheckTime;
} RobotState;
