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
#define QUANTIZE_DT_S (256.0/11025.0)
#define QUANTIZE_A_S (256.0/11025.0)
#define HASH_BITMASK 0x000fffff
#define STFT_A_BANDS 8

struct FPCode {
    FPCode() : frame(0), code(0) {}
    FPCode(uint f, int c) : frame(f), code(c) {}
    uint frame;
    uint code;
};

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed );

class Fingerprint {
public:
    uint quantized_time_for_frame_delta(uint frame_delta);
    uint quantized_time_for_frame_absolute(uint frame);
    Fingerprint(Spectrogram* p128Spectrogram, int offset);
    void Compute();
    uint adaptiveOnsets(int ttarg, matrix_u&out, uint*&onset_counter_for_band) ;
    std::vector<FPCode>& getCodes(){return _Codes;}
protected:
    Spectrogram *_p128Spectrogram;
    int _Offset;
    std::vector<FPCode> _Codes;
};

#endif
