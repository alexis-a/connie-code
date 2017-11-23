#ifndef _globalConstants_h_
#define _globalConstants_h_


extern int  gVerbosity;
extern bool gKeepParallelOvsc;
extern bool gSimulateBlanck;
extern bool gPointEvents ;
extern bool gRandomPos;
extern bool gPositionGridFlag;
extern const int gMaxModelCharacters;

extern unsigned int gSeedRandom;
extern int gNColDefault;
extern int gNRowDefault;
extern double gNoiseDefualt;
extern double gDcMeanDefault;
extern double gGainDefault;
extern double gFanoFactorDefault;
extern double gMaxDiffusionDefault;
extern int gNEvent;
extern int gDistAmongEventDefault;
extern double gCCDThickDefault;
extern double gRoSiDefault;
extern double eInEVDefault;
extern double dEdxFixedDefault;
extern double MeVInEV;
extern double cmInUm;
extern double dQdxFixed;

extern double gPixelSizeDefault;
extern double diffParam1Default;
extern double diffParam2Default;

extern const int    kMaxLine;

//colors
extern const char cyan[];
extern const char magenta[];
extern const char red[];
extern const char green[];
extern const char yellow[];
extern const char blue[];

extern const char bold[];

extern const char whiteOnRed[];
extern const char whiteOnGreen[];
extern const char whiteOnPurple[] ;
extern const char whiteOnViolet[];
extern const char whiteOnBrown[];
extern const char whiteOnGray[];

extern const char normal[];

extern const double kPrescan;
extern const float kSatValue;
extern const float kSatMargin;
extern const double pi;

extern const char *gBaseTNtupleVars[];
extern const int   gNBaseTNtupleVars;
extern const char *gExtraTNtupleVars[];
extern const int   gNExtraTNtupleVars;

#endif
