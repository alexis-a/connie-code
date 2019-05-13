#include <math.h>
#include <map>
#include "globalConstants.h"


int  gVerbosity = true;
bool gKeepParallelOvsc = false;
bool gSimulateBlanck = true;
bool gPointEvents = true;
bool gRandomPos = false;
bool gPositionGridFlag = false;
const int gMaxModelCharacters = 1000;

unsigned int gSeedRandom = 0;
int gNColDefault = 4096;
int gNRowDefault = 4096;
double gNoiseDefualt = 0;//[e-]
double gDcMeanDefault = 0; //[e-]
double gGainDefault = 1;
double gFanoFactorDefault = 0.115;// adimensional
double gMaxDiffusionDefault = 1.2;//[pixels]
int gNEvent = 10;//max num of events to simulate
int gDistAmongEventDefault = 20; //[pixels]
double gCCDThickDefault = 675;//[um]
double gRoSiDefault = 2.329;//[g/cm^3]
double eInEVDefault = 3.745;//[eV/e-]
double dEdxFixedDefault = 1.6; //[MeV*cm^2/g]
double MeVInEV = 1e6; //[eV/MeV]
double cmInUm = 1e4; //[um/cm]
double dQdxFixedDefault = dEdxFixedDefault*gRoSiDefault*MeVInEV/(eInEVDefault*cmInUm); //[e-/um]
double gPixelSizeDefault = 15;//[um]
double diffParam1Default = 215;//[um^2]
double diffParam2Default = 0.0013;//[1/um]

const int    kMaxLine = 10000;

//colors
const char cyan[] = { 0x1b, '[', '1', ';', '3', '6', 'm', 0 };
const char magenta[] = { 0x1b, '[', '1', ';', '3', '5', 'm', 0 };
const char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
const char green[] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
const char yellow[] = { 0x1b, '[', '1', ';', '3', '3', 'm', 0 };
const char blue[] = "\x1b[1;34m";

const char bold[] = "\x1b[1;39m";

const char whiteOnRed[]    = "\x1b[1;41m";
const char whiteOnGreen[]  = "\x1b[1;42m";
const char whiteOnPurple[] = "\x1b[1;45m";
const char whiteOnViolet[] = "\x1b[1;44m";
const char whiteOnBrown[]  = "\x1b[1;43m";
const char whiteOnGray[]   = "\x1b[1;47m";

const char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

const double kPrescan = 6;
const float kSatValue = 1e10;
const float kSatMargin = 0.9;
const double pi =3.1416;



const char *gBaseTNtupleVars[]  = {"runID", "ohdu", "nSat", "flag", "xMin", "xMax", "yMin", "yMax"};
const int   gNBaseTNtupleVars   = 8;
const char *gExtraTNtupleVars[] = {"E", "n", "xBary", "yBary", "xVar", "yVar"};
const int   gNExtraTNtupleVars  = 6;
