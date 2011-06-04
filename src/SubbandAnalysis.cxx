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

    // 128pt, 1/8th band low-pass prototype subsampled from Table_analysis_window
    static float C[C_LEN] = {
     0.000000477,  0.000000954,  0.000001431,  0.000002384,  0.000003815,  0.000006199,  0.000009060,  0.000013828, 
     0.000019550,  0.000027657,  0.000037670,  0.000049591,  0.000062943,  0.000076771,  0.000090599,  0.000101566, 
    -0.000108242, -0.000106812, -0.000095367, -0.000069618, -0.000027180,  0.000034332,  0.000116348,  0.000218868, 
     0.000339031,  0.000472546,  0.000611782,  0.000747204,  0.000866413,  0.000954151,  0.000994205,  0.000971317, 
    -0.000868797, -0.000674248, -0.000378609,  0.000021458,  0.000522137,  0.001111031,  0.001766682,  0.002457142, 
     0.003141880,  0.003771782,  0.004290581,  0.004638195,  0.004752159,  0.004573822,  0.004049301,  0.003134727, 
    -0.001800537, -0.000033379,  0.002161503,  0.004756451,  0.007703304,  0.010933399,  0.014358521,  0.017876148, 
     0.021372318,  0.024725437,  0.027815342,  0.030526638,  0.032754898,  0.034412861,  0.035435200,  0.035780907, 
    -0.035435200, -0.034412861, -0.032754898, -0.030526638, -0.027815342, -0.024725437, -0.021372318, -0.017876148, 
    -0.014358521, -0.010933399, -0.007703304, -0.004756451, -0.002161503,  0.000033379,  0.001800537,  0.003134727, 
    -0.004049301, -0.004573822, -0.004752159, -0.004638195, -0.004290581, -0.003771782, -0.003141880, -0.002457142, 
    -0.001766682, -0.001111031, -0.000522137, -0.000021458,  0.000378609,  0.000674248,  0.000868797,  0.000971317, 
    -0.000994205, -0.000954151, -0.000866413, -0.000747204, -0.000611782, -0.000472546, -0.000339031, -0.000218868, 
    -0.000116348, -0.000034332,  0.000027180,  0.000069618,  0.000095367,  0.000106812,  0.000108242,  0.000101566, 
    -0.000090599, -0.000076771, -0.000062943, -0.000049591, -0.000037670, -0.000027657, -0.000019550, -0.000013828, 
    -0.000009060, -0.000006199, -0.000003815, -0.000002384, -0.000001431, -0.000000954, -0.000000477, 0};
    

    _C=matrix_f(C_LEN,1);
    for(uint i=0;i<C_LEN;i++) _C(i,0) = C[i];
    
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
            Z(i,0) = _pSamples[ t*SUBBANDS + i] * _C(i,0);
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

