typedef struct{
	short delayTime;
	short currentState;

	short color;
	short color2;
	short irDir;
	short dist;
	short time;
	float degrees;
	float x_distance;
	float y_distance;
	float z_distance;

	short lastSensorCheckTime;
} RobotState;
