//
//  Copyright 2011 The Echo Nest. All rights reserved.
//


#include "VectorUtility.h"
#include <stddef.h>
#include <math.h>
#include <algorithm>
#include <numeric>
#ifdef _WIN32
#include "win_unistd.h"
#endif

#if _IEEE_FLOAT_
#define MAX_FLOAT 3.4028234663852886e+38
#define MIN_FLOAT 1.1754943508222875e-38
#else
#define MAX_FLOAT 1.7014117331926443e+38
#define MIN_FLOAT 2.9387358770557188e-39
#endif


namespace VectorUtility {

bool TextFileOutput(const float* v, uint nSize, const char* filename) {
    FILE *f = fopen(filename, "w");
    bool success = (f != NULL);
    if (success) {
        for (uint j = 0; j < nSize; j++)
            fprintf(f, "%2.3f ", v[j]);
        fclose(f);
    }
    return success;
}


void finor0vec(float *aVector, uint aSize) {
    for(uint i=0;i<aSize;i++) 
        aVector[i] = finor0(aVector[i]);
}


// Finds the maximum value in a vector
float maxv(const float * aVector, uint aSize) {
    float val = -MAX_FLOAT;
    for(uint i=0;i<aSize;i++)
        if(aVector[i] > val) val = aVector[i];
    return val;
}

// Multiplies the 2 vectors, and puts the results in aResult.
void vmul(float * aResult, const float * aVector1, uint aSize) {
    for(uint i=0;i<aSize;i++)
        aResult[i] = aResult[i] * aVector1[i];
}

// Computes the multiplication of two vectors (vector1 * vector2) and puts the result in aResult
void vmul(float * aResult, const float * aVector1, const float * aVector2, uint aSize) {
    if (aResult == aVector1)
        vmul(aResult, aVector2, aSize);
    else if (aResult == aVector2)
        vmul(aResult, aVector1, aSize);
    else
        for(uint i=0;i<aSize;i++)
            aResult[i] = aVector1[i] * aVector2[i];
}

// Computes the multiplication of a vector by a scalar
void vsmul(float * aResult, const float * aVector, uint aSize, float aScalar) {
    for(uint i=0;i<aSize;i++)
        aResult[i] = aVector[i] * aScalar;
}


bool isPowerOfTwo(long n) {
    return (n != 0) && !(n & (n - 1));
}


// returns log2 for an integer which IS A POWER OF TWO. 
long log2(long nPowerOfTwo) {    
    assert(isPowerOfTwo(nPowerOfTwo));
    long ret = -1;
    while (nPowerOfTwo != 0) 
    {
        nPowerOfTwo >>= 1;
        ret++;
    }
    return ret;
}


// Computes the ceiling of log2(n)
// i.e. log2max(7) = 3, log2max(8) = 3, log2max(9) = 4
long log2max(long n) {   
    if (n==1) 
        return 0;
        
    long power = 1;
    long k = 1;    
    while ((k <<= 1) < n) 
        power++;
    return power;
}



// avoid nans and infs by replacing them w/ 0
float finor0(float in){
	// baw 7/10/2010 -- finite() not defined on iOS, this works on OSX, need to test win/lin. if not do #ifdef w/ finite()
#ifdef _WIN32
	return finite(in) ? in : 0;
#else
    return isfinite(in) ? in : 0;
#endif
}



} // namespace
