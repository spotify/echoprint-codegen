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
    inline uint getNumBands() const {return _Bands;}
    matrix_row_f getFrame(uint nIndex) {return row(_Data, nIndex);} 
    const matrix_f& getMatrix() {return _Data;}  
protected:

    const float* _pSamples;
    uint _NumSamples;
    uint _NumFrames;
    matrix_f _Data;
    matrix_f _C;
    uint _WLen;
    uint _Bands;
    uint _Hop;

private:
    void Init();
};

#endif
