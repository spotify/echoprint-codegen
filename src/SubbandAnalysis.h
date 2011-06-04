//
//  Copyright 2011 The Echo Nest. All rights reserved.
//

#ifndef SUBBANDANALYSIS_H
#define SUBBANDANALYSIS_H
#include "Common.h"
#include "Params.h"
#include "MatrixUtility.h"


#ifdef __APPLE__
	#include "TargetConditionals.h"
	#define USE_ACCELERATE 1
	#include <Accelerate/Accelerate.h>
	#include <CoreFoundation/CoreFoundation.h>
#else
    // Nothing yet
#endif

#define C_LEN 128
#define SUBBANDS 8
#define M_ROWS 8
#define M_COLS 16



class AudioStreamInput;

class SubbandAnalysis {
public:
    inline SubbandAnalysis() {};
    SubbandAnalysis(AudioStreamInput* pAudio);
    SubbandAnalysis(const float* pSamples, uint numSamples);    
    virtual ~SubbandAnalysis();
    void Compute();
public:
    inline uint getNumFrames() const {return _NumFrames;}
    inline uint getNumBands() const {return SUBBANDS;}
    const matrix_f& getMatrixI() {return _DataI;}  
    const matrix_f& getMatrixR() {return _DataR;}  
    
protected:

    const float* _pSamples;
    uint _NumSamples;
    uint _NumFrames;
    matrix_f _C;
    matrix_f _Mi;
    matrix_f _Mr;
    matrix_f _DataI;
    matrix_f _DataR;

private:
    void Init();
};

#endif
