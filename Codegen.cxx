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
#include "FingerprintStage2.h"

#include "Base64.h"
#include "easyzlib.h"

#ifdef __APPLE__
// iOS debugging
extern "C" {
	extern void NSLog(CFStringRef format, ...); 
}
#endif

Fingerprint* Codegen::computeStage1Fingerprint(Spectrogram *p16Spectrogram, int start_offset) {
    Fingerprint *pFingerprint = new Fingerprint(p16Spectrogram, start_offset);
    pFingerprint->Compute();
    return pFingerprint;
}

FingerprintStage2* Codegen::computeStage2Fingerprint(Spectrogram *p16Spectrogram, Spectrogram *p512Spectrogram, int start_offset) {
    FingerprintStage2 *pFingerprint = new FingerprintStage2(p16Spectrogram, p512Spectrogram, start_offset);
    pFingerprint->Compute();
    return pFingerprint;
}

Codegen::Codegen(const float* pcm, uint numSamples, int start_offset, bool stage1, bool stage2) {
    if (Params::AudioStreamInput::MaxSamples < (uint)numSamples)
        throw std::runtime_error("File was too big\n");

    if (!stage1 && !stage2) 
        throw std::runtime_error("Need at least one stage\n");
        
    AudioBufferInput *pAudio = new AudioBufferInput();
    pAudio->SetBuffer(pcm, numSamples);
    Spectrogram *p16Spectrogram = new Spectrogram(pAudio, 8, 16, 16);
    p16Spectrogram->Compute();

    if(stage1) {
        Fingerprint * pFingerprint;
        pFingerprint = computeStage1Fingerprint(p16Spectrogram, start_offset);
        _Stage1CodeString = createCodeString(pFingerprint->getCodes());
        _Stage1NumCodes = pFingerprint->getCodes().size();
        delete pFingerprint;
    }
    if(stage2) {
        Spectrogram *p512Spectrogram = new Spectrogram(pAudio, 128, 512, 512);
        p512Spectrogram->Compute();
        FingerprintStage2 * pFingerprint;
        pFingerprint = computeStage2Fingerprint(p16Spectrogram, p512Spectrogram, start_offset);
        _Stage2CodeString = createCodeString(pFingerprint->getCodes());
        _Stage2NumCodes = pFingerprint->getCodes().size();
        delete pFingerprint;
        delete p512Spectrogram;
    }
    delete p16Spectrogram;
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
    auto_ptr<unsigned char> pDest(new unsigned char[nDest]);
    ezcompress(pDest.get(), &nDest, (unsigned char*)s.c_str(), s.size());
    return base64_encode(pDest.get(), nDest, true);
}
