//
//  echoprint-codegen
//  Copyright 2011 The Echo Nest Corporation. All rights reserved.
//


#ifndef CODEGEN_H
#define CODEGEN_H

// Entry point for generating codes from PCM data.
#define ECHOPRINT_VERSION 4.11

#include <string>
#include <vector>

using std::string;
using std::vector;

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
class SubbandAnalysis;
struct FPCode;

class CODEGEN_API Codegen {
public:
    Codegen(const float* pcm, unsigned int numSamples, int start_offset);

    string getCodeString(){return _CodeString;}
    int getNumCodes(){return _NumCodes;}
    static double getVersion() { return ECHOPRINT_VERSION; }
private:
    Fingerprint* computeFingerprint(SubbandAnalysis *pSubbandAnalysis, int start_offset);
    string createCodeString(vector<FPCode> vCodes);

    string compress(const string& s);
    string _CodeString;
    int _NumCodes;
};

#endif
