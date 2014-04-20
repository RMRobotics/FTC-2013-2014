// task for maintaing running average of accelerometer readings

float pitchAvg;
task trackAccelerometerReadings()
{
	int instXAccel;
	int instYAccel;
	int instZAccel;
	float pitchSum;
	float instPitch[TRACK_PAST_N_VALUES];

	while(true)
	{
		pitchSum = 0;
		for (int i = TRACK_PAST_N_VALUES-2; i > 0; i--)
		{
			instPitch[i+1] = instPitch[i];
			pitchSum += instPitch[i];
		}
		HTACreadAllAxes(HTACCEL, instXAccel, instYAccel, instZAccel);
		instPitch[0] = acos(instZAccel/(sqrt(pow(instXAccel, 2) + pow(instYAccel, 2) + pow(instZAccel, 2)))) * 180.0 / PI;
		pitchSum += instPitch[0];
		pitchAvg = pitchSum/TRACK_PAST_N_VALUES;

		string a = instXAccel;
		string b = instYAccel;
		string c = instZAccel;
		string d = ((pow(instXAccel, 2) + pow(instYAccel, 2) + pow(instZAccel, 2)));
		string e = (acos(instZAccel/(sqrt(pow(instXAccel, 2) + pow(instYAccel, 2) + pow(instZAccel, 2)))) * 180.0 / PI);
		string f = pitchAvg;
		nxtDisplayTextLine(1, a);
		nxtDisplayTextLine(2, b);
		nxtDisplayTextLine(3, c);
		nxtDisplayTextLine(4, d);
		nxtDisplayTextLine(5, e);
		nxtDisplayTextLine(6, f);

		wait1Msec(20);
	}
}
