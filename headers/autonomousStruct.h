typedef struct{
	short delayTime;
	short currentState;

	short color;
	short irDir;
	short dist;
	short time;
	float degrees;
	//float x_distance;
	//float y_distance;
	//float Z_distance;

	short lastSensorCheckTime;
} RobotState;
