//
//  Copyright 2011 The Echo Nest. All rights reserved.
//


#ifndef FINGERPRINTLOWRANK_H
#define FINGERPRINTLOWRANK_H
#include "Common.h"
#include "Spectrogram.h"
#include "MatrixUtility.h"
#include <vector>
#include "Fingerprint.h"

#define QUANTIZE_MS 52
#define STFT_B_BANDS 257 

class FingerprintLowRank {
public:
    uint quantized_time_for_frame(uint frame);
    FingerprintLowRank(Spectrogram* p16Spectrogram, Spectrogram* p512Spectrogram, int offset);
    std::vector<FPCode>& getCodes(){return _Codes;}
    void Compute();
    uint adaptiveOnsets(int ttarg, matrix_f&out, unsigned char*&band_for_onset, uint*&frame_delta_for_onset) ;
public:
    Spectrogram *_p16Spectrogram;
    Spectrogram *_p512Spectrogram;
    int _Offset;
    std::vector<FPCode> _Codes;
};

#endif
