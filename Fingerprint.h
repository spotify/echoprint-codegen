//
//  Copyright 2011 The Echo Nest. All rights reserved.
//


#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include "Common.h"
#include "Spectrogram.h"
#include "MatrixUtility.h"
#include <vector>

#define HASH_SEED 0x9ea5fa36
#define QUANTIZE_MS 52
#define LANDMARK_BITMASK 0x000fffff
#define STFT_A_BANDS 9
#define STFT_B_BANDS 257 

struct FPCode {
    FPCode() : frame(0), code(0) {}
    FPCode(uint f, int c) : frame(f), code(c) {}
    uint frame;
    uint code;
};

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed );

class Fingerprint {
public:
    uint quantized_time_for_frame(uint frame);
    Fingerprint(Spectrogram* p16Spectrogram, Spectrogram* p512Spectrogram, int offset);
    void Compute();
    uint adaptiveOnsets(int ttarg, matrix_f&out, unsigned char*&band_for_onset, uint*&frame_delta_for_onset) ;
    std::vector<FPCode>& getCodes(){return _Codes;}
public:
    Spectrogram *_p16Spectrogram;
    Spectrogram *_p512Spectrogram;
    std::vector<FPCode> _Codes;
    int _Offset;
};

#endif
