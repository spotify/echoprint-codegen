//
//  Copyright 2011 The Echo Nest. All rights reserved.
//


#ifndef VECTORUTILITY_H
#define VECTORUTILITY_H

#include "Common.h"
#include <vector>

namespace VectorUtility {
    bool TextFileOutput(const float* v, uint nSize, const char* filename);
    void finor0vec(float *aVector, uint aSize) ;
    float maxv(const float * aVector, uint aSize);
    void vmul(float * aResult, const float * aVector1, uint aSize);
    void vmul(float * aResult, const float * aVector1, const float * aVector2, uint aSize);
    void vsmul(float * aResult, const float * aVector, uint aSize, float aScalar);
    bool isPowerOfTwo(long n);
    long log2max(long n);
    float finor0(float in);
    #ifndef log2
        long log2(long nPowerOfTwo);
    #endif
}

#endif
