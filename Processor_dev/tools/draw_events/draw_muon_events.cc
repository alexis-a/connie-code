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
    Int_t oCharge;
    Int_t hdu;
    Double_t oSigma;
    Double_t oMuX;
    Double_t oMuY;
    Double_t oMuZ;
    Double_t noise;
    Double_t dcMean;
    Double_t gain;
    //Double_t diffParam1;
    //Double_t diffParam2;
    string diffModel;
    
    hitTreeEntry_t(): oEnergy(-1), oCharge(-1), hdu(-1), oSigma(-1), oMuX(-1), oMuY(-1), noise(-1), dcMean(-1), gain(-1), diffModel("")
    {
    };
    ~hitTreeEntry_t()
    {
    };
};

struct muonTreeEntry_t {
    Double_t dEdx;
    Double_t dQdx;
    Int_t oCharge;
    Int_t hdu;
    Double_t oInitX;
    Double_t oInitY;
    Double_t oFinX;
    Double_t oFinY;
    Double_t oZenith;
    Double_t oAzimuth;
    Double_t tLength;
    Double_t iLength;
    Double_t aModel;
    Double_t bModel;
    
    
    muonTreeEntry_t(): dEdx(-1), dQdx(-1), oCharge(-1), hdu(-1), oInitX(-1), oInitY(-1), oZenith(-1), oAzimuth(-1), tLength(-1), iLength(-1), aModel(-1), bModel(-1)
    {
    };
    ~muonTreeEntry_t()
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
        cout << "This program computes overscan mean and subtracts it line by line.\n";
        cout << "It handles all the available HDUs. The HDUs in the output fit file\n";
        cout << "will be double (64bits) for 64bits images and float (32bis) in all\n";
        cout << "the other cases.\n";
        cout << "The card \"TRIMSEC\" must be present in the header to use this program.\n";
        cout << normal;
    }
    cout << "==========================================================================\n";
    cout << yellow;
    cout << "\nUsage:\n";
    cout << "  "   << exeName << " <input file> -o <output filename> \n\n";
    cout << "\nOptions:\n";
    cout << "  -q for quiet (no screen output)\n";
    cout << "  -y for keeping the y-overscan\n";
    cout << "  -f for processing images taken at Fermilab (channels 1 & 12)\n";
    cout << "  -s <HDU number> for processing a single HDU \n\n";
    cout << normal;
    cout << blue;
    cout << "For any problems or bugs contact Javier Tiffenberg <javiert@fnal.gov>\n\n";
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
    fits_create_img(outfptr, -32, naxis, naxes, &status);
    singleHdu.push_back(1);
    if (status != 0) return(status);
    
    
    
    
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
            /* create output image with the same size as the input image*/
//            int xMin=0;
//            int xMax=naxes[0];
//            int yMin=0;
//            int yMax=naxes[1];
            
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
            //cout << "Electron out of matrix." << endl;
            //return 1;
            continue;
        }
        arrayindex=xindex-1+((yindex-1)*nCol);
        outArray[arrayindex]=outArray[arrayindex] + 1.0*evt.gain;
        
    }
    
    return 0;
}

