#include <string.h>
#include <stdio.h>
#include "fitsio.h"

#include <iostream>
#include <sstream>

#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include <fstream>
#include <unistd.h>
#include <getopt.h>    /* for getopt_long; standard getopt is in unistd.h */
#include <vector>
#include <algorithm>
#include <ctime>
#include <climits>
#include <cmath>
#include <iomanip>

#include "TFile.h"
#include "TNtuple.h"
#include "TObject.h"
#include "TTree.h"
#include "TF1.h"
#include "TBranch.h"

#include "TRandom3.h"


#include "tinyxml2.h"
#include "gConfig.h"
#include "globalConstants.h"

using namespace std;


struct distModel_t {
    string pdf;
    Double_t minValue;
    Double_t maxValue;
    Double_t fixedValue;
    bool fixedValueFlag;
    
    distModel_t():pdf(""),minValue(-1), maxValue(-1), fixedValue(-1), fixedValueFlag(false)
    {
        
    };
    ~distModel_t()
    {
        
    };
};

struct simulationConfigTree_t {
    
    Int_t nCols;
    Int_t nRows;
    UInt_t seedRandom;
    Double_t ccdThick;
    Double_t pixelSize;
    Double_t electronInEV;
    Int_t nEvents;
    Int_t minDistEvent;
    
    simulationConfigTree_t(): nCols(-1), nRows(-1), seedRandom(-1), ccdThick(-1), pixelSize(-1), electronInEV(-1), nEvents(-1), minDistEvent(-1)
    {
    };
    ~simulationConfigTree_t()
    {
    };
};

struct hitTreeEntry_t {
    Double_t oEnergy;
    UInt_t eventID;
    Int_t oCharge;
    Int_t hdu;
    Int_t flag;
    Double_t oSigma;
    Double_t oMuX;
    Double_t oMuY;
    Double_t oMuZ;
    Double_t noise;
    Double_t dcMean;
    Double_t gain;
    string diffModel;
    
    hitTreeEntry_t(): oEnergy(-1), eventID(-1), oCharge(-1), hdu(-1), flag(-1), oSigma(-1), oMuX(-1), oMuY(-1), noise(-1), dcMean(-1), gain(-1), diffModel("")
    {
    };
    ~hitTreeEntry_t()
    {
    };
};

int deleteFile(const char *fileName){
    cout << yellow;
    cout << "Will overwrite: " << fileName << endl << endl;
    cout << normal;
    return unlink(fileName);
}

bool fileExist(const char *fileName){
    ifstream in(fileName,ios::in);
    
    if(in.fail()){
        //cout <<"\nError reading file: " << fileName <<"\nThe file doesn't exist!\n\n";
        in.close();
        return false;
    }
    
    in.close();
    return true;
}

/*========================================================
 ASCII progress bar
 ==========================================================*/
void showProgress(unsigned int currEvent, unsigned int nEvent) {
    
    const int nProgWidth=50;
    
    if ( currEvent != 0 ) {
        for ( int i=0;i<nProgWidth+8;i++)
            cout << "\b";
    }
    
    double percent = (double) currEvent/ (double) nEvent;
    int nBars = (int) ( percent*nProgWidth );
    
    cout << " |";
    for ( int i=0;i<nBars-1;i++)
        cout << "=";
    if ( nBars>0 )
        cout << ">";
    for ( int i=nBars;i<nProgWidth;i++)
        cout << " ";
    cout << "| " << setw(3) << (int) (percent*100.) << "%";
    cout << flush;
    
}

void printCopyHelp(const char *exeName, bool printFullHelp=false){
    
    if(printFullHelp){
        cout << bold;
        cout << endl;
        cout << "This program simulate point like events following configuration\n";
        cout << "parameters from an xml configuration file. It can draw events in\n";
        cout << "existing fits files or simulate new images.\n";
        cout << "It handles all the available HDUs. The HDUs in the output fit file\n";
        cout << "will be double (64bits) for 64bits images and float (32bis) in all\n";
        cout << "the other cases.\n";
        cout << normal;
    }
    cout << "==========================================================================\n";
    cout << yellow;
    cout << "\nUsage:\n";
    cout << "  "   << exeName << " -o <output filename> -c <configuration file> \n\n";
    cout << "\nOptions:\n";
    cout << "  -q for quiet (no screen output)\n";
    cout << "  -i <input file> to draw events in an existing file\n";
    cout << "  -f <separation> draw events with a position offset as a grid in the image. Separated <separation> among them.\n";
    cout << "  -s <HDU number> for processing a single HDU \n\n";
    cout << normal;
    cout << blue;
    cout << "For any problems or bugs contact Guillermo Fernandez <fmoroni.guillermo@gmail.com>\n\n";
    cout << normal;
    cout << "==========================================================================\n\n";
}

