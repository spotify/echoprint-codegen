//
//  Copyright 2011 The Echo Nest. All rights reserved.
//

#include <sstream>
#include <iostream>
#include <iomanip>
#include "Codegen.h"
#include "AudioBufferInput.h"
#include "Spectrogram.h"
#include "Fingerprint.h"
#include "FingerprintLowRank.h"

#include "Base64.h"
#include "easyzlib.h"

#ifdef __APPLE__
// iOS debugging
extern "C" {
	extern void NSLog(CFStringRef format, ...); 
}
#endif

Fingerprint* Codegen::computeFullFingerprint(SubbandAnalysis *pSubbandAnalysis, int start_offset) {
    Fingerprint *pFingerprint = new Fingerprint(pSubbandAnalysis, start_offset);
    pFingerprint->Compute();
    return pFingerprint;
}

FingerprintLowRank* Codegen::computeLowRankFingerprint(Spectrogram *p16Spectrogram, Spectrogram *p512Spectrogram, int start_offset) {
    FingerprintLowRank *pFingerprint = new FingerprintLowRank(p16Spectrogram, p512Spectrogram, start_offset);
    pFingerprint->Compute();
    return pFingerprint;
}

Codegen::Codegen(const float* pcm, uint numSamples, int start_offset, bool full, bool lowrank) {
    if (Params::AudioStreamInput::MaxSamples < (uint)numSamples)
        throw std::runtime_error("File was too big\n");

    if (!full && !lowrank) 
        throw std::runtime_error("Need at least one stage\n");
        
    AudioBufferInput *pAudio = new AudioBufferInput();
    pAudio->SetBuffer(pcm, numSamples);
    _LowRankCodeString = "";
    _LowRankNumCodes = 0;
    if(full) {
        Fingerprint * pFingerprint;
        SubbandAnalysis *pSubbandAnalysis = new SubbandAnalysis(pAudio);
        pSubbandAnalysis->Compute();
        pFingerprint = computeFullFingerprint(pSubbandAnalysis, start_offset);
        _FullCodeString = createCodeString(pFingerprint->getCodes());
        _FullNumCodes = pFingerprint->getCodes().size();
        delete pFingerprint;
        delete pSubbandAnalysis;
    }
    if(lowrank) {
        Spectrogram *p16Spectrogram = new Spectrogram(pAudio, 8, 16, 16);
        p16Spectrogram->Compute();
        Spectrogram *p512Spectrogram = new Spectrogram(pAudio, 128, 512, 512);
        p512Spectrogram->Compute();
        FingerprintLowRank * pFingerprint;
        pFingerprint = computeLowRankFingerprint(p16Spectrogram, p512Spectrogram, start_offset);
        _LowRankCodeString = createCodeString(pFingerprint->getCodes());
        _LowRankNumCodes = pFingerprint->getCodes().size();
        delete pFingerprint;
        delete p512Spectrogram;
    }
    delete pAudio;
}

string Codegen::createCodeString(vector<FPCode> vCodes) {
    if (vCodes.size() < 3) {
        return "";
    }
    std::ostringstream codestream;
    codestream << std::setfill('0') << std::hex;
    for (uint i = 0; i < vCodes.size(); i++)
        codestream << std::setw(5) << vCodes[i].frame;
    
    for (uint i = 0; i < vCodes.size(); i++) {
        int hash = vCodes[i].code;
        codestream << std::setw(5) << hash;
    }
    return compress(codestream.str());
}


string Codegen::compress(const string& s) {
    long nDest = (long)EZ_COMPRESSMAXDESTLENGTH((float)s.size());
    unsigned char *pDest = new unsigned char[nDest];
    ezcompress(pDest, &nDest, (unsigned char*)s.c_str(), s.size());
    string encoded = base64_encode(pDest, nDest, true);
    delete [] pDest;
    return encoded;
}
