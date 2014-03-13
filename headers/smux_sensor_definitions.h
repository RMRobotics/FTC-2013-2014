//1st Multiplexer Sensors
const tMUXSensor HTACCEL = msensor_S2_2;

//2nd Multiplexer Sensors
const tMUXSensor HTIRS = msensor_S4_1;
const tMUXSensor sonar = msensor_S4_2;
const tMUXSensor HTGYRO = msensor_S4_4;

//if going backward switch color and color2 sensor definitions
#ifdef __BACKWARDSTART__
const tMUXSensor color = msensor_S2_1;
const tMUXSensor color2 = msensor_S4_3;
#else
const tMUXSensor color2 = msensor_S2_1;
const tMUXSensor color = msensor_S4_3;
#endif
