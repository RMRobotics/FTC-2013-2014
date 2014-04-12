//enabling or disabling certain macros
#define AUDIO_DEBUG_ENABLED
//#define TIMEOUTS_ENABLED

//state machine constants
#define FINDLINE_DRIVE 0   // Drive until you hit the corner of the low goal
#define FINDLINE_TURN 1    // turn until you're on the outside of the low goal tape line or until you make a 45 degree turn
#define FINDLINE_DRIVE2 2  // if you missed the tape line and made the 45 degree turn,
                           // drive forward but lean slightly to the right to try to catch the line at a shallow enough angle to line follow
#define LINEFOLLOW 3       // line follow
#define SCOREBLOCK 4       // score the block
#define GOTOEND 5          // only in H: move forward some more
#define PARK_TURN1 6       // turn in toward the lines at the center of the field
#define PARK_DRIVE1 7      // drive until you hit the lines at the center of the field
#define PARK_TURN2 8       // turn to be parallel with the ramp wall again
#define PARK_DRIVE2 9      // drive until accelerometer is triggered
#define END 10             // end of program
#define HARVEST 11         // harvest cubes during autonomous, not used
#define CHECKEND 12        // check to see if accelerometer really is triggered, depreciated, not used
#define INITIALSTATE 999   // not an actual physical state, used to initialize prevState variable

//Drive and turn speeds
#ifdef __BACKWARDSTART__
#define TURNSPEED -75
#define DRIVESPEED -100
#define ANGLESPECIAL 15
#else
#define TURNSPEED 75
#define DRIVESPEED 100
#define ANGLESPECIAL 9
#endif

//compensation for robots tendency to drift right
#define COMP 1

//line following constants
#define LINEFOLLOWRATIO 0.20
#define WHITE 0
#define BLACK 1
#define BLUE 2
#define RED 3
#define GREEN 4

//servo positions
#define PAWSERVODOWN 240
#define PAWSERVOUP 128
#define FLAGSERVOOUT 255
#define FLAGSERVOIN 0
#define STOPPERIN 255
#define STOPPEROUT 0
#define WRISTIN 245
#define WRISTOUT 50
#define ELBOWIN 0
#define ELBOWOUT 140

//LEDBitmask addres definitions
#define B2 0x04 //IR detected output address
#define B3 0x08 //block detected output address
#define B4 0x0F //robot lifting output address
#define B5 0x20 //stopper in output address

//*******************************macros*********************************
// enables beap during state transitions
#ifdef AUDIO_DEBUG_ENABLED
#define AUDIO_DEBUG(frequency, duration) PlayTone(frequency, duration)
#else
#define AUDIO_DEBUG(frequency, duration)
#endif

// enables emergency timeouts if a state takes longer than expected to finish
#ifdef TIMEOUTS_ENABLED
#define TIMEOUT(time) time1[T1] > time
#else
#define TIMEOUT(time) time1[T1] > 100000000
#endif

//Change turn direction in state PARK_TURN2 depending on whether going front or back
#ifdef __BACKWARDSTART__
#define INITIALDRIVE()
#define DRIVESPECIAL(left, right) drive(-left, -right)
#define WAITSPECIAL() wait1Msec(100)
#else
#define INITIALDRIVE() drive(25, 25); wait1Msec(400)
#define DRIVESPECIAL(left, right) drive(left, right)
#define WAITSPECIAL() wait1Msec(0)
#endif
//**********************************************************************
