#include <map>
#include <string>
#include "globalConstants.h"

class gConfig
{
public:
    static gConfig& getInstance()
    {
        static gConfig instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    bool readConfFile(const char* confFileName);
    
    double getExtNoise(const int ext);
    double getExtDcMean(const int ext);
    double getExtGain(const int ext);
    
    double getFanoFactor () {return fFanoFactor;}; ////creo que este no va!!!
    double getMaxDiffusion () {return fMaxDiffusion;};////creo que este no va!!!!
    
    double getCCDThickness() {return fCCDThick;};
    double getPixelSize() {return fPixelSize;};
    double getElectronIonizationEnergy() {return fElecInEv;};
    
    //simulation
    unsigned int getSeedRandom() {return fSeedRandom;};
    int getNumColsNewImage() {return fNColNewImage;};
    int getNumRowsNewImage() {return fNRowNewImage;};
    int getDistAmongEvent() {return fDistAmongEvent;};
    int getNumOfEvents() {return fNEvent;};
    
    //Energy model
    bool getFixedEnergyFlag() {return fFixedEner;};
    double getFixedEnergy() {return fFixedEner;};
    std::string getEnergyModel() {return fEnerModel;};
    double getMinEnergy() {return fEMin;};
    double getMaxEnergy() {return fEMax;};
    
    //Z-Model
    bool getFixedZFlag() {return fFixedZFlag;};
    double getFixedZ() {return fFixedZ;};
    std::string getZModel() {return fZModel;};
    double getMinZ() {return fZMin;};
    double getMaxZ() {return fZMax;};
    
    //X-Model
    bool getFixedXFlag() {return fFixedXFlag;};
    double getFixedX() {return fFixedX;};
    std::string getXModel() {return fXModel;};
    double getMinX() {return fXMin;};
    double getMaxX() {return fXMax;};

    //Y-Model
    bool getFixedYFlag() {return fFixedYFlag;};
    double getFixedY() {return fFixedY;};
    std::string getYModel() {return fYModel;};
    double getMinY() {return fYMin;};
    double getMaxY() {return fYMax;};
    
    //Diffusion Model
    std::string getExtDiffusionModel(const int ext);
//    double getExtDiffParam1(const int ext);
//    double getExtDiffParam2(const int ext);
    
    void printVariables();
    
private:
    gConfig();
    
    std::map<int, double> fExt2Noise;
    double fDefaultNoise;
    
    std::map<int, double> fExt2DCMean;
    double fDefaultDCMean;
    
    double gain;
    std::map<int, double> fExt2Gain;
    double fDefaultGain;
    
    float fFanoFactor;
    double fMaxDiffusion;
    
    double fCCDThick;
    double fPixelSize;
    double fElecInEv;
    
    //simulation
    unsigned int fSeedRandom;
    int fNColNewImage;
    int fNRowNewImage;
    int fDistAmongEvent; //[pixels]
    int fNEvent;//max num of events to simulate (Hacer un default)

    //point events
    bool fFixedEnerFlag;
    double fFixedEner;
    std::string fEnerModel;
    double fEMin;
    double fEMax;

    //Z-position model
    bool fFixedZFlag;
    double fFixedZ;
    std::string fZModel;
    double fZMin;
    double fZMax;
    
    //X-position model
    bool fFixedXFlag;
    double fFixedX;
    std::string fXModel;
    double fXMin;
    double fXMax;
    
    //Y-position model
    bool fFixedYFlag;
    double fFixedY;
    std::string fYModel;
    double fYMin;
    double fYMax;
    
    //diffusion model
    std::map<int, std::string> fExt2DiffModel;
    std::string fDefaultDiffModel;
    //std::map<int, double> fExt2DiffParam1;
    //double fDefaultDiffParam1;
    //std::map<int, double> fExt2DiffParam2;
    //double fDefaultDiffParam2;
    
    
    // Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
    gConfig(gConfig const&);              // Don't Implement
    void operator=(gConfig const&); // Don't implement
};
