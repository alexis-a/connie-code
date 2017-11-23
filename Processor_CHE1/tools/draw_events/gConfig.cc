#include "tinyxml2.h"
#include "gConfig.h"
#include "globalConstants.h"
#include <iostream>
#include <sstream>
#include <math.h>
#include <map>


using namespace std;

gConfig::gConfig(): fDefaultNoise(-1.0), fDefaultDCMean(-1.0), fDefaultGain(-1.0), fFanoFactor(-1.0), fMaxDiffusion(-1.0), fCCDThick(-1.0), fPixelSize(-1.0), fElecInEv(-1.0), fSeedRandom(0), fNColNewImage(-1), fNRowNewImage(-1), fFixedEner(-1.0), fEnerModel(""), fEMin(-1.0), fEMax((-1.0)), fFixedZFlag(true), fFixedZ(-1.0), fZModel(""), fZMin(-1.0), fZMax(-1.0), fFixedXFlag(true), fFixedX(-1.0), fXMin(-1.0), fXMax(-1.0), fFixedYFlag(true), fFixedY(-1.0), fYMin(-1.0), fYMax(-1.0), fDefaultDiffModel("")

{}

double gConfig::getExtNoise(const int ext){
    
    std::map<int, double>::const_iterator it = fExt2Noise.find( ext );
    if ( it == fExt2Noise.end() ) {
        return fDefaultNoise;
    }
    return it->second;
}

double gConfig::getExtDcMean(const int ext){
    
    std::map<int, double>::const_iterator it = fExt2DCMean.find( ext );
    if ( it == fExt2DCMean.end() ) {
        return fDefaultDCMean;
    }
    return it->second;
}

double gConfig::getExtGain(const int ext){
    
    std::map<int, double>::const_iterator it = fExt2Gain.find( ext );
    if ( it == fExt2Gain.end() ) {
        return fDefaultGain;
    }
    return it->second;
}

std::string gConfig::getExtDiffusionModel(const int ext) {
    std::map<int, std::string>::const_iterator it = fExt2DiffModel.find( ext );
    if ( it == fExt2DiffModel.end() ) {
        return fDefaultDiffModel;
    }
    return it->second;
}

void gConfig::printVariables(){
    cout << endl;
    cout << "===== Image Noise =====\n";
    cout << "nosie (only used on new images):\n";
    cout << "\tdefault: " << fDefaultNoise << endl;
    typedef std::map<int, double>::iterator it_type;
    for(it_type it = fExt2Noise.begin(); it != fExt2Noise.end(); it++) {
        cout << "\text" << it->first << ": " << it->second << endl;
    }
    cout << "===== Image Dark Current =====\n";
    cout << "dark current (only used on new images):\n";
    cout << "\tdefault: " << fDefaultDCMean << endl;
    typedef std::map<int, double>::iterator it_type;
    for(it_type it = fExt2DCMean.begin(); it != fExt2DCMean.end(); it++) {
        cout << "\text" << it->first << ": " << it->second << endl;
    }
    
    cout << "===== Image Gain =====\n";
    cout << "gain:\n";
    cout << "\tdefault: " << fDefaultGain << endl;
    typedef std::map<int, double>::iterator it_type;
    for(it_type it = fExt2Gain.begin(); it != fExt2Gain.end(); it++) {
        cout << "\text" << it->first << ": " << it->second << endl;
    }
    
    
    cout << "=====Simulation Parameters=====" << endl;
    cout << "=====" << "" << "=====" << endl;
    cout << "\tNum. of Events per hdu: " << fNEvent << endl;
    cout << "\tCCD Thickness: " << fCCDThick << endl;
    cout << "\tPixel Size: " << fPixelSize << endl;
    cout << "\te-h pair ionization energy " << fElecInEv << endl;
    cout << "\tRandom Seed:      " << fSeedRandom << endl;
    
    cout << "Simulation parameters for new images:\n";
    cout << "\tNum. Cols New Image:       " << fNColNewImage << endl;
    cout << "\tNum. Rows New Image: " << fNRowNewImage << endl;
    cout << "\tDist. Among Events: " << fDistAmongEvent << endl;
    
    cout << "=====Point Events Model Parameters=====" << endl;
    cout << "Energy Model:\n";
    cout << "\tEnergy PDF:       " << fEnerModel << endl;
    cout << "\tMin. Energy: " << fEMin << endl;
    cout << "\tMax. Energy: " << fEMax << endl;
    //cout << "\tFixed Energy: " << fFixedEner << endl;
    
    cout << "Z Model:\n";
    cout << "\tZ PDF:       " << fZModel << endl;
    cout << "\tMin. Z: " << fZMin << endl;
    cout << "\tMax. Z: " << fZMax << endl;
    //cout << "\tFixed Z: " << fFixedZ << endl;
    
    cout << "X Model:\n";
    cout << "\tX PDF:       " << fXModel << endl;
    cout << "\tMin. X: " << fXMin << endl;
    cout << "\tMax. X: " << fXMax << endl;
    //cout << "\tFixed X: " << fFixedX << endl;
    
    cout << "Y Model:\n";
    cout << "\tY PDF:       " << fYModel << endl;
    cout << "\tMin. Y: " << fYMin << endl;
    cout << "\tMax. Y: " << fYMax << endl;
    //cout << "\tFixed Y: " << fFixedY << endl;
    
    cout << "===== Diffusion models =====\n";
    cout << "diffusioin model:\n";
    cout << "\tdefault: " << fDefaultDiffModel << endl;
    typedef std::map<int, std::string>::iterator it_type2;
    for(it_type2 it = fExt2DiffModel.begin(); it != fExt2DiffModel.end(); it++) {
        cout << "\text" << it->first << ": " << it->second << endl;
    }
    
}

