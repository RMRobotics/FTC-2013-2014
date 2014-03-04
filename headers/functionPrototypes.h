//function prototypes
void initialize(RobotState *state);
void getSensors(RobotState *state);
void blockScorer();
void LEDController(ubyte LEDBitmask);
void resetGyroAccel(RobotState *state);
void drive(short leftSpeed, short rightSpeed);
void stopMotors();
void showDiagnostics(RobotState *state);
