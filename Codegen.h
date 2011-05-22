//
//  Copyright 2011 The Echo Nest. All rights reserved.
//

#ifndef CODEGEN_H
#define CODEGEN_H

// Entry point for generating codes from PCM data.
#define VERSION 4.00

#include <memory>
#include <string>

#include "Common.h"
#include "AudioBufferInput.h"
#include "Spectrogram.h"
#include "Fingerprint.h"

using namespace std;

#ifdef _MSC_VER
    #ifdef CODEGEN_EXPORTS
        #define CODEGEN_API __declspec(dllexport)
        #pragma message("Exporting codegen.dll")
    #else
        #define CODEGEN_API __declspec(dllimport)
        #pragma message("Importing codegen.dll")
    #endif
#else
    #define CODEGEN_API
#endif

class Fingerprint;
class FingerprintStage2;

class CODEGEN_API Codegen {
public:
    Codegen(const float* pcm, uint numSamples, int start_offset, bool stage1, bool stage2);

    string getStage1CodeString(){return _Stage1CodeString;}
    string getStage2CodeString(){return _Stage2CodeString;}
    int getStage1NumCodes(){return _Stage1NumCodes;}
    int getStage2NumCodes(){return _Stage2NumCodes;}
    float getVersion() { return VERSION; }
private:
    Fingerprint* computeStage1Fingerprint(Spectrogram *p16Spectrogram, int start_offset);
    FingerprintStage2* computeStage2Fingerprint(Spectrogram *p16Spectrogram, Spectrogram *p512Spectrogram, int start_offset);
    string createCodeString(vector<FPCode> vCodes);

    string compress(const string& s);
    string _Stage1CodeString;
    string _Stage2CodeString;
    int _Stage1NumCodes;
    int _Stage2NumCodes;
};

#endif