string bitpix2TypeName(int bitpix){
    
    string typeName;
    switch(bitpix) {
        case BYTE_IMG:
            typeName = "BYTE(8 bits)";
            break;
        case SHORT_IMG:
            typeName = "SHORT(16 bits)";
            break;
        case LONG_IMG:
            typeName = "INT(32 bits)";
            break;
        case FLOAT_IMG:
            typeName = "FLOAT(32 bits)";
            break;
        case DOUBLE_IMG:
            typeName = "DOUBLE(64 bits)";
            break;
        default:
            typeName = "UNKNOWN";
    }
    return typeName;
}

int createStructure(const string inFile, const char *outF, vector<int> &singleHdu, const vector<long> naxesOut, const simulationConfigTree_t &sim){
    
    fitsfile  *outfptr; /* FITS file pointers defined in fitsio.h */
    
    int status = 0;
//    int single = 0;
    
    int naxis = 2;
    int nhdu = 0;
    long naxes[2] = {sim.nCols,sim.nRows};
    //    long totpix = 0;
    
    ostringstream fileStructSummary;
    
    //create file
    fits_create_file(&outfptr, outF, &status);/* Create the output file */
    if (status != 0) return(status);
    
    
    //create hdu
    unsigned int nUseHdu = 0;
    if (singleHdu.size()==0) {
        nUseHdu = 1;
        singleHdu.push_back(1);
    } else {
        nUseHdu = singleHdu.size();
    }
    
    for (uint i = 0; i<nUseHdu; i++){
        fits_create_img(outfptr, -32, naxis, naxes, &status);
        if (status != 0) return(status);
        
    }

    
    
    fileStructSummary << "The output file will contain " << nhdu << " hdus availables in the input files."<< endl;
    
    fits_close_file(outfptr,  &status);
    
    if(gVerbosity){
        cout << bold << "Files structure summary:\n" << normal;
        cout << fileStructSummary.str();
        cout << green << "Structure generated.\n\n" << normal;
    }
    return status;
    
    return 0;
}