int simulateMuon(muonTreeEntry_t &evt, double* outArray, const int totpix, const int xMin, const int xMax, const int yMin, const int yMax, const int nCol, const simulationConfigTree_t &sim) {
//    TRandom3 r(sim.seedRandom);
//    
//    
//    //Orginal point of electrons
//    double slopeXY = (evt.oFinY - evt.oInitY)/(evt.oFinX-evt.oInitX);
//    double* xElecInit = new double[evt.oCharge];
//    double* yElecInit = new double[evt.oCharge];
//    for(int q=0; q<evt.oCharge; q++) {
//        xElecInit[q] = r.Uniform(evt.oInitX,evt.oFinX);
//        yElecInit[q] = evt.oInitY + slopeXY*(xElecInit[q]-evt.oInitX);
//    }
//    
//    //Diffusion simulation
//    double* xElec = new double[evt.oCharge];
//    
//    double* yElec = new double[evt.oCharge];
//    double z= 0.0;
//    double sigma = 0.0;
//    for(int q=0; q<evt.oCharge; q++) {
//        z= sim.pixelSize*tan(pi/2-evt.oZenith)*sqrt(pow(xElecInit[q]-evt.oInitX,2) + pow(yElecInit[q]-evt.oInitY,2));
//        sigma = sqrt(-evt.aModel*log(1 - evt.bModel*z))/sim.pixelSize;
//        xElec[q]=r.Gaus(xElecInit[q],sigma);
//        yElec[q]=r.Gaus(yElecInit[q],sigma);
//    }
//    
//    //Fill signal array
//    int xindex, yindex, arrayindex;
//    for(int q=0; q<evt.oCharge; q++) {
//        xindex=(int)round(xElec[q]);
//        yindex=(int)round(yElec[q]);
//        if(xindex>xMax || xindex < xMin ||  yindex>yMax || yindex<yMin) {
//            //cout << "Electron out of matrix." << endl;
//            //return 1;
//            continue;
//        }
//        arrayindex=xindex-1+((yindex-1)*nCol);
//        outArray[arrayindex]=outArray[arrayindex] + 1.0*gain;
//        
//    }
    
    return 0;
}

