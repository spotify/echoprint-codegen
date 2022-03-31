//
//  echoprint-codegen
//  Copyright 2011 The Echo Nest Corporation. All rights reserved.
//


#include "SubbandAnalysis.h"
#include "AudioStreamInput.h"

#ifdef _WIN32
#include "win_funcs.h"
#endif

SubbandAnalysis::SubbandAnalysis(const float* pSamples, uint numSamples) :
    _pSamples(pSamples), _NumSamples(numSamples) {
    Init();
}

SubbandAnalysis::~SubbandAnalysis() {
}

void SubbandAnalysis::Init() {
    // Calculate the analysis filter bank coefficients
    _Mr = matrix_f(SubbandFilterBank::M_ROWS, SubbandFilterBank::M_COLS);
    _Mi = matrix_f(SubbandFilterBank::M_ROWS, SubbandFilterBank::M_COLS);
    for (uint i = 0; i < SubbandFilterBank::M_ROWS; ++i) {
        for (uint k = 0; k < SubbandFilterBank::M_COLS; ++k) {
            _Mr(i,k) = cos((2*i + 1)*(k-4)*(M_PI/16.0));
            _Mi(i,k) = sin((2*i + 1)*(k-4)*(M_PI/16.0));
        }
    }
}

void SubbandAnalysis::Compute() {
    uint t, i, j;

    float Z[SubbandFilterBank::C_LEN];
    float Y[SubbandFilterBank::M_COLS];

    _NumFrames = (_NumSamples - SubbandFilterBank::C_LEN + 1)/ SubbandFilterBank::SUBBANDS;
    assert(_NumFrames > 0);

    _Data = matrix_f(SubbandFilterBank::SUBBANDS, _NumFrames);

    for (t = 0; t < _NumFrames; ++t) {
        for (i = 0; i < SubbandFilterBank::C_LEN; ++i) {
            Z[i] = _pSamples[ t*SubbandFilterBank::SUBBANDS + i] * SubbandFilterBank::C[i];
        }

        for (i = 0; i < SubbandFilterBank::M_COLS; ++i) {
            Y[i] = Z[i];
        }
        for (i = 0; i < SubbandFilterBank::M_COLS; ++i) {
            for (j = 1; j < SubbandFilterBank::M_ROWS; ++j) {
                Y[i] += Z[i + SubbandFilterBank::M_COLS*j];
            }
        }
        for (i = 0; i < SubbandFilterBank::M_ROWS; ++i) {
            float Dr = 0, Di = 0;
            for (j = 0; j < SubbandFilterBank::M_COLS; ++j) {
                Dr += _Mr(i,j) * Y[j];
                Di -= _Mi(i,j) * Y[j];
            }
            _Data(i,t) = Dr*Dr + Di*Di;
        }
    }
}