int copyStructure(const string inFile, const char *outF, vector<int> &singleHdu, const simulationConfigTree_t &sim){
    
    fitsfile  *outfptr; /* FITS file pointers defined in fitsio.h */
    fitsfile *infptr;   /* FITS file pointers defined in fitsio.h */
    
    int status = 0;
    int single = 0;
    
    int hdutype, bitpix, naxis = 0, nkeys;
    int nhdu = 0;
    long naxes[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    long totpix = 0;
    //char card[81];
    
    ostringstream fileStructSummary;
    
    const char* inF = inFile.c_str();
    fits_open_file(&infptr, inF, READONLY, &status); /* Open the input file */
    if (status != 0) return(status);
    
    fits_get_num_hdus(infptr, &nhdu, &status);
    
    /* check the extensions to process*/
    for(unsigned int i=0;i<singleHdu.size();++i){
        if(singleHdu[i] > nhdu){
            fits_close_file(infptr,  &status);
            cerr << red << "\nError: the file does not have the required HDU!\n\n" << normal;
            return -1000;
        }
    }
    
    if(singleHdu.size() == 0){
        for(int i=0;i<nhdu;++i){
            singleHdu.push_back(i+1);
        }
    }
    
    const unsigned int nUseHdu = (singleHdu[0]==-1)? 1 : singleHdu.size();
    
    if(single)
        fileStructSummary << "The output file will contain " << nUseHdu << " of " << nhdu << " hdus availables in the input files."<< endl;
    
    fits_create_file(&outfptr, outF, &status);/* Create the output file */
    if (status != 0) return(status);
    
    
    fileStructSummary << bold << "HDU   hdutype  #Axis  #Cols  #Rows   IN_datatype      OUT_datatype\n" << normal;
    // HDU  hdutype #Axis #Cols #Rows datatype
    for (unsigned int eI=0; eI<nUseHdu; ++eI)  /* Main loop through each extension */
    {
        const unsigned int n = (singleHdu[0]==-1)? 2 : singleHdu[eI];
        
        /* get image dimensions and total number of pixels in image */
        fits_movabs_hdu(infptr, n, &hdutype, &status);
        for (int i = 0; i < 9; ++i) naxes[i] = 1;
        fits_get_img_param(infptr, 9, &bitpix, &naxis, naxes, &status);
        totpix = naxes[0] * naxes[1] * naxes[2] * naxes[3] * naxes[4] * naxes[5] * naxes[6] * naxes[7] * naxes[8];
        fileStructSummary  << setw (3) << n << "  "  << setw (8) << hdutype << "  " << setw (5) << naxis << "  " << setw (5) << naxes[0] << "  " << setw (5) << naxes[1] << "  " << setw (15) << bitpix2TypeName(bitpix);
        
        //continue;
        if (hdutype != IMAGE_HDU || naxis == 0 || totpix == 0){
            /* just copy tables and null images */
            fits_copy_hdu(infptr, outfptr, 0, &status);
            if (status != 0) return(status);
            fileStructSummary << "  " << setw (8) << magenta << "Not an image HDU" << normal;
        } else{
            
            if(abs(bitpix) == 64) fits_create_img(outfptr, -64, naxis, naxes, &status);
            else{
                fits_create_img(outfptr, -32, naxis, naxes, &status);
            }
            if (status != 0) return(status);
            
            /* copy the relevant keywords (not the structural keywords) */
            fits_get_hdrspace(infptr, &nkeys, NULL, &status);
            for (int i = 1; i <= nkeys; ++i) {
                char card[FLEN_CARD];
                fits_read_record(infptr, i, card, &status);
                if(strncmp (card, "TRIMSEC", 7) == 0) continue;
                if(strncmp (card, "DATASEC", 7) == 0) continue;
                if(strncmp (card, "BZERO", 5) == 0) continue;
                if (fits_get_keyclass(card) > TYP_CMPRS_KEY) fits_write_record(outfptr, card, &status);
            }
            fileStructSummary << "  " << setw (15) << bitpix2TypeName( (abs(bitpix) == 64)? -64:-32 );
        }
        fileStructSummary << endl;
        
    }
    fits_close_file(infptr, &status);
    fits_close_file(outfptr,  &status);
    
    if(gVerbosity){
        cout << bold << "Files structure summary:\n" << normal;
        cout << fileStructSummary.str();
        cout << green << "Structure copied.\n\n" << normal;
    }
    return status;
}


int simulatehit(hitTreeEntry_t &evt, double* outArray, const int totpix, const int xMin, const int xMax, const int yMin, const int yMax, const int nCol, const simulationConfigTree_t &sim) {
    TRandom3 r(sim.seedRandom);
    
    //Coordinates of the electrons
    double* xElec = new double[evt.oCharge];
    double* yElec = new double[evt.oCharge];
    for(int q=0; q<evt.oCharge; q++) {
        xElec[q]=r.Gaus(evt.oMuX,evt.oSigma);
        yElec[q]=r.Gaus(evt.oMuY,evt.oSigma);
    }
    
    //Fill signal array
    int xindex, yindex, arrayindex;
    for(int q=0; q<evt.oCharge; q++) {
        xindex=(int)round(xElec[q]);
        yindex=(int)round(yElec[q]);
        if(xindex>xMax || xindex < xMin ||  yindex>yMax || yindex<yMin) {
            evt.flag = 1;
            //cout << "Electron out of matrix." << endl;
            //return 1;
            continue;
        }
        arrayindex=xindex-1+((yindex-1)*nCol);
        outArray[arrayindex]=outArray[arrayindex] + 1.0/evt.gain;
        
    }
    
    return 0;
}


int computeImagePointEvents(const string inFile, const char *outF, vector<int> &singleHdu, simulationConfigTree_t &sim){
    int status = 0;
    int nhdu = 0;
    
    //get configuration instance
    gConfig &gc = gConfig::getInstance();
    
    /*Get Distribution Models*/
    distModel_t energy;//energy model
    energy.pdf=             gc.getEnergyModel();
    energy.minValue =       gc.getMinEnergy();
    energy.maxValue =       gc.getMaxEnergy();
    energy.fixedValue =     gc.getFixedEnergy();
    energy.fixedValueFlag = gc.getFixedEnergyFlag();
    
    distModel_t z;//Z model
    z.pdf =           gc.getZModel();
    z.minValue =        gc.getMinZ();
    z.maxValue =        gc.getMaxZ();
    z.fixedValue =      gc.getFixedZ();
    z.fixedValueFlag =  gc.getFixedZFlag();
    
    distModel_t x;//X model
    x.pdf = gc.getXModel();
    x.minValue =        gc.getMinX();
    x.maxValue =        gc.getMaxX();
    x.fixedValue =      gc.getFixedX();
    x.fixedValueFlag =  gc.getFixedXFlag();
    
    distModel_t y;//Y model
    y.pdf = gc.getYModel();
    y.minValue =        gc.getMinY();
    y.maxValue =        gc.getMaxY();
    y.fixedValue =      gc.getFixedY();
    y.fixedValueFlag =  gc.getFixedYFlag();
    
    /* Open the output file */
    fitsfile  *outfptr; /* FITS file pointers defined in fitsio.h */
    fits_open_file(&outfptr, outF, READWRITE, &status);
    if (status != 0) return(status);
    
    fits_get_num_hdus(outfptr, &nhdu, &status);
    
    fitsfile  *infptr; /* FITS file pointers defined in fitsio.h */
    const char* inF = inFile.c_str();
    if (!gSimulateBlanck) {
        fits_open_file(&infptr, inF, READONLY, &status); /* Open the input file */
        if (status != 0) return(status);
    }
    
    const unsigned int nUseHdu = (singleHdu[0]==-1)? 1 : singleHdu.size();
    
    ostringstream fileSummary;
    
    /*event vars*/
    hitTreeEntry_t evt;
    
    /*create output files and tree*/
    string rootOutFile(outF);
    rootOutFile.append(".root");
    TFile *outTFile = new TFile(rootOutFile.c_str(),"RECREATE");
    outTFile->cd();
    TTree *hitSumm = new TTree("hitSumm","hitSumm");
    TTree *config = new TTree("config","config");

    /*event branches*/
    hitSumm->Branch("eventID",&(evt.eventID),"eventID/i");
    hitSumm->Branch("hdu",&(evt.hdu),"hdu/I");
    hitSumm->Branch("oEnergy",&(evt.oEnergy),"oEnergy/D");
    hitSumm->Branch("oCharge",&(evt.oCharge),"oCharge/I");
    hitSumm->Branch("oMuX",&(evt.oMuX),"oMuX/D");
    hitSumm->Branch("oMuY",&(evt.oMuY),"oMuY/D");
    hitSumm->Branch("oSigma",&(evt.oSigma),"oSigma/D");
    hitSumm->Branch("flag",&(evt.flag),"flag/I");
    hitSumm->Branch("dcMean",&(evt.dcMean),"dcMean/D");
    hitSumm->Branch("noise",&(evt.noise),"noise/D");
    hitSumm->Branch("gain",&(evt.gain),"gain/D");
    hitSumm->Branch("diffModel",&(evt.diffModel));
    
    
    /*config branches*/
    config->Branch("nEvents",&(sim.nEvents),"nEvents/I");
    config->Branch("nCols",&(sim.nCols),"nCols/I");
    config->Branch("nRows",&(sim.nRows),"nRows/I");
    config->Branch("seedRandom",&(sim.seedRandom),"seedRandom/I");
    config->Branch("ccdThick",&(sim.ccdThick),"ccdThick/D");
    config->Branch("pixelSize",&(sim.pixelSize),"pixelSize/D");
    config->Branch("electronInEV",&(sim.electronInEV),"electronInEV/D");
    config->Branch("EPDF",&(energy.pdf));
    config->Branch("EMin",&(energy.minValue),"EMin/D");
    config->Branch("EMax",&(energy.maxValue),"EMax/D");
    config->Branch("zPDF",&(z.pdf));
    config->Branch("zMin",&(z.minValue),"zMin/D");
    config->Branch("zMax",&(z.maxValue),"zMax/D");
    config->Branch("xPDF",&(x.pdf));
    config->Branch("xMin",&(x.minValue),"xMin/D");
    config->Branch("xMax",&(x.maxValue),"xMax/D");
    config->Branch("yPDF",&(y.pdf));
    config->Branch("yMin",&(y.minValue),"yMin/D");
    config->Branch("yMax",&(y.maxValue),"yMax/D");
    
    
    TRandom3 r(sim.seedRandom);
    gRandom->SetSeed(sim.seedRandom);
    for (unsigned int eI=0; eI<nUseHdu; ++eI)  /* Main loop through each extension */
    {
        
        const unsigned int n = (singleHdu[0]==-1)? 2 : singleHdu[eI];
        const int nHDUsToProcess = nUseHdu;
        int nOut = eI+1;
        
        /* get output image dimensions and total number of pixels in image */
        int hdutype, bitpix, bytepix, naxis = 0;
        long naxes[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
        fits_movabs_hdu(outfptr, nOut, &hdutype, &status);
        for (int i = 0; i < 9; ++i) naxes[i] = 1;
        fits_get_img_param(outfptr, 9, &bitpix, &naxis, naxes, &status);
        long totpix = naxes[0] * naxes[1];
        
        /* Don't try to process data if the hdu is empty */
        if (hdutype != IMAGE_HDU || naxis == 0 || totpix == 0){
            continue;
        }
        
        bytepix = abs(bitpix) / 8;
        if(bytepix!=4 && bytepix!=8) return -1000;
        
        double* outArray = new double[totpix];
        
        for(int i=0;i<totpix;++i) outArray[i] = 0.0;

        //obatain blank
        evt.hdu =           nOut;
        evt.gain =          gc.getExtGain(evt.hdu);
        evt.noise =         gc.getExtNoise(evt.hdu);
        evt.dcMean =        gc.getExtDcMean(evt.hdu);
        evt.diffModel = gc.getExtDiffusionModel(evt.hdu);
        
        int xMin=1;
        int xMax=naxes[0];
        int yMin=1;
        int yMax=naxes[1];
        sim.nCols = naxes[0];
        sim.nRows = naxes[1];
        const int nLines = yMax - yMin +1;
        const int nCol = xMax - xMin + 1;
        const long npix =  nCol*nLines;
        double* sArray;
        if (!gSimulateBlanck) {
            int status = 0;
            double nulval = 0.0;
            int anynul = 0;
            long fpixel[2]={xMin,yMin};
            long lpixel[2]={xMax,yMax};
            
            long inc[2]={1,1};
            sArray = new double[npix];
            /* Read the images as doubles, regardless of actual datatype. */
            fits_movabs_hdu(infptr, n, &hdutype, &status);
            fits_read_subset(infptr, TDOUBLE, fpixel, lpixel, inc, &nulval, sArray, &anynul, &status);
        } else {
            
//            TRandom3 r(sim.seedRandom);
            //Produce only noise array
            sArray = new double[npix];
            for(int i=0; i<npix; i++) sArray[i]=r.Gaus(0.0,evt.noise)/evt.gain;
            //Noise + signal array
            for(int i=0; i<npix; i++) sArray[i]=sArray[i] + r.Poisson(evt.dcMean)/evt.gain;
        }
        
        
        if(gVerbosity){
            showProgress((eI)*3+0,3*nHDUsToProcess);
        }
        
        
        /*X and Y fixed coordinate*/
        vector <Double_t> xEventVec;
        vector <Double_t> yEventVec;
        uint nProcEvent = 0;
        if (gPositionGridFlag) {
            Int_t xEvent = floor(gDistAmongEventDefault/2);
            Int_t yEvent = floor(gDistAmongEventDefault/2);
            while (yEvent < yMax && nProcEvent < sim.nEvents) {
                while (xEvent < xMax && nProcEvent < sim.nEvents) {
                    xEventVec.push_back(xEvent);
                    yEventVec.push_back(yEvent);
                    xEvent = xEvent + gDistAmongEventDefault;
                    nProcEvent++;
                }
                xEvent = floor(gDistAmongEventDefault/2);
                yEvent = yEvent + gDistAmongEventDefault;
            }
        } else {
            while (nProcEvent < sim.nEvents) {
                xEventVec.push_back(0);
                yEventVec.push_back(0);
                nProcEvent++;
            }
        }
        
        /*X and Y distributions*/
        TF1 *fx = new TF1("fx",x.pdf.c_str(),x.minValue,x.maxValue);
        TF1 *fy = new TF1("fy",y.pdf.c_str(), y.minValue, y.maxValue);
        for (uint eventIndex = 0; eventIndex<xEventVec.size(); eventIndex++) {

            xEventVec[eventIndex] = xEventVec[eventIndex] + fx->GetRandom();
            yEventVec[eventIndex] = yEventVec[eventIndex] + fy->GetRandom();
//            cout << "xEventVec[eventIndex] " << xEventVec[eventIndex] << endl;
//            cout << "yEventVec[eventIndex]: " << yEventVec[eventIndex] << endl;
        }

        /*Ener and Z distributions*/
        TF1 *fEner = new TF1("fEner",energy.pdf.c_str(),energy.minValue,energy.maxValue);
        TF1 *fz = new TF1("fz",z.pdf.c_str(), z.minValue, z.maxValue);
        TF1 *fDiff = new TF1("fDiff",evt.diffModel.c_str());
        for (uint eventIndex=0; eventIndex<nProcEvent; eventIndex++) {
            
            evt.eventID = eventIndex;
            evt.flag = 0;
            //generar eventos de neutrinos
            evt.oMuX = xEventVec[eventIndex];
            evt.oMuY = yEventVec[eventIndex];
            
            //Generate hit parameters:
            evt.oEnergy = fEner->GetRandom();
            evt.oCharge = round(evt.oEnergy/sim.electronInEV);
            evt.oMuZ = fz->GetRandom();
            evt.oSigma = fDiff->Eval(evt.oMuZ);
            status = simulatehit(evt, sArray, totpix, xMin, xMax, yMin, yMax, nCol, sim);
            
            //guardar eventos en el root de salida
            hitSumm->Fill();
        }
        
        
        //guardar eventos en la imagen de salida
        for(int i=0; i<npix; i++) outArray[i]=outArray[i]+sArray[i];
        
        if(gVerbosity){
            showProgress((eI)*3+2,3*nHDUsToProcess);
        }
        
        fits_write_img(outfptr, TDOUBLE, 1, totpix, outArray, &status);
        
        delete[] outArray;
        delete[] sArray;
        
        //fileSummary  << "processed hdus: " << eI << endl;
        
        if(gVerbosity){
            showProgress((eI)*3+3,3*nHDUsToProcess);
        }
        
    }
    
    /* Close the output file */
    fits_close_file(outfptr,  &status);
    if (!gSimulateBlanck) {
        fits_close_file(infptr,  &status);
    }
    /*Close root file*/
    hitSumm->Write("");
    config->Fill();
    config->Write("");
    outTFile->Close();
    
    if(gVerbosity){
        showProgress(1,1);
        cout << endl << fileSummary.str() << endl;
    }
    return status;
}


void checkArch(){
    if(sizeof(float)*CHAR_BIT!=32 || sizeof(double)*CHAR_BIT!=64){
        cout << red;
        cout << "\n ========================================================================================\n";
        cout << "   WARNING: the size of the float and double variables is non-standard in this computer.\n";
        cout << "   The program may malfunction or produce incorrect results\n";
        cout << " ========================================================================================\n";
        cout << normal;
    }
}

int processCommandLineArgs(const int argc, char *argv[], vector<int> &singleHdu, string &inFile, string &outFile, string &confFile, vector <long> &naxesOut){
    
    if(argc == 1) return 1;
    bool outFileFlag = false;
    bool confFileFlag = false;
    inFile = "";
   
    singleHdu.clear();
    int opt=0;
    while ( (opt = getopt(argc, argv, "i:o:s:c:f:nqyhH?")) != -1) {
        switch (opt) {
            case 'o':
                if(!outFileFlag){
                    outFile = optarg;
                    outFileFlag = true;
                }
                else{
                    cerr << red << "\nError, can not set more than one output file!\n\n" << normal;
                    return 2;
                }
                break;
            case 's':
                singleHdu.push_back(atoi(optarg));
                break;
            case 'i':
                inFile = optarg;
                gSimulateBlanck = false;
                break;
//            case 'Q':
//            case 'p':
//                gPointEvents = true;
//                break;
            case 'f':
                gPositionGridFlag = true;
                gDistAmongEventDefault = atoi(optarg);
                break;
            case 'c':
                if(!confFileFlag){
                    confFile = optarg;
                    confFileFlag = true;
                }
                else{
                    cerr << red << "\nError, can not set more than one configuration file!\n\n" << normal;
                    return 2;
                }
                break;
            case 'q':
                gVerbosity = 0;
                break;
            case 'y':
                gKeepParallelOvsc = true;
                break;
            case 'h':
            case 'H':
            default: /* '?' */
                return 1;
        }
    }

    if(!outFileFlag){
        cerr << red << "\nError: output filename missing.\n" << normal;
        return 2;
    }
    
    if (!gSimulateBlanck) {
        if(!fileExist(inFile.c_str())){
            cout << red << "\nError reading input file: " << inFile <<"\nThe file doesn't exist!\n\n" << normal;
            return 1;
        }
    }
    
    return 0;
}
int getSimulationParameters(simulationConfigTree_t &sim) {
    //get configuration instance
    gConfig &gc = gConfig::getInstance();
    sim.nCols = gc.getNumColsNewImage();
    sim.nRows = gc.getNumRowsNewImage();
    sim.seedRandom = gc.getSeedRandom();
    sim.ccdThick = gc.getCCDThickness();
    sim.pixelSize = gc.getPixelSize();
    sim.electronInEV = gc.getElectronIonizationEnergy();
    sim.nEvents = gc.getNumOfEvents();
//    sim.minDistEvent = gc.getDistAmongEvent();
    return 0;
}

int main(int argc, char *argv[])
{
    
    checkArch(); //Check the size of the double and float variables.
    
    time_t start,end;
    double dif;
    time (&start);
    
    string outFile;
    string inFile;
    vector<int> singleHdu;
    vector<long> naxesOut;
    string confFile;

    int returnCode = processCommandLineArgs( argc, argv, singleHdu, inFile, outFile, confFile, naxesOut);
    if(returnCode!=0){
        if(returnCode == 1) printCopyHelp(argv[0],true);
        if(returnCode == 2) printCopyHelp(argv[0]);
        return returnCode;
    }
    
    /* Create configuration singleton and read configuration file */
    gConfig &gc = gConfig::getInstance();
    if(gc.readConfFile(confFile.c_str()) == false){
        return 1;
    }
    if(gVerbosity){
        cout << "\nConfig file: " << confFile << endl;
        gc.printVariables();
    }
    
    if(gVerbosity){
        cout << bold << "\nWill read the following file:\n" << normal;
        cout << "\t" << inFile << endl;
        cout << bold << "\nThe output will be saved in the file:\n\t" << normal << outFile << endl;
        cout << bold << "\nConfiguration file: " << confFile << endl;
    }
    
    
    /* Overwrite the output file if it already exist */
    if(fileExist(outFile.c_str())){
        cout << yellow << "\nThe output file exist. " << normal;
        deleteFile(outFile.c_str());
    }

    /* Do the actual processing */
    int status = 0;
    simulationConfigTree_t sim;
    status = getSimulationParameters(sim);
    if (!gSimulateBlanck) {
        status = copyStructure( inFile,  outFile.c_str(), singleHdu, sim);
    } else {
        status = createStructure( inFile,  outFile.c_str(), singleHdu, naxesOut, sim);
    }
    
    if (status != 0){
        fits_report_error(stderr, status);
        return status;
    }
    

    status = computeImagePointEvents( inFile,  outFile.c_str(), singleHdu, sim);

    
    if (status != 0){
        fits_report_error(stderr, status);
        return status;
    }
    
    /* Report */
    time (&end);
    dif = difftime (end,start);
    if(gVerbosity) cout << green << "All done!\n" << bold << "-> It took me " << dif << " seconds to do it!\n\n" << normal;
    
    return status;
}