int computeImageMuons(const string inFile, const char *outF, vector<int> &singleHdu, const simulationConfigTree_t &sim){
    int status = 0;
//    int nhdu = 0;
//    
//    /* Open the output file */
//    fitsfile  *outfptr; /* FITS file pointers defined in fitsio.h */
//    fits_open_file(&outfptr, outF, READWRITE, &status);
//    if (status != 0) return(status);
//    
//    fits_get_num_hdus(outfptr, &nhdu, &status);
//    
//    fitsfile  *infptr; /* FITS file pointers defined in fitsio.h */
//    const char* inF = inFile.c_str();
//    if (!gSimulateBlanck) {
//        fits_open_file(&infptr, inF, READONLY, &status); /* Open the input file */
//        if (status != 0) return(status);
//    }
//    
//    const unsigned int nUseHdu = (singleHdu[0]==-1)? 1 : singleHdu.size();
//    
//    ostringstream fileSummary;
//    
//    string rootOutFile(outF);
//    rootOutFile.append(".root");
//    TFile *outTFile = new TFile(rootOutFile.c_str(),"RECREATE");
//    outTFile->cd();
//    TTree *hitSumm = new TTree("hitSumm","hitSumm");
//    /*create output files and tree*/
//    muonTreeEntry_t evt;
//    hitSumm->Branch("hdu",&(evt.hdu),"hdu/I");
//    hitSumm->Branch("dEdx",&(evt.dEdx),"dEdx/D");
//    hitSumm->Branch("dQdx",&(evt.dQdx),"dQdx/D");
//    hitSumm->Branch("oCharge",&(evt.oCharge),"oCharge/I");
//    hitSumm->Branch("oInitX",&(evt.oInitX),"oInitX/D");
//    hitSumm->Branch("oInitY",&(evt.oInitY),"oInitY/D");
//    hitSumm->Branch("oFinX",&(evt.oFinX),"oFinX/D");
//    hitSumm->Branch("oFinY",&(evt.oFinY),"oFinY/D");
//    hitSumm->Branch("oZenith",&(evt.oZenith),"oZenith/D");
//    hitSumm->Branch("oAzimuth",&(evt.oAzimuth),"oAzimuth/D");
//    hitSumm->Branch("tLength",&(evt.tLength),"tLength/D");
//    hitSumm->Branch("iLength",&(evt.iLength),"iLength/D");
//    hitSumm->Branch("aModel",&(evt.aModel),"aModel/D");
//    hitSumm->Branch("bModel",&(evt.bModel),"bModel/D");
//    hitSumm->Branch("gain",&gain,"gain/D");
//    hitSumm->Branch("dcMean",&dcMean,"dcMean/D");
//    hitSumm->Branch("noiseSigma",&noiseSigma,"noiseSigma/D");
//    
//    TRandom3 r(seedRandom);
//    for (unsigned int eI=0; eI<nUseHdu; ++eI)  /* Main loop through each extension */
//    {
//        
//        const unsigned int n = (singleHdu[0]==-1)? 2 : singleHdu[eI];
//        const int nHDUsToProcess = nUseHdu;
//        int nOut = eI+1;
//        
//        /* get output image dimensions and total number of pixels in image */
//        int hdutype, bitpix, bytepix, naxis = 0;
//        long naxes[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
//        fits_movabs_hdu(outfptr, nOut, &hdutype, &status);
//        for (int i = 0; i < 9; ++i) naxes[i] = 1;
//        fits_get_img_param(outfptr, 9, &bitpix, &naxis, naxes, &status);
//        long totpix = naxes[0] * naxes[1];
//        
//        /* Don't try to process data if the hdu is empty */
//        if (hdutype != IMAGE_HDU || naxis == 0 || totpix == 0){
//            continue;
//        }
//        
//        bytepix = abs(bitpix) / 8;
//        if(bytepix!=4 && bytepix!=8) return -1000;
//        
//        double* outArray = new double[totpix];
//        
//        for(int i=0;i<totpix;++i) outArray[i] = 0.0;
//        
//        
//        
//        int xMin=1;
//        int xMax=naxes[0];
//        int yMin=1;
//        int yMax=naxes[1];
//        const int nLines = yMax - yMin +1;
//        const int nCol = xMax - xMin + 1;
//        const long npix =  nCol*nLines;
//        double* sArray;
//        if (!gSimulateBlanck) {
//            int status = 0;
//            double nulval = 0.0;
//            int anynul = 0;
//            long fpixel[2]={xMin,yMin};
//            long lpixel[2]={xMax,yMax};
//            
//            long inc[2]={1,1};
//            sArray = new double[npix];
//            /* Read the images as doubles, regardless of actual datatype. */
//            fits_movabs_hdu(infptr, n, &hdutype, &status);
//            fits_read_subset(infptr, TDOUBLE, fpixel, lpixel, inc, &nulval, sArray, &anynul, &status);
//        } else {
//            
//            TRandom3 r(sim.seedRandom);
//            //Produce only noise array
//            sArray = new double[npix];
//            for(int i=0; i<npix; i++) sArray[i]=gain*r.Gaus(0.0,noiseSigma);
//            //Noise + signal array
//            for(int i=0; i<npix; i++) sArray[i]=sArray[i] + gain*r.Poisson(dcMean);
//        }
//        
//        ///
//        
//        
//        
//        if(gVerbosity){
//            showProgress((eI)*3+0,3*nHDUsToProcess);
//        }
//        
//        
//        
//        evt.hdu = nOut;
//        vector <Int_t> xEventVec;
//        vector <Int_t> yEventVec;
//        uint nProcEvent = 0;
//        if (!gRandomPos) {
//            Int_t xEvent = floor(minDistEvent/2);
//            Int_t yEvent = floor(minDistEvent/2);
//            while (yEvent < yMax && nProcEvent < Nevent) {
//                while (xEvent < xMax && nProcEvent < Nevent) {
//                    xEventVec.push_back(xEvent);
//                    yEventVec.push_back(yEvent);
//                    xEvent = xEvent + minDistEvent;
//                    nProcEvent++;
//                }
//                xEvent = floor(minDistEvent/2);
//                yEvent = yEvent + minDistEvent;
//            }
//        } else {
//            while (nProcEvent < Nevent) {
//                xEventVec.push_back(r.Uniform(floor(xMin + 10)-0.5,floor(xMax - 10)+0.5));
//                yEventVec.push_back(r.Uniform(floor(yMin + 10)-0.5,floor(yMax - 10)+0.5));
//                nProcEvent++;
//            }
//        }
//        
//        for (uint eventIndex=0; eventIndex<nProcEvent; eventIndex++) {
//            
//            //generar eventos de neutrinos
//            evt.dEdx = dEdxFixed;
//            evt.aModel = aModel;
//            evt.bModel = bModel;
//            evt.oInitX = xEventVec[eventIndex];
//            evt.oInitY = yEventVec[eventIndex];
//            //TF1 *f1 = new TF1("f1","cos(x)*cos(x)",0,pi/2);
//            TF1 *f1 = new TF1("f1","1",1,1+0.001);
//            evt.oZenith = f1->GetRandom();
//            cout << "evt.oZenith" << evt.oZenith << endl;
//            evt.oAzimuth = r.Uniform(-pi,pi);
//            evt.tLength = (ccdThick/pixelSize)/sin(pi/2-evt.oZenith);
//            evt.iLength = (ccdThick/pixelSize)/tan(pi/2-evt.oZenith);
//            evt.oFinX = evt.oInitX + evt.iLength*cos(evt.oAzimuth);
//            evt.oFinY = evt.oInitY + evt.iLength*sin(evt.oAzimuth);
//            evt.dQdx = dQdxFixed;
//            evt.oCharge = round(evt.dQdx*evt.tLength*pixelSize);
//            
//            //generar eventos de muones
//            int statusSim = simulateMuon(evt, sArray, totpix, xMin, xMax, yMin, yMax, nCol, sim);
//            
//            //guardar eventos en el root de salida
//            hitSumm->Fill();
//            
//        }
//        
//        //guardar eventos en la imagen de salida
//        for(int i=0; i<npix; i++) outArray[i]=outArray[i]+sArray[i];
//        
//        if(gVerbosity){
//            showProgress((eI)*3+2,3*nHDUsToProcess);
//        }
//        
//        fits_write_img(outfptr, TDOUBLE, 1, totpix, outArray, &status);
//        
//        delete[] outArray;
//        delete[] sArray;
//        
//        //fileSummary  << "processed hdus: " << eI << endl;
//        
//        if(gVerbosity){
//            showProgress((eI)*3+3,3*nHDUsToProcess);
//        }
//        
//    }
//    
//    /* Close the output file */
//    fits_close_file(outfptr,  &status);
//    if (!gSimulateBlanck) {
//        fits_close_file(infptr,  &status);
//    }
//    /*Close root file*/
//    hitSumm->Write("");
//    outTFile->Close();
//    
//    if(gVerbosity){
//        showProgress(1,1);
//        
//        cout << endl << fileSummary.str() << endl;
//    }
    return status;
}



