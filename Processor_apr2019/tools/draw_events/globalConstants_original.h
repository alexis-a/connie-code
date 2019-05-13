#ifndef _globalConstants_h_
#define _globalConstants_h_

#include <math.h> 
#include <map>



int  gVerbosity = true;
bool gKeepParallelOvsc = false;
bool gSimulateBlanck = true;
bool gPointEvents = true;
bool gRandomPos = false;

///HARD CODED PARAMETERS
//UInt_t seedRandom = 0.0;
//Int_t nColNewImage = 4096;
//Int_t nRowNewImage = 4096;
//Double_t noiseSigma = 2;//[e-]
//Double_t dcMean = 10; //[e-]
//Double_t gain = 1;//[adu/e-]
//Double_t fanoFactor = 0.115;// adimensional
//Double_t maxDiffusion = 1.2; //[pixels]
//Int_t Nevent = 10;//max num of events to simulate
//Int_t Qevent = 10000;//[e-]
//Int_t minDistEvent = 200; //[pixels]
//Int_t ccdThick = 675;//[um]
//Double_t roSi = 2.329;//[g/cm^3]
//Double_t eInEV = 3.745;//[eV/e-]
//Double_t dEdxFixed = 1.6; //[MeV*cm^2/g]
//Double_t MeVInEV = 1e6; //[eV/MeV]
//Double_t cmInUm = 1e4; //[um/cm]
//Double_t dQdxFixed = dEdxFixed*roSi*MeVInEV/(eInEV*cmInUm); //[e-/um]
//Double_t pixelSize = 15;//[um]
//Double_t aModel = 215;//[um^2]
//Double_t bModel = 0.0013;//[1/um]

unsigned int seedRandom = 0.0;
int nColNewImage = 4096;
int nRowNewImage = 4096;
double noiseSigma = 2;//[e-]
double dcMean = 10; //[e-]
double gain = 1;//[adu/e-]
double fanoFactor = 0.115;// adimensional
double maxDiffusion = 1.2; //[pixels]
int Nevent = 10;//max num of events to simulate
int Qevent = 10000;//[e-]
int minDistEvent = 200; //[pixels]
int ccdThick = 675;//[um]
double roSi = 2.329;//[g/cm^3]
double eInEV = 3.745;//[eV/e-]
double dEdxFixed = 1.6; //[MeV*cm^2/g]
double MeVInEV = 1e6; //[eV/MeV]
double cmInUm = 1e4; //[um/cm]
double dQdxFixed = dEdxFixed*roSi*MeVInEV/(eInEV*cmInUm); //[e-/um]
double pixelSize = 15;//[um]
double aModel = 215;//[um^2]
double bModel = 0.0013;//[1/um]


inline double sq(double x){return x*x;};

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

#endif