bool gConfig::readConfFile(const char* confFileName = "simulationConfig.xml"){
    
    tinyxml2::XMLDocument doc;
    if(doc.LoadFile( confFileName ) != 0){
        cerr << red << "\nCan't read config file: " << confFileName << endl;
        cerr << "Will not continue.\n\n" << normal;
        return false;
    }
    //   doc.Print();
    
    /* Noise*/
    fDefaultNoise = gNoiseDefualt;
    if( doc.FirstChildElement("noise") == 0 ){
        //cerr << "Missing \'noise\' in config file.\n\n";
        //return false;
        cout << "Missing \'noise\' in config file. Default values will be used.\n\n";
        
    } else {
        if(doc.FirstChildElement("noise")->QueryDoubleAttribute("default", &fDefaultNoise) !=0){
            cerr << "Missing \'noise:default\' in config file.\n\n";
            return false;
        }
        
        for(int i=0;i<100;++i){
            ostringstream ossExtSig;
            ossExtSig << "ext" << i;
            double extNoise = -1;
            if(doc.FirstChildElement("noise")->QueryDoubleAttribute(ossExtSig.str().c_str(), &extNoise) == 0){
                fExt2Noise[i] = extNoise;
            }
        }
    }
    
    
    /* Dark Current*///////////////////////////////
    fDefaultDCMean = gDcMeanDefault;
    if( doc.FirstChildElement("darkCurrent") == 0 ){
        //cerr << "Missing \'darkCurrent\' in config file.\n\n";
        //return false;
        cout << "Missing \'darkCurrent\' in config file. Default values will be used.\n\n";
    } else {
        
        if(doc.FirstChildElement("darkCurrent")->QueryDoubleAttribute("default", &fDefaultDCMean) !=0){
            cerr << "Missing \'darkCurrent:default\' in config file.\n\n";
            return false;
        }
        
        for(int i=0;i<100;++i){
            ostringstream ossExtSig;
            ossExtSig << "ext" << i;
            double extDC = -1;
            if(doc.FirstChildElement("darkCurrent")->QueryDoubleAttribute(ossExtSig.str().c_str(), &extDC) == 0){
                fExt2DCMean[i] = extDC;
            }
        }
    }
    
    /* Gain*/////////////////////////////////////
    fDefaultGain = gGainDefault;
    if( doc.FirstChildElement("gain") == 0 ){
        //cerr << "Missing \'gain\' in config file.\n\n";
        //return false;
        cout << "Missing \'gain\' in config file. Default values will be used.\n\n";
    } else {
        
        if(doc.FirstChildElement("gain")->QueryDoubleAttribute("default", &fDefaultGain) !=0){
            cerr << "Missing \'gain:default\' in config file.\n\n";
            return false;
        }
        
        for(int i=0;i<100;++i){
            ostringstream ossExtSig;
            ossExtSig << "ext" << i;
            double extGain = -1;
            if(doc.FirstChildElement("gain")->QueryDoubleAttribute(ossExtSig.str().c_str(), &extGain) == 0){
                fExt2Gain[i] = extGain;
            }
        }
    }
    
    
    /*CCD Configuration*////////////////////////
    if( doc.FirstChildElement("ccdConfig") == 0 ){
        //cerr << "Missing \'ccdConfig\' in config file.\n\n";
        //return false;
        fFanoFactor = gFanoFactorDefault;
        fMaxDiffusion = gMaxDiffusionDefault;
        fCCDThick = gCCDThickDefault;
        fPixelSize = gPixelSizeDefault;
        fElecInEv = eInEVDefault;
        cout << "Missing \'ccdConfig\' in config file. Default values will be used.\n\n";
        
    } else {
        
        if(doc.FirstChildElement("ccdConfig")->QueryDoubleAttribute("thickness", &fCCDThick) !=0){
            fCCDThick = gCCDThickDefault;
            cout << "CCD thickness, default value will be used: " << gCCDThickDefault << " [um]" << endl;
        }
        if(doc.FirstChildElement("ccdConfig")->QueryDoubleAttribute("pixelSize", &fPixelSize) !=0){
            fPixelSize = gPixelSizeDefault;
            cout << "Pixel size, default value will be used: " << gPixelSizeDefault << " [um]" << endl;
        }
        if(doc.FirstChildElement("ccdConfig")->QueryDoubleAttribute("elecIonizEner", &fElecInEv) !=0){
            fElecInEv = eInEVDefault;
            cout << "Electron ionizing energy, default value will be used: " << eInEVDefault << " [eV]" << endl;
        }

    }
    
    /*simulation config*/////////////////////////////
    if( doc.FirstChildElement("simulationConfig") == 0 ){
        cerr << "Missing \'simulationConfig\' in config file.\n\n";
        return false;
    }
    if( doc.FirstChildElement("simulationConfig")->QueryUnsignedAttribute("randomSeed", &fSeedRandom) != 0 ){
        fSeedRandom = gSeedRandom;
        cout << "TRandom seed (if the case), default value will be used: " << gSeedRandom << endl;
    }
    if( doc.FirstChildElement("simulationConfig")->QueryIntAttribute("colsNewImage", &fNColNewImage) != 0 ){
        fNColNewImage = gNColDefault;
        cout << "Num. of columns in new images (if the case), default value will be used: " << gNColDefault << endl;
    }
    if( doc.FirstChildElement("simulationConfig")->QueryIntAttribute("rowsNewImage", &fNRowNewImage) != 0 ){
        fNRowNewImage = gNRowDefault;
        cout << "Num. of rows in new images (if the case), default value will be used: " << gNRowDefault << endl;
    }
    if( doc.FirstChildElement("simulationConfig")->QueryIntAttribute("distAmongEvents", &fDistAmongEvent) != 0 ){
        fDistAmongEvent = gDistAmongEventDefault;
        cout << "Separation between pixels (if the case), default value will be used: " << gDistAmongEventDefault << endl;
    }
    if( doc.FirstChildElement("simulationConfig")->QueryIntAttribute("Nevents", &fNEvent) != 0 ){
        fNEvent = gNEvent;
    }
    
    /*Point Events Model*////////////////////////////
    if( doc.FirstChildElement("pointEvent") == 0 ){
        cerr << "Missing \'simulationConfig\' in config file.\n\n";
        return false;
    }
    
    const char *fEnerModelTemp = doc.FirstChildElement("pointEvent")->Attribute("EPDF");
    if(fEnerModelTemp == NULL){
        cerr << "Missing \'EPDF\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("EMin", &fEMin) != 0){
        cerr << "Missing \'EMin\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("EMax", &fEMax) != 0){
        cerr << "Missing \'EMax\' in config file.\n\n";
    }
    fEnerModel = fEnerModelTemp;

    const char *fZModelTemp = doc.FirstChildElement("pointEvent")->Attribute("zPDF");
    if( fZModelTemp == NULL){
        cerr << "Missing \'zPDF\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("zMin", &fZMin) != 0){
        cerr << "Missing \'zMin\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("zMax", &fZMax) != 0){
        cerr << "Missing \'zMax\' in config file.\n\n";
    }
    fZModel = fZModelTemp;

    const char *fXModelTemp = doc.FirstChildElement("pointEvent")->Attribute("xPDF");
    if( fXModelTemp == NULL){
        cerr << "Missing \'xModel\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("xMin", &fXMin) != 0){
        cerr << "Missing \'xMin\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("xMax", &fXMax) != 0){
        cerr << "Missing \'xMax\' in config file.\n\n";
    }
    fXModel = fXModelTemp;

    const char *fYModelTemp = doc.FirstChildElement("pointEvent")->Attribute("yPDF");
    if(fYModelTemp == NULL){
        cerr << "Missing \'yModel\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("yMin", &fYMin) != 0){
        cerr << "Missing \'yMin\' in config file.\n\n";
    }
    if( doc.FirstChildElement("pointEvent")->QueryDoubleAttribute("yMax", &fYMax) != 0){
        cerr << "Missing \'yMax\' in config file.\n\n";
    }
    fYModel = fYModelTemp;
    
    
    /* Diffusion Model*///////////////////////////////
    if( doc.FirstChildElement("diffusionModel") == 0 ){
        cerr << "Missing \'diffusionModel\' in config file.\n\n";
        return false;
    }
    
    const char * fDiffModelTemp = doc.FirstChildElement("diffusionModel")->Attribute("default");
    if( fDiffModelTemp==NULL){
        cerr << "Missing \'diffusionModel:default\' in config file.\n\n";
        return false;
    } else {
        fDefaultDiffModel = fDiffModelTemp;
    }
    
    for(int i=0;i<100;++i){
        ostringstream ossExtSig;
        ossExtSig << "ext" << i;
        const char * extDiffModelTemp = doc.FirstChildElement("diffusionModel")->Attribute(ossExtSig.str().c_str());
        if(extDiffModelTemp != NULL){
            fExt2DiffModel[i] = extDiffModelTemp;
        }
    }
    
    
    
    return true;
}