int computeImagePointEvents(const string inFile, const char *outF, vector<int> &singleHdu, simulationConfigTree_t &sim){
    int status = 0;
    int nhdu = 0;
    
    //get configuration instance
    gConfig &gc = gConfig::getInstance();
    
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
    hitSumm->Branch("hdu",&(evt.hdu),"hdu/I");
    hitSumm->Branch("oEnergy",&(evt.oEnergy),"oEnergy/I");
    hitSumm->Branch("oCharge",&(evt.oCharge),"oCharge/I");
    hitSumm->Branch("oMuX",&(evt.oMuX),"oMuX/D");
    hitSumm->Branch("oMuY",&(evt.oMuY),"oMuY/D");
    hitSumm->Branch("oSigma",&(evt.oSigma),"oSigma/D");
    hitSumm->Branch("dcMean",&(evt.dcMean),"dcMean/D");
    hitSumm->Branch("noise",&(evt.noise),"noise/D");
    hitSumm->Branch("gain",&(evt.gain),"gain/D");
    
    /*config branches*/
    config->Branch("nEvents",&(sim.nEvents),"nEvents/I");
    config->Branch("nCols",&(sim.nCols),"nCols/I");
    config->Branch("nRows",&(sim.nRows),"nRows/I");
    config->Branch("seedRandom",&(sim.seedRandom),"seedRandom/I");
    config->Branch("ccdThick",&(sim.ccdThick),"ccdThick/D");
    config->Branch("pixelSize",&(sim.pixelSize),"pixelSize/D");
    config->Branch("electronInEV",&(sim.electronInEV),"electronInEV/D");
    
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
    
    TRandom3 r(sim.seedRandom);
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
        //evt.diffParam1 =    gc.getExtDiffParam1(evt.hdu);
        //evt.diffParam2 =    gc.getExtDiffParam1(evt.hdu);
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
            
            TRandom3 r(sim.seedRandom);
            //Produce only noise array
            sArray = new double[npix];
            for(int i=0; i<npix; i++) sArray[i]=evt.gain*r.Gaus(0.0,evt.noise);
            //Noise + signal array
            for(int i=0; i<npix; i++) sArray[i]=sArray[i] + evt.gain*r.Poisson(evt.dcMean);
        }
        
        
        if(gVerbosity){
            showProgress((eI)*3+0,3*nHDUsToProcess);
        }
        
        
        vector <Int_t> xEventVec;
        vector <Int_t> yEventVec;
        uint nProcEvent = 0;
        if (!gRandomPos) {
            Int_t xEvent = floor(sim.minDistEvent/2);
            Int_t yEvent = floor(sim.minDistEvent/2);
            while (yEvent < yMax && nProcEvent < sim.nEvents) {
                while (xEvent < xMax && nProcEvent < sim.nEvents) {
                    xEventVec.push_back(xEvent);
                    yEventVec.push_back(yEvent);
                    xEvent = xEvent + sim.minDistEvent;
                    nProcEvent++;
                }
                xEvent = floor(sim.minDistEvent/2);
                yEvent = yEvent + sim.minDistEvent;
            }
        } else {
            /*X and Y distributions*/
            TF1 *fx = new TF1("fx",x.pdf.c_str(),x.minValue,x.maxValue);
            TF1 *fy = new TF1("fy",y.pdf.c_str(), y.minValue, y.maxValue);
            while (nProcEvent < sim.nEvents) {
                xEventVec.push_back(fx->GetRandom());
                yEventVec.push_back(fy->GetRandom());
//                xEventVec.push_back(r.Uniform(floor(xMin + 10)-0.5,floor(xMax - 10)+0.5));
//                yEventVec.push_back(r.Uniform(floor(yMin + 10)-0.5,floor(yMax - 10)+0.5));
                nProcEvent++;
            }
        }
        
