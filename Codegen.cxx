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

#include "Base64.h"
#include "easyzlib.h"

#ifdef __APPLE__
// iOS debugging
extern "C" {
	extern void NSLog(CFStringRef format, ...); 
}
#endif

Fingerprint* Codegen::computeFingerprint(auto_ptr<AudioBufferInput>pAudio, auto_ptr<Spectrogram>p16Spectrogram, int start_offset) {
    Fingerprint *pFingerprint = new Fingerprint(p16Spectrogram.get(), start_offset);
    pFingerprint->Compute();
    return pFingerprint;
}

Codegen::Codegen(auto_ptr<AudioBufferInput>pAudio, auto_ptr<Spectrogram>p16Spectrogram, int start_offset) {
    Fingerprint * pFingerprint;
    pFingerprint = computeFingerprint(pAudio, p16Spectrogram, start_offset);
    setCodeString(pFingerprint);
    delete pFingerprint;

}

Codegen::Codegen(const float* pcm, uint numSamples, int start_offset) {
    if (Params::AudioStreamInput::MaxSamples < (uint)numSamples)
        throw std::runtime_error("File was too big\n");


    auto_ptr<AudioBufferInput> pAudio(new AudioBufferInput());
    pAudio->SetBuffer(pcm, numSamples);
    auto_ptr<Spectrogram> p16Spectrogram(new Spectrogram(pAudio.get(), 8, 16, 16));
    p16Spectrogram->Compute();

    Fingerprint * pFingerprint;
    pFingerprint = computeFingerprint(pAudio, p16Spectrogram, start_offset);

    setCodeString(pFingerprint);

    delete pFingerprint;
}

void Codegen::setCodeString(Fingerprint *pFingerprint) {
    vector<FPCode> vCodes = pFingerprint->getCodes();
    _NumCodes = vCodes.size();

    if (_NumCodes < 3) {
        _CodeString = "";
        return;
    }
        
    // Can constrain decimated time to 5 hex digits, since that represents about 3 hours of audio (we only support 1)
    // hash codes are 16-bit numbers, so we need 4 hex digits to represent them.
    std::ostringstream codestream;
    codestream << std::setfill('0') << std::hex;
    for (uint i = 0; i < vCodes.size(); i++)
        codestream << std::setw(5) << vCodes[i].frame;
    
    for (uint i = 0; i < vCodes.size(); i++)
    {
        int hash = vCodes[i].code;
        codestream << std::setw(8) << hash;
    }
    _CodeString = compress(codestream.str());
}


string Codegen::compress(const string& s) {
    long nDest = (long)EZ_COMPRESSMAXDESTLENGTH((float)s.size());
    auto_ptr<unsigned char> pDest(new unsigned char[nDest]);
    ezcompress(pDest.get(), &nDest, (unsigned char*)s.c_str(), s.size());
    return base64_encode(pDest.get(), nDest, true);
}
