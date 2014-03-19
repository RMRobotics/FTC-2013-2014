typedef struct {
	short delayTime;
	short currentState;

	short color;
	short color2;
	short irDir;
	short dist;
	short time;
	float degrees;
	int x_accel;
	int y_accel;
	int z_accel;

	short lastSensorCheckTime;
} RobotState;

//function prototypes
void initialize(RobotState *state);
void setDelayTime(RobotState *state);
void getSensors(RobotState *state);
void blockScorer();
void LEDController(ubyte LEDBitmask);
void resetGyroAccel(RobotState *state);
void drive(short leftSpeed, short rightSpeed);
void stopMotors();
void showDiagnostics(RobotState *state);