//        for (uint i=0; i<xEventVec.size(); i++) {
//            cout << xEventVec[i] << " " << yEventVec[i] << endl;
//        }
        
        /*X and Y distributions*/
        TF1 *fEner = new TF1("fEner",energy.pdf.c_str(),energy.minValue,energy.maxValue);
        TF1 *fz = new TF1("fz",z.pdf.c_str(), z.minValue, z.maxValue);
        TF1 *fDiff = new TF1("fDiff",evt.diffModel.c_str());
        for (uint eventIndex=0; eventIndex<nProcEvent; eventIndex++) {
            
            //generar eventos de neutrinos
            evt.oMuX = xEventVec[eventIndex];
            evt.oMuY = yEventVec[eventIndex];
            
            //Generate hit parameters:
            //evt.oCharge=r.Gaus(Qevent,sqrt(Qevent*fanoFactor));
            evt.oEnergy = fEner->GetRandom();
            evt.oCharge = round(evt.oEnergy/sim.electronInEV);
            evt.oMuZ = fz->GetRandom();
            evt.oSigma = fDiff->Eval(evt.oMuZ);
            int statusSim = simulatehit(evt, sArray, totpix, xMin, xMax, yMin, yMax, nCol, sim);
            
            //generar eventos de muones
            
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

int processCommandLineArgs(const int argc, char *argv[], vector<int> &singleHdu, string &inFile, string &outFile, vector <long> &naxesOut){
    
    if(argc == 1) return 1;
    bool outFileFlag = false;
    inFile = "";
    
    singleHdu.clear();
    int opt=0;
    while ( (opt = getopt(argc, argv, "i:o:s:pmrnqyQhH?")) != -1) {
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
            case 'Q':
            case 'p':
                gPointEvents = true;
                break;
            case 'r':
                gRandomPos = true;
                break;
            case 'm':
                gPointEvents = false;
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
    sim.minDistEvent = gc.getDistAmongEvent();
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
    //string confFile = "extractConfig.xml";
    string confFile = "simulationConfig.xml";
    
    int returnCode = processCommandLineArgs( argc, argv, singleHdu, inFile, outFile, naxesOut);
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
    
    if (gPointEvents) {
        status = computeImagePointEvents( inFile,  outFile.c_str(), singleHdu, sim);
    } else {
        status = computeImageMuons( inFile,  outFile.c_str(), singleHdu, sim);
    }
    
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
