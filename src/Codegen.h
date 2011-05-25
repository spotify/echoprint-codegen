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
class FingerprintLowRank;

class CODEGEN_API Codegen {
public:
    Codegen(const float* pcm, uint numSamples, int start_offset, bool full, bool lowrank);

    string getFullCodeString(){return _FullCodeString;}
    string getLowRankCodeString(){return _LowRankCodeString;}
    int getFullNumCodes(){return _FullNumCodes;}
    int getLowRankNumCodes(){return _LowRankNumCodes;}
    float getVersion() { return VERSION; }
private:
    Fingerprint* computeFullFingerprint(Spectrogram *p16Spectrogram, int start_offset);
    FingerprintLowRank* computeLowRankFingerprint(Spectrogram *p16Spectrogram, Spectrogram *p512Spectrogram, int start_offset);
    string createCodeString(vector<FPCode> vCodes);

    string compress(const string& s);
    string _FullCodeString;
    string _LowRankCodeString;
    int _FullNumCodes;
    int _LowRankNumCodes;
};

#endif
