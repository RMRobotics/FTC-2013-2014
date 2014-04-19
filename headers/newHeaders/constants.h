//Number of accelerometer readings to track
#define TRACK_PAST_N_VALUES 10

//Drive and turn speeds
const short TURNSPEED = 75;
const short DRIVESPEED = 100;
const short SLOWSPEED = 25;

//compensation for robots tendency to drift right
const short COMP = 1;

//line following constants
const float LINEFOLLOWRATIO = 0.15;
const short WHITE = 0;
const short BLACK = 1;
const short BLUE = 2;
const short RED = 3;
const short GREEN = 4;

//servo positions
const short PAWSERVODOWN = 240;
const short PAWSERVOUP = 128;
const short FLAGSERVOOUT = 255;
const short FLAGSERVOIN = 0;
const short STOPPERIN = 255;
const short STOPPEROUT = 0;
const short WRISTIN = 245;
const short WRISTOUT = 50;
const short ELBOWIN = 0;
const short ELBOWOUT = 130;

//LEDBitmask addres definitions
#define B2 0x04 //IR detected output address
#define B3 0x08 //block detected output address
#define B4 0x0F //robot lifting output address
#define B5 0x20 //stopper in output address
