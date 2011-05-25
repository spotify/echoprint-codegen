//
//  Copyright 2011 The Echo Nest. All rights reserved.
//

#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H
#include "Common.h"
#include "Params.h"
#include "MatrixUtility.h"


#ifdef __APPLE__
	#include "TargetConditionals.h"
	#define USE_ACCELERATE 1
	#include <Accelerate/Accelerate.h>
	#include <CoreFoundation/CoreFoundation.h>
	#define fft_complex_type COMPLEX_SPLIT
	#define fft_config_type FFTSetup
#else
    extern "C" {
	    #include "fft/kiss_fft.h"
	    #include "fft/kiss_fftr.h"
        #include "fft/_kiss_fft_guts.h"
    }
    #define fft_complex_type kiss_fft_cpx
    #define fft_config_type kiss_fftr_cfg
#endif


class AudioStreamInput;

class Spectrogram {
public:
    inline Spectrogram() {};
    Spectrogram(AudioStreamInput* pAudio, uint hopSize, uint fftSize, uint windowSize);
    Spectrogram(const float* pSamples, uint numSamples, uint hopSize, uint fftSize, uint windowSize);    
    virtual ~Spectrogram();
    void SimpleFFT(float*& input);
    void SimpleSTFT();
    void Compute();
public:
    inline uint getNumFrames() const {return _NumFrames;}
    inline uint getNumBands() const {return (_FFTSize / 2) + 1;}
    matrix_row_f getFrame(uint nIndex) {return row(_Data, nIndex);} 
    const matrix_f& getMatrix() {return _Data;}  
protected:
    fft_config_type _fftr_state;
    fft_complex_type *_pBuffer;

    const float* _pSamples;
    uint _NumSamples;
    uint _NumFrames;
    matrix_f _Data;
    uint _HopSize;
    uint _FFTSize;
    uint _Log2FFTSize;
    uint _WindowSize;
private:
    void Init();
};

#endif
