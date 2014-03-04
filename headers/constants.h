#define AUDIO_DEBUG_ENABLED
//#define TIMEOUTS_ENABLED

#define FINDLINE_TURN 0
#define FINDLINE_DRIVE 1
#define LINEFOLLOW 2
#define SCOREBLOCK 3
#define GOTOEND 4
#define PARK_TURN1 5
#define PARK_DRIVE1 6
#define PARK_TURN2 7
#define PARK_DRIVE2 8
#define END 9

#ifdef __BACKWARDSTART__
	#define TURNSPEED -75
	#define DRIVESPEED -50
#else
	#define TURNSPEED 75
	#define DRIVESPEED 50
#endif

#define COMP .99

#define INLINEFOLLOWRATIO .5
#define OUTLINEFOLLOWRATIO .5
//#define INNERSONARBOUND 25
//#define OUTERSONARBOUND 35
#define WHITE 0
#define BLACK 1
#define BLUE 2
#define RED 3

#define FLAGSERVOOUT 255
#define FLAGSERVOIN 0
#define STOPPERIN 255
#define STOPPEROUT 0
#define WRISTIN 255
#define WRISTOUT 50
#define ELBOWIN 0
#define ELBOWOUT 115

//LEDBitmask addres definitions
#define B2 0x04 //IR detected output address
#define B3 0x08 //block detected output address
#define B4 0x0F //robot lifting output address
#define B5 0x20 //stopper in output address

#ifdef AUDIO_DEBUG_ENABLED
#define AUDIO_DEBUG(frequency, duration) PlayTone(frequency, duration)
#else
#define AUDIO_DEBUG(frequency, duration)
#endif

#ifdef TIMEOUTS_ENABLED
#define TIMEOUT(time) time1[T1] > time
#else
#define TIMEOUT(time) time1[T1] > 100000000
#endif
