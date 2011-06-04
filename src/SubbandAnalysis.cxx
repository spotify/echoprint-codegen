//
//  Copyright 2011 The Echo Nest. All rights reserved.
//


#include "SubbandAnalysis.h"
#include "AudioStreamInput.h"
#include "VectorUtility.h"

SubbandAnalysis::SubbandAnalysis(AudioStreamInput* pAudio) {
    _pSamples = pAudio->getSamples();
    _NumSamples = pAudio->getNumSamples();
    Init();
}

SubbandAnalysis::SubbandAnalysis(const float* pSamples, uint numSamples) : 
    _pSamples(pSamples), _NumSamples(numSamples) {
    Init();
}

SubbandAnalysis::~SubbandAnalysis() {
}

void SubbandAnalysis::Init() {
    // Calculate the analysis filter bank coefficients
    _Mr = matrix_f(M_ROWS, M_COLS);
    _Mi = matrix_f(M_ROWS, M_COLS);
    for (uint i = 0; i < M_ROWS; ++i) {
        for (uint k = 0; k < M_COLS; ++k) {
            _Mr(i,k) = cos((2*i + 1)*(k-4)*(M_PI/16.0));
    		_Mi(i,k) = sin((2*i + 1)*(k-4)*(M_PI/16.0));
		}
    }
    
}

void SubbandAnalysis::Compute() {
    uint t, i, j;
    
    matrix_f Z = matrix_f(C_LEN,1);
    matrix_f Y = matrix_f(M_COLS,1);
    MatrixUtility::clear(Y);
    MatrixUtility::clear(Z);
    
	_NumFrames = (_NumSamples - C_LEN + 1)/SUBBANDS;
    assert(_NumFrames > 0);

    _DataI = matrix_f(SUBBANDS, _NumFrames);
    _DataR = matrix_f(SUBBANDS, _NumFrames);
    MatrixUtility::clear(_DataI);
    MatrixUtility::clear(_DataR);

    for (t = 0; t < _NumFrames; ++t) {
        for (i = 0; i < C_LEN; ++i) {
            Z(i,0) = _pSamples[ t*SUBBANDS + i] * _C[i];
        }

        for (i = 0; i < M_COLS; ++i) {
            Y(i,0) = Z(i,0);
        }
        for (i = 0; i < M_COLS; ++i) {
            for (j = 1; j < M_ROWS; ++j) {
                Y(i,0) += Z(i + M_COLS*j,0);
    	    }
        }
        for (i = 0; i < M_ROWS; ++i) {
            for (j = 0; j < M_COLS; ++j) {
                _DataR(i, t) += _Mr(i,j) * Y(j,0);
                _DataI(i, t) -= _Mi(i,j) * Y(j,0);
            }
        }
	}

}

