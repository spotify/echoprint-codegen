//
//  Copyright 2011 The Echo Nest. All rights reserved.
//

#include <sstream>
#include <iostream>
#include <iomanip>
#include "Codegen.h"
#include "AudioBufferInput.h"
#include "Fingerprint.h"

#include "Base64.h"
#include "easyzlib.h"

Fingerprint* Codegen::computeFingerprint(SubbandAnalysis *pSubbandAnalysis, int start_offset) {
    Fingerprint *pFingerprint = new Fingerprint(pSubbandAnalysis, start_offset);
    pFingerprint->Compute();
    return pFingerprint;
}

Codegen::Codegen(const float* pcm, uint numSamples, int start_offset) {
    if (Params::AudioStreamInput::MaxSamples < (uint)numSamples)
        throw std::runtime_error("File was too big\n");
        
    AudioBufferInput *pAudio = new AudioBufferInput();
    pAudio->SetBuffer(pcm, numSamples);
    Fingerprint * pFingerprint;
    SubbandAnalysis *pSubbandAnalysis = new SubbandAnalysis(pAudio);
    pSubbandAnalysis->Compute();
    pFingerprint = computeFingerprint(pSubbandAnalysis, start_offset);
    _CodeString = createCodeString(pFingerprint->getCodes());
    _NumCodes = pFingerprint->getCodes().size();
    delete pFingerprint;
    delete pSubbandAnalysis;
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
